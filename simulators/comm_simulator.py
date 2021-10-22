import json
import socket
import mosaik_api
from simpy.io.codec import JSON

from config import STEP_SIZE_1_MS, CONNECT_ATTR
from message_pb2 import CosimaMsgGroup
from simulators.omnetpp_connection import OmnetppConnection

META = {
    'type': 'event-based',
    'models': {
        'CommModel': {
            'public': True,
            'any_inputs': False,
            'params': [],
            'attrs': ['message',  # input
                      ],
        },
    },
}


class CommModel:
    """Model for Comm_Simulator"""

    def __init__(self, m_id):
        self._m_id = m_id
        self._current_messages = []

    @property
    def messages(self):
        return self._current_messages

    @messages.setter
    def messages(self, msg):
        self._current_messages = msg

    def step(self, message):
        """step-method of the model"""
        self._current_messages.append(message)

    def get_data(self):
        """get_data-method of model"""
        output = self._current_messages
        self._current_messages = []
        return output


class CommSimulator(mosaik_api.Simulator):
    """Comm-Simulator enables connection towards OMNeT, synchronizes schedulers
     and manages a communication-delayed call of step-methods for
      other simulators """

    def __init__(self):
        super().__init__(META)
        self.sid = 'CommSim'
        self.eid = None
        self._omnetpp_connection = None
        self.event_queue = None
        self.data = None
        self.output_time = None
        self._node_connections = {}
        self._models = {}
        self._model_names = []
        self._step_size = None
        self._received_answer = True
        self._number_messages_sent = 0
        self._number_messages_received = 0
        self._sid = None
        self._waining_msgs_counter = 0

    def init(self, sid, config_file, step_size):
        self.sid = sid

        with open(config_file, "r") as jsonfile:
            config = json.load(jsonfile)

        if 'observer_port' in config:
            observer_port = config['observer_port']

        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connection_eids = set()
        connections = {}

        for name in config['connections']:
            src, dest = name.split('->')

            connection_eids.add(name)
            connections[src] = {
                dest: {'delay': 2}}  # TODO: use setdefault?

        self._model_names = config['nodes']

        self._step_size = step_size
        self._omnetpp_connection = OmnetppConnection(observer_port,
                                                     client_socket,
                                                     connection_eids,
                                                     connections)
        # TODO only acc to connections
        for model_name, information in self.meta["models"].items():
            for agent_name in self._model_names:
                self.meta["models"][model_name]['attrs'].append(
                    f'message_with_delay_for_{agent_name}')
                self._models[agent_name] = CommModel(m_id=agent_name)

        return self.meta

    def create(self, num, model):
        if num > 1:
            raise RuntimeError(
                'Can only create one instance of CommSimulator.')

        self.event_queue = []
        # own id
        self._sid = 'CommSim'

        return [{'eid': self._sid, 'type': model}]

    def create_protobuf_msg(self, messages, sim_time):
        """creates protobuf message from given dictionary"""
        msg_group = CosimaMsgGroup()
        for message, size in messages:
            msg = msg_group.msg.add()
            msg.type = message["type"]
            msg.sender = message["sender"]
            msg.receiver = message["receiver"]
            msg.max_advance = self.calculate_to_ms(message["max_advance"])
            msg.size = size
            msg.content = message["message"]
            msg.simTime = self.calculate_to_ms(sim_time)
            msg.stepSize = self._step_size
            msg.msgId = message['msgId']
        return msg_group

    def receive_messages_from_omnetpp(self, simtime, max_advance):
        """
        Method to connect to omnetpp to receive messages. If messages
        are received, they are returned. If the CommunicationSimulator
        is still waiting for further messages, this method also returns
        the delay of the received message.
        :param simtime:
        :param max_advance:
        :return:
        """
        answers = []
        waiting_for_msg = True
        waiting_msg_sent = False

        while waiting_for_msg:
            msgs = self._omnetpp_connection.return_messages()
            if len(msgs) == 0:
                if not self._received_answer and (
                        max_advance == self.mosaik.world.until):
                    if not waiting_msg_sent and max_advance != \
                            self.mosaik.world.until:
                        self.send_waiting_msg(max_advance, simtime)
                        waiting_msg_sent = True
                elif not self._received_answer and not max_advance == \
                                                       self.mosaik.world.until:
                    return answers
                elif self._received_answer:
                    return answers
            else:
                received_info_msg = False
                for msg in msgs:
                    if msg.type == CosimaMsgGroup.CosimaMsg.MsgType.INFO:
                        received_info_msg = True
                        self._number_messages_received += 1
                        if self._number_messages_received == \
                                self._number_messages_sent:
                            self._number_messages_received = 0
                            self._number_messages_sent = 0
                            self._received_answer = True
                        answers.append(msg)
                    elif msg.type == CosimaMsgGroup.CosimaMsg.MsgType.TRANSMISSION_ERROR:
                        print('case received transmission error')
                        self._number_messages_sent -= 1
                        if self._number_messages_received == \
                                self._number_messages_sent:
                            self._number_messages_received = 0
                            self._number_messages_sent = 0
                            self._received_answer = True
                        self.send_waiting_msg(max_advance, simtime + 1)
                        waiting_msg_sent = True
                    else:
                        self.send_waiting_msg(max_advance, simtime)
                if received_info_msg:
                    return answers

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

        # send msg via omnetpp connection
        if self._omnetpp_connection.is_still_connected:
            self._omnetpp_connection.send_message(msg)
        else:
            self.finalize()

    def send_waiting_msg(self, max_advance, simtime):
        # send out waiting message
        waiting_msg = {
            'type': CosimaMsgGroup.CosimaMsg.MsgType.WAITING,
            'sender': 'CommSim',
            'receiver': '',
            'max_advance': self.calculate_to_ms(max_advance),
            'message': 'Waiting for another msg',
            'simTime': self.calculate_to_ms(simtime),
            'stepSize': self._step_size,
            'msgId': f'WaitingMessage_{self._waining_msgs_counter}'}
        self._waining_msgs_counter += 1
        size = len(JSON().encode(waiting_msg))
        self.send_message_to_omnetpp([(waiting_msg, size)], simtime)

    @staticmethod
    def calculate_to_used_step_size(value):
        """calculates step-size based on delay"""
        value = value * 1000
        # needs to be divisible by step size
        value = int(value - (value % STEP_SIZE_1_MS))
        return value

    @staticmethod
    def calculate_to_ms(value):
        """calculates value to milliseconds for OMNeT++"""
        value = value * STEP_SIZE_1_MS
        return value

    def step(self, time, inputs, max_advance):
        next_step = None
        received_messages = []

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
            self._number_messages_sent += len(messages_to_send)
            self._received_answer = False
            answers = self.receive_messages_from_omnetpp(time, max_advance)
            received_messages.extend(answers)
        if received_messages:
            for reply in received_messages:
                self.process_msg_from_omnet(reply, time)

        self.data = {}

        return next_step

    def process_msg_from_omnet(self, reply, time):
        """processes answer messages from omnet, distinguishes between
        scheduler synchronization (FES) and communication delay """
        if type(reply) is dict:
            print('type of message is dict. This should not be the case')
        elif type(reply) is CosimaMsgGroup.CosimaMsg:
            if time > reply.simTime:
                raise RuntimeError(
                    'Simulation time in OMNeT++ is is behind the simulation '
                    'time in mosaik.')
            # determine delay in milliseconds for mosaik
            omnetpp_delay = CommSimulator.calculate_to_used_step_size(
                reply.delay)

            msg_as_dict = {'sender': reply.sender,
                           'receiver': reply.receiver,
                           'message': reply.content,
                           'delay': omnetpp_delay,
                           'output_time': reply.simTime
                           }

            self._models[reply.receiver].step(msg_as_dict)
        else:
            print('message has type: ', type(reply))

    def get_data(self, outputs):
        """gets data of entities for mosaik core"""
        data = {}
        time = None
        for eid, model in self._models.items():
            model_data = model.get_data()
            if len(model_data) > 0:
                data[f'{CONNECT_ATTR}{eid}'] = model_data
                for msg in model_data:
                    if time is None:
                        time = msg['output_time']
                    elif time != msg['output_time']:
                        print('There are messages with different output '
                              'times in det_data! ')
                        if time < msg['output_time']:
                            time = msg['output_time']
        # output time is the lowest value of all times in messages to
        # make sure the agents receive the messages in time
        output_object = {self._sid: data}
        if time is not None:
            output_object['time'] = time
        return output_object

    def finalize(self):
        """last step of comm-simulator"""
        self._omnetpp_connection.close_connection()


if __name__ == '__main__':
    mosaik_api.start_simulation(CommSimulator())
