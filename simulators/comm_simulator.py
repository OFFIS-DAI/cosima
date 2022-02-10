import socket

import mosaik_api
from simpy.io.codec import JSON

from config import USED_STEP_SIZE, CONNECT_ATTR
from message_pb2 import CosimaMsgGroup
from simulators.omnetpp_connection import OmnetppConnection
from util_functions import log

META = {
    'type': 'event-based',
    'models': {
        'CommModel': {
            'public': True,
            'any_inputs': False,
            'params': [],
            'attrs': ['message',  # input
                      'next_step',  # input
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
        self._is_first_step = True
        self._next_agent_steps = []
        self._is_in_waiting_modus = False
        self._current_time = 0

    def init(self, sid, step_size, port, agent_names):
        self.sid = sid

        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self._model_names = agent_names

        self._step_size = step_size
        self._omnetpp_connection = OmnetppConnection(port,
                                                     client_socket)
        self._omnetpp_connection.start_connection()
        # TODO only acc to connections
        for model_name, information in self.meta["models"].items():
            for agent_name in self._model_names:
                self.meta["models"][model_name]['attrs'].append(
                    f'message_with_delay_for_{agent_name}')
                self._models[agent_name] = CommModel(m_id=agent_name)

        self.meta["models"][model_name]['attrs'].append(
            f'ctrl_message')
        self._models['ict_controller'] = CommModel(m_id='ict_controller')

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
            msg.msgId = message['msgId']
            msg.simTime = self.calculate_to_ms(sim_time)
            msg.creation_time = self.calculate_to_ms(sim_time)
            if message["type"] is CosimaMsgGroup.CosimaMsg.MsgType.DISCONNECT or message[
                "type"] is CosimaMsgGroup.CosimaMsg.MsgType.RECONNECT:
                msg.change_module = message["module_name"]
                msg.simTime = self.calculate_to_ms(message["simTime"])
                msg.creation_time = self.calculate_to_ms(sim_time)
            else:
                msg.simTime = self.calculate_to_ms(sim_time)
                msg.creation_time = self.calculate_to_ms(sim_time)
                msg.sender = message["sender"]
                msg.receiver = message["receiver"]
                msg.max_advance = self.calculate_to_ms(message["max_advance"])
                msg.size = size
                msg.content = message["message"]
                msg.stepSize = self._step_size
                msg.until = self.calculate_to_ms(self.mosaik.world.until)
                msg.change_module = ""
        return msg_group

    @staticmethod
    def calculate_correct_msg_size(msg):
        if msg['type'] is not CosimaMsgGroup.CosimaMsg.MsgType.RECONNECT \
                and msg['type'] \
                is not CosimaMsgGroup.CosimaMsg.MsgType.DISCONNECT:
            minimal_dict = {'receiver': msg['receiver'],
                            'sender': msg['sender'],
                            'message': msg['message']}
            return len(JSON().encode(minimal_dict))
        else:
            return 0

    def remove_old_steps(self, sim_time):
        self._next_agent_steps = [x for x in self._next_agent_steps if
                                  x >= sim_time]
        self._next_agent_steps.sort()

    def next_agent_step(self, sim_time):
        self.remove_old_steps(sim_time)

        return self._next_agent_steps[0] if len(
            self._next_agent_steps) > 0 else None

    def receive_messages_from_omnetpp(self, sim_time, max_advance):
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
        next_step = None
        next_agent_step = self.next_agent_step(sim_time)

        while waiting_for_msg:
            msgs = self._omnetpp_connection.return_messages()
            if len(msgs) == 0:
                if self._received_answer:
                    return answers, next_step
                if max_advance == next_agent_step:
                    # waiting msg?
                    return answers, next_step
                if max_advance == sim_time and self._is_in_waiting_modus:
                    return answers, sim_time + 1
                else:
                    # wait
                    self._is_in_waiting_modus = True
                    if not waiting_msg_sent:
                        self.send_waiting_msg(max_advance, sim_time)
                        waiting_msg_sent = True

            else:
                received_info_msg = False
                for idx, msg in enumerate(msgs):
                    if msg.type == CosimaMsgGroup.CosimaMsg.MsgType.INFO:
                        self._is_in_waiting_modus = False
                        received_info_msg = True
                        self._number_messages_received += 1
                        if self._number_messages_received == \
                                self._number_messages_sent:
                            self._number_messages_received = 0
                            self._number_messages_sent = 0
                            self._received_answer = True
                            next_step = msg.simTime + 1
                        answers.append(msg)
                    elif msg.type == CosimaMsgGroup.CosimaMsg.MsgType. \
                            TRANSMISSION_ERROR:
                        log('case received transmission error')
                        self._number_messages_sent -= 1
                        if self._number_messages_received == \
                                self._number_messages_sent:
                            self._number_messages_received = 0
                            self._number_messages_sent = 0
                            self._received_answer = True
                        self.send_waiting_msg(max_advance, sim_time + 1)
                        waiting_msg_sent = True
                    elif msg.type == CosimaMsgGroup.CosimaMsg.MsgType. \
                            DISCONNECT or msg.type == CosimaMsgGroup.CosimaMsg.MsgType. \
                            RECONNECT:
                        log('case received disconnect notification')
                        self._number_messages_sent -= 1
                        answers.append(msg)
                        if idx == len(msgs) - 1:
                            return answers, next_step
                    elif msg.type == CosimaMsgGroup.CosimaMsg.MsgType.MAX_ADVANCE:
                        # if max advance message is not the last message,
                        # ignore it
                        if idx == len(msgs) - 1:
                            log(f'received max advance event at time {sim_time}'
                                  f' and max_advance {max_advance}. Event contains'
                                  f' time: {msg.simTime}')
                            # do we need waiting modus?
                            if sim_time < msg.simTime:
                                # max advance already reached
                                return answers, msg.simTime
                            elif sim_time == msg.simTime:
                                log('simtime == msg.simtime')
                                return answers, msg.simTime + 1
                            else:
                                log(f'mosaik time: {sim_time}, max advance'
                                      f' time from OMNeT: {msg.simTime}')
                                raise ValueError
                if received_info_msg:
                    return answers, next_step

    def send_message_to_omnetpp(self, messages, sim_time):
        """
        Method to send message to omnetpp. Takes messages, sim_time and
        max_advance and transfers this information to omnetpp.

        :param messages: messages to be send
        :param sim_time: current mosaik sim_time
        :param max_advance: current max advance according to mosaik
        """
        proto_message = self.create_protobuf_msg(messages, sim_time)
        msg = proto_message.SerializeToString()

        # send msg via omnetpp connection
        if self._omnetpp_connection.is_still_connected:
            self._omnetpp_connection.send_message(msg)
        else:
            log('Error with omnet connection')
            self.finalize()

    def send_initial_message(self, sim_time):
        # send out waiting message
        initial_msg = {
            'type': CosimaMsgGroup.CosimaMsg.MsgType.CMD,
            'sender': 'CommSim',
            'receiver': '',
            'max_advance': 0,
            'message': 'Inform about simulation end time in mosaik',
            'simTime': 0,
            'stepSize': self._step_size,
            'msgId': f'InitialMessage'}
        size = len(JSON().encode(initial_msg))
        log("send initial message")
        self.send_message_to_omnetpp([(initial_msg, size)], sim_time)

    def send_waiting_msg(self, max_advance, sim_time):
        # send out waiting message
        waiting_msg = {
            'type': CosimaMsgGroup.CosimaMsg.MsgType.WAITING,
            'sender': 'CommSim',
            'receiver': '',
            'max_advance': self.calculate_to_ms(max_advance),
            'message': 'Waiting for another msg',
            'simTime': self.calculate_to_ms(sim_time),
            'stepSize': self._step_size,
            'msgId': f'WaitingMessage_{self._waining_msgs_counter}'}
        log(f'WaitingMessage_{self._waining_msgs_counter}')
        self._waining_msgs_counter += 1
        size = self.calculate_correct_msg_size(waiting_msg)
        self.send_message_to_omnetpp([(waiting_msg, size)], sim_time)

    @staticmethod
    def calculate_to_used_step_size(value):
        """calculates step-size based on delay"""
        value = value * 1000
        # needs to be divisible by step size
        value = int(value - (value % USED_STEP_SIZE))
        return value

    @staticmethod
    def calculate_to_ms(value):
        """calculates value to milliseconds for OMNeT++"""
        return value * USED_STEP_SIZE

    def step(self, time, inputs, max_advance):
        self._current_time = time
        if self._is_first_step:
            self.send_initial_message(time)
            self._is_first_step = False
        next_step = None
        received_messages = []

        messages_to_send = []
        for eid, attr_names in inputs.items():
            for attribute, sources_to_values in attr_names.items():
                for values in sources_to_values.values():
                    if type(values) is list:
                        for value in values:
                            message_size = self.calculate_correct_msg_size(
                                value)
                            value['max_advance'] = max_advance
                            messages_to_send.append((value, message_size))
                    else:
                        raise ValueError(
                            f'Type error of msgs! Received msg from mosaik'
                            f'with type {type(values)}')

        log(f'Comm Sim steps in {time} with input {messages_to_send}')

        if len(messages_to_send) > 0:
            self.send_message_to_omnetpp(messages_to_send, time)
            self._number_messages_sent += len(messages_to_send)
            self._received_answer = False
            answers, next_step = self.receive_messages_from_omnetpp(
                time, max_advance)
            received_messages.extend(answers)
        elif not self._received_answer:
            self.send_waiting_msg(max_advance, time)
            answers, next_step = self.receive_messages_from_omnetpp(
                time, max_advance)
            received_messages.extend(answers)

        if received_messages:
            for reply in received_messages:
                self.process_msg_from_omnet(reply, time)

        self.data = {}

        if next_step and next_step >= self.mosaik.world.until:
            log('call finalize.')
            self.finalize()
        return next_step

    def process_msg_from_omnet(self, reply, time):
        """processes answer messages from omnet, distinguishes between
        scheduler synchronization (FES) and communication delay """
        if type(reply) is CosimaMsgGroup.CosimaMsg:
            if time > reply.simTime:
                raise RuntimeError(
                    'Simulation time in OMNeT++ is is behind the simulation '
                    'time in mosaik.')
            if reply.type is CosimaMsgGroup.CosimaMsg.RECONNECT or reply.type \
                    is CosimaMsgGroup.CosimaMsg.DISCONNECT:
                msg_as_dict = {'sender': reply.sender,
                               'creation_time': reply.creation_time,
                               'connection_change_successful':
                                   reply.connection_change_successful,
                               'connection_change_type': reply.type,
                               'output_time': reply.simTime,
                               'msgId': reply.msgId
                               }
                self._models['ict_controller'].step(msg_as_dict)
            else:
                # determine delay in milliseconds for mosaik
                omnetpp_delay = CommSimulator.calculate_to_used_step_size(
                    reply.delay)

                msg_as_dict = {'sender': reply.sender,
                               'receiver': reply.receiver,
                               'message': reply.content,
                               'delay': omnetpp_delay,
                               'creation_time': reply.creation_time,
                               'output_time': reply.simTime,
                               'msgId': reply.msgId
                               }
                self._models[reply.receiver].step(msg_as_dict)
        else:
            raise ValueError(
                f'Message from OMNeT has invalid type {type(reply)}')

    def get_data(self, outputs):
        """gets data of entities for mosaik core"""
        data = {}
        time = None
        for eid, model in self._models.items():
            model_data = model.get_data()
            if len(model_data) > 0:
                if eid == 'ict_controller':
                    data['ctrl_message'] = model_data
                else:
                    data[f'{CONNECT_ATTR}{eid}'] = model_data
                for msg in model_data:
                    if time is None:
                        time = msg['output_time']
                        # if time == self._current_time:
                        #     print('would be the same step again!')
                    elif time != msg['output_time']:
                        log('There are messages with different output '
                              'times in det_data! ')
                        if time > msg['output_time']:
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
