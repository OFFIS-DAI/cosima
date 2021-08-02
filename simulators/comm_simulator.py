"""
TODO
"""
import json
import socket

import heapq as hq
from typing import Dict, Any

import mosaik_api
from simpy.io.codec import JSON
from message_pb2 import CosimaMsgGroup

from util import STEP_SIZE_1_MS

META = {
    'type': 'event-based',
    'models': {
        'CommModel': {
            'public': True,
            'any_inputs': False,
            'params': [],
            'attrs': ['message',  # input
                      'message_with_delay',  # output
                      ],
        },
    },
}


class CommModel:
    """Model for Comm_Simulator"""

    def __init__(self, m_id):
        self._m_id = m_id
        self._current_message = None

    @property
    def message(self):
        return self._current_message

    @message.setter
    def message(self, msg):
        self._current_message = msg

    def step(self, message):
        """step-method of the model"""
        self._current_message = message

    def get_data(self):
        """get_data-method of model"""
        output = self._current_message
        self._current_message = None
        return output


class CommSimulator(mosaik_api.Simulator):
    """Comm-Simulator enables connection towards OMNeT, synchronizes schedulers and manages a
    communication-delayed call of step-methods for other simulators """

    def __init__(self):
        super().__init__(META)
        self.sid = 'CommSim'
        self.eid = None
        self.connections = {}
        self.connection_eids = set()
        self.observer_port = 0
        self.client_socket = None

        self.event_queue = None
        self.data = None
        self.output_time = None
        self._current_output = {}
        self._node_connections = {}
        self._models = {}
        self._model_names = []
        self._current_msg = False
        self._delay_infos_expected = 0

    def init(self, sid, config_file):
        self.sid = sid

        with open(config_file, "r") as jsonfile:
            config = json.load(jsonfile)

        if 'observer_port' in config:
            self.observer_port = config['observer_port']

        for name in config['connections']:
            src, dest = name.split('->')

            self.connection_eids.add(name)
            self.connections[src] = {
                dest: {'delay': 2}}  # TODO: use setdefault?

        self._model_names = config['nodes']

        return self.meta

    def create(self, num, model):
        if num > 1:
            raise RuntimeError(
                'Can only create one instance of CommSimulator.')

        self.event_queue = []

        if self._model_names:
            m_id = self._model_names.pop(0)
            self._models[m_id] = CommModel(m_id=m_id)
        else:
            raise AttributeError('No ID left to create a model!')

        return [{'eid': m_id, 'type': model}]

    @staticmethod
    def create_protobuf_msg(messages, sim_time, step_size):
        """creates protobuf message from given dictionary"""
        msg_group = CosimaMsgGroup()
        for message, size in messages:
            msg = msg_group.msg.add()
            msg.type = message["type"]
            msg.sender = message["sender"]
            msg.receiver = message["receiver"]
            msg.max_advance = message["max_advance"]
            msg.size = size
            msg.content = message["message"]
            msg.simTime = sim_time
            msg.stepSize = step_size
        return msg_group

    def send_message_to_omnetpp(self, messages, simtime, step_size,
                                max_advance=None):
        """set up connection to OMNeT++"""
        servername = "127.0.0.1"
        dest_port = self.observer_port
        with socket.socket(socket.AF_INET,
                           socket.SOCK_STREAM) as self.client_socket:
            self.client_socket.connect((servername, dest_port))

            proto_message = CommSimulator.create_protobuf_msg(messages,
                                                              simtime,
                                                              step_size)
            msg = proto_message.SerializeToString()
            print('send msg to omnet')
            self.client_socket.send(msg)
            received_answer = False
            answers = []
            number_messages_sent = len(messages) + self._delay_infos_expected
            self._delay_infos_expected = 0
            number_messages_received = 0

            while not received_answer:
                try:
                    data = self.client_socket.recv(4096)
                    msg_group = CosimaMsgGroup()
                    msg_group.ParseFromString(data)
                    msg = msg_group.msg[0]
                    print('received msg', msg.type)

                    if msg.type == CosimaMsgGroup.CosimaMsg.MsgType.INFO:
                        number_messages_received += 1
                        if number_messages_received == number_messages_sent:
                            received_answer = True
                        answers.append({'sender': msg.sender,
                                        'receiver': msg.receiver,
                                        'message': msg.content,
                                        'delay': msg.delay
                                        })
                        if self._delay_infos_expected > 0:
                            self._delay_infos_expected -= 1
                    else:
                        self._delay_infos_expected += 1
                    if not received_answer:
                        self.send_waiting_msg(max_advance, simtime, step_size)

                except Exception as current_exception:
                    print(current_exception)
                    print("Connection to OMNeT++ closed.")
                    self.finalize()

        return answers

    def send_waiting_msg(self, max_advance, simtime, step_size):
        # send out waiting message
        waiting_msg = {
            'type': CosimaMsgGroup.CosimaMsg.MsgType.WAITING,
            'sender': 'CommSim',
            'receiver': '',
            'max_advance': max_advance,
            'message': 'Waiting for another msg',
            'simTime': simtime,
            'stepSize': step_size}
        size = len(JSON().encode(waiting_msg))
        proto_message = CommSimulator.create_protobuf_msg(
            [(waiting_msg, size)],
            simtime,
            step_size)
        msg = proto_message.SerializeToString()
        print('send waiting msg to omnet')
        self.client_socket.send(msg)

    @staticmethod
    def calculate_to_used_step_size(value):
        """calculates step-size based on delay"""
        value = value * 1000
        # needs to be divisible by step size
        value = int(value - (value % STEP_SIZE_1_MS))
        return value

    def step(self, time, inputs, max_advance):
        messages_to_send = []
        for eid, attr_names in inputs.items():
            for attribute, sources_to_values in attr_names.items():
                for values in sources_to_values.values():
                    message_size = len(JSON().encode(values))
                    messages_to_send.append((values, message_size))

        print('Comm Sim steps in ', time, ' with input ', messages_to_send)

        if len(messages_to_send) > 0:
            replys = self.send_message_to_omnetpp(messages_to_send, time,
                                                  STEP_SIZE_1_MS, max_advance)
            if replys:
                for reply in replys:
                    self.process_msg_from_omnet(reply, inputs, time)

        self.data = {}

        # If there are events left, request a new step:
        if self.event_queue:
            next_step = self.event_queue[0][0]
            _, eid, attr, value = hq.heappop(self.event_queue)
        else:
            next_step = None

        print('next step', next_step)

        return next_step

    def process_msg_from_omnet(self, reply, inputs, time):
        """processes answer messages from omnet, distinguishes between
        scheduler synchronization (FES) and communication delay """
        if type(reply) is dict:
            # determine delay in milliseconds for mosaik
            omnetpp_delay = CommSimulator.calculate_to_used_step_size(
                reply['delay'])
            print('delay ', str(omnetpp_delay))

            reply['delay'] = omnetpp_delay
            reply['output_time'] = time + omnetpp_delay
            if 'receiver' in reply:
                self._current_output[reply['receiver']] = reply
                self._models[reply['sender']].step(reply)
            for eid, attrs in inputs.items():
                for attr, value_dict in attrs.items():
                    for src_full_id, value in value_dict.items():
                        for dest, props in self.connections[eid].items():
                            factor = props.get('factor', None)
                            if factor is not None:
                                value = value * factor
                            offset = props.get('offset', None)
                            if offset is not None:
                                value = value + offset
                            if omnetpp_delay >= 0:
                                arrival_time = time + omnetpp_delay
                                if not self.event_queue or self.event_queue[0][
                                    0] != arrival_time:
                                    hq.heappush(self.event_queue,
                                                (arrival_time, dest, attr,
                                                 value))

    def get_data(self, outputs):
        """gets data of entities for mosaik core"""
        data = {}
        for eid, value in outputs.items():
            model_data = self._models[eid].get_data()
            if model_data is not None:
                data[eid] = {'message_with_delay': model_data}
                data['time'] = int(model_data['output_time'])

        return data

    def finalize(self):
        """last step of comm-simulator"""
        pass


if __name__ == '__main__':
    mosaik_api.start_simulation(CommSimulator())
