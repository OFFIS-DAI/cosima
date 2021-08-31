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
        self._current_message = []

    @property
    def message(self):
        return self._current_message

    @message.setter
    def message(self, msg):
        self._current_message = msg

    def step(self, message):
        """step-method of the model"""
        self._current_message.append(message)

    def get_data(self):
        """get_data-method of model"""
        output = self._current_message
        self._current_message = []
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
        self._node_connections = {}
        self._models = {}
        self._model_names = []
        self._current_msg = False
        self._step_size = None
        self._received_answer = True
        self._number_messages_sent = 0
        self._number_messages_received = 0
        self._servername = "127.0.0.1"

    def init(self, sid, config_file, step_size):
        self.sid = sid

        with open(config_file, "r") as jsonfile:
            config = json.load(jsonfile)

        if 'observer_port' in config:
            self.observer_port = config['observer_port']

        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        for name in config['connections']:
            src, dest = name.split('->')

            self.connection_eids.add(name)
            self.connections[src] = {
                dest: {'delay': 2}}  # TODO: use setdefault?

        self._model_names = config['nodes']

        self._step_size = step_size

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

    def create_protobuf_msg(self, messages, sim_time):
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
            msg.stepSize = self._step_size
        return msg_group

    def receive_message_from_omnetpp(self, simtime, max_advance):
        """
        Method to connect to omnetpp to receive messages. If messages
        are received, they are returned. If the CommunicationSimulator
        is still waiting for further messages, this method also returns
        the delay of the received message.
        :param simtime:
        :param max_advance:
        :return:
        """
        try:
            self.client_socket.connect((self._servername, self.observer_port))
        except OSError:
            # client is still connected
            pass
        self._received_answer = False
        answers = []
        default_delay = None

        timeout_count = 3
        while not self._received_answer and timeout_count > 0:
            try:
                data = self.client_socket.recv(4096)
                msg_group = CosimaMsgGroup()
                msg_group.ParseFromString(data)
                msg = msg_group.msg[0]
                print('received msg', msg.type)

                if msg.type == CosimaMsgGroup.CosimaMsg.MsgType.INFO:
                    self._number_messages_received += 1
                    if self._number_messages_received == \
                            self._number_messages_sent:
                        self._received_answer = True
                        self._number_messages_received = 0
                        self._number_messages_sent = 0
                    default_delay = msg.delay
                    answers.append(msg)

                if not self._received_answer:
                    self.send_waiting_msg(max_advance, simtime)
                    if msg.type == CosimaMsgGroup.CosimaMsg.MsgType.INFO:
                        return answers, default_delay

            except Exception as current_exception:
                timeout_count -= 1
                print(current_exception)
                print("Connection to OMNeT++ closed.")
                self.finalize()

        return answers, default_delay

    def send_message_to_omnetpp(self, messages, simtime):
        """
        Method to send message to omnetpp. Takes messages, simtime and
        max_advance and transfers this information to omnetpp.

        :param messages: messages to be send
        :param simtime: current mosaik simtime
        :param max_advance: current max advance according to mosaik
        """
        proto_message = self.create_protobuf_msg(messages, simtime)
        msg = proto_message.SerializeToString()
        self._number_messages_sent += len(messages)
        print('send msg to omnet')
        try:
            self.client_socket.connect((self._servername, self.observer_port))
        except OSError:
            # client is still connected
            pass

        self.client_socket.send(msg)

    def send_waiting_msg(self, max_advance, simtime):
        # send out waiting message
        waiting_msg = {
            'type': CosimaMsgGroup.CosimaMsg.MsgType.WAITING,
            'sender': 'CommSim',
            'receiver': '',
            'max_advance': max_advance,
            'message': 'Waiting for another msg',
            'simTime': simtime,
            'stepSize': self._step_size}
        size = len(JSON().encode(waiting_msg))
        proto_message = self.create_protobuf_msg(
            [(waiting_msg, size)],
            simtime)
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
        next_step = None
        received_messages = []
        if not self._received_answer:
            print('still waiting for another message')
            answers, delay = self.receive_message_from_omnetpp(time,
                                                               max_advance)
            received_messages.extend(answers)
            if delay is not None:
                delay = self.calculate_to_used_step_size(delay)
                delay += time
            next_step = delay
        messages_to_send = []
        for eid, attr_names in inputs.items():
            for attribute, sources_to_values in attr_names.items():
                for values in sources_to_values.values():
                    if type(values) is list:
                        for value in values:
                            message_size = len(JSON().encode(value))
                            messages_to_send.append((value, message_size))
                    else:
                        message_size = len(JSON().encode(values))
                        messages_to_send.append((values, message_size))

        print('Comm Sim steps in ', time, ' with input ', messages_to_send)

        if len(messages_to_send) > 0:
            self.send_message_to_omnetpp(messages_to_send, time)
            # TODO: do we even want to call receive_message. Maybe its
            #  the second time (see l 238)
            answers, delay = self.receive_message_from_omnetpp(time,
                                                               max_advance)
            received_messages.extend(answers)
            if delay is not None:
                delay = self.calculate_to_used_step_size(delay)
                delay += time
                if next_step is None:
                    next_step = delay
                elif delay < next_step:
                    next_step = delay
        if received_messages:
            for reply in received_messages:
                self.process_msg_from_omnet(reply, time)

        self.data = {}

        # If there are events left, request a new step:
        if self.event_queue:
            for event in self.event_queue:
                if event[0] > time:
                    possible_next_step = event[0]
                    if possible_next_step is not None:
                        if possible_next_step < next_step:
                            next_step = possible_next_step
                            self.event_queue.pop()
                            break
        print('next step', next_step)
        if next_step == time:
            next_step = None
        return next_step

    def process_msg_from_omnet(self, reply, time):
        """processes answer messages from omnet, distinguishes between
        scheduler synchronization (FES) and communication delay """
        if type(reply) is dict:
            print('type of message is dict. This should not be the case')
        elif type(reply) is CosimaMsgGroup.CosimaMsg:
            if time > reply.simTime:
                raise RuntimeError(
                    'Simulation time in OMNeT++ is is behind the simulation time in mosaik.')
            # determine delay in milliseconds for mosaik
            omnetpp_delay = CommSimulator.calculate_to_used_step_size(
                reply.delay)
            print('delay ', omnetpp_delay)

            msg_as_dict = {'sender': reply.sender,
                           'receiver': reply.receiver,
                           'message': reply.content,
                           'delay': omnetpp_delay,
                           'output_time': reply.simTime
                           }

            self._models[reply.sender].step(msg_as_dict)
        else:
            print('message has type: ', type(reply))

    def get_data(self, outputs):
        """gets data of entities for mosaik core"""
        data = {}
        for eid, value in outputs.items():
            model_data = self._models[eid].get_data()
            if len(model_data) > 0:
                # TODO wenn Nachrichten unterschiedliche Delays haben
                data[eid] = {'message_with_delay': model_data}
                if 'time' not in data.keys() or int(model_data[0]['output_time']) < data['time']:
                    data['time'] = int(model_data[0]['output_time'])
                    print(f'set output time to {data["time"]}')

        return data

    def finalize(self):
        """last step of comm-simulator"""
        pass


if __name__ == '__main__':
    mosaik_api.start_simulation(CommSimulator())
