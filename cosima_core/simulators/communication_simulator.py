import socket

import mosaik_api

from cosima_core.config import USED_STEP_SIZE
from cosima_core.messages.message_pb2 import InitialMessage, InfoMessage, SynchronisationMessage, \
    InfrastructureMessage
from cosima_core.simulators.omnetpp_connection import OmnetppConnection
from cosima_core.util.util_functions import log, create_protobuf_msg, get_dict_from_protobuf_message

META = {
    'models': {
        'CommunicationModel': {
            'public': True,
            'any_inputs': False,
            'params': [],
            'attrs': ['message',  # input
                      'next_step',  # input
                      'ict_message'
                      ],
        },
    }, 'type': 'event-based'}


class CommunicationModel:
    """Model for CommunicationSimulator"""

    def __init__(self, m_id, connect_attr):
        self._m_id = m_id
        self._connect_attr = connect_attr
        self._current_messages = []

    @property
    def messages(self):
        return self._current_messages

    @messages.setter
    def messages(self, msgs):
        self._current_messages = msgs

    def step(self, message):
        """step-method of the model"""
        self._current_messages.append(message)

    def get_data(self):
        """get_data-method of model"""
        output = self._current_messages
        self._current_messages = []
        if len(output) > 0:
            return {self._connect_attr: output}
        else:
            return {}


class CommunicationSimulator(mosaik_api.Simulator):
    """Communication-Simulator enables connection towards OMNeT, synchronizes schedulers
     and manages a communication-delayed call of step-methods for
      other simulators """

    def __init__(self):
        super().__init__(META)
        self.sid = 'CommunicationSimululator'
        self.eid = None
        self._use_communication_simulation = True
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
        self._waiting_msgs_counter = 0
        self._is_first_step = True
        self._is_in_waiting_modus = False
        self._current_time = 0
        self._steps = 0
        self._timeout_messages = list()

    def init(self, sid, time_resolution=1., **sim_params):
        self.sid = sid

        for client, attr in sim_params['client_attribute_mapping'].items():
            self._models[client] = CommunicationModel(m_id=client, connect_attr=attr)
            self.meta["models"]["CommunicationModel"]['attrs'].append(attr)

        if 'use_communication_simulation' in sim_params.keys():
            self._use_communication_simulation = sim_params['use_communication_simulation']
            if not self._use_communication_simulation:
                return self.meta

        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self._model_names = list(sim_params['client_attribute_mapping'].keys())

        self._step_size = sim_params['step_size']
        self._omnetpp_connection = OmnetppConnection(sim_params['port'],
                                                     client_socket)
        self._omnetpp_connection.start_connection()

        self.meta["models"]["CommunicationModel"]['attrs'].append(
            f'ctrl_message')
        self._models['ict_controller'] = CommunicationModel(m_id='ict_controller', connect_attr='ctrl_message')

        return self.meta

    def create(self, num, model, **model_params):
        if num > 1:
            raise RuntimeError(
                'Can only create one instance of CommunicationSimulator.')

        self.event_queue = []
        # own id
        self._sid = 'CommunicationSimulator'

        return [{'eid': self._sid, 'type': model}]

    def is_waiting_for_messages(self):
        if self._number_messages_received == \
                self._number_messages_sent:
            self._number_messages_received = 0
            self._number_messages_sent = 0
            self._received_answer = True
            return False
        return True

    def get_next_step(self, old_next_step, new_step):
        if not old_next_step or (old_next_step > new_step):
            return new_step
        return old_next_step

    def receive_messages_from_omnetpp(self, time, max_advance, messages_sent):
        """
        Method to connect to omnetpp to receive messages. If messages
        are received, they are returned. If the CommunicationSimulator
        is still waiting for further messages, this method also returns
        the delay of the received message.
        :param time:
        :param messages_sent:
        :return:
        """
        answers = []
        waiting_for_msg = True
        send_waiting_msg = False
        next_step = None
        initial_waiting_msg_sent = False

        while waiting_for_msg:
            msgs = self._omnetpp_connection.return_messages()
            if len(msgs) == 0:
                if not initial_waiting_msg_sent:
                    if not messages_sent:
                        self.send_waiting_msg(sim_time=time + 1, max_advance=max_advance)
                    initial_waiting_msg_sent = True
                # return answers, time+1
            else:
                for idx, msg in enumerate(msgs):
                    if type(msg) == InfoMessage:
                        log('case received info message')
                        answers.append(msg)
                        next_step = self.get_next_step(next_step, msg.sim_time + 1)
                        if msg.msg_id in self._timeout_messages:
                            self._timeout_messages.remove(msg.msg_id)
                        else:
                            self._number_messages_received += 1
                    elif type(msg) == SynchronisationMessage \
                            and msg.msg_type == SynchronisationMessage.MsgType.TRANSMISSION_ERROR:
                        log('case received transmission error')
                        self._number_messages_sent -= 1
                        send_waiting_msg = True
                        next_step = self.get_next_step(next_step, time + 1)
                        if msg.timeout:
                            log(f'Message is a timeout message. Message might arrive later. ')
                            self._timeout_messages.append(msg.timeout_msg_id)
                    elif type(msg) == InfrastructureMessage:
                        log('case received disconnect or connect notification')
                        answers.append(msg)
                        self._number_messages_sent -= 1
                        send_waiting_msg = True
                    elif type(msg) == SynchronisationMessage \
                            and msg.msg_type == SynchronisationMessage.MsgType.MAX_ADVANCE:
                        if msg.sim_time > time:
                            next_step = self.get_next_step(next_step, msg.sim_time)
                        elif msg.sim_time == time:
                            next_step = self.get_next_step(next_step, time + 1)
                        else:
                            raise ValueError(f'mosaik time: {time}, max advance'
                                             f' time from OMNeT: {msg.sim_time}')
                    elif type(msg) == SynchronisationMessage \
                            and msg.msg_type == SynchronisationMessage.MsgType.WAITING:
                        log('received info about continued waiting in OMNeT++')
                        log(f'number of messages: {len(msgs)}, simtime from OMNeT++: {msg.sim_time},'
                            f'mosaik time: {time}')
                        # TODO: do we step too often now?
                        next_step = self.get_next_step(next_step, time+1)
                if send_waiting_msg:
                    # next_step+1 because OMNeT++ always has to be in advance of mosaik
                    if not messages_sent:
                        if not next_step:
                            self.send_waiting_msg(sim_time=time + 1, max_advance=max_advance)
                        else:
                            self.send_waiting_msg(sim_time=next_step + 1, max_advance=max_advance)
                return answers, next_step

    def send_message_to_omnetpp(self, messages):
        """
        Method to send message to omnetpp. Takes messages, sim_time and
        max_advance and transfers this information to omnetpp.

        :param messages: messages to be send
        :param sim_time: current mosaik sim_time
        :param max_advance: current max advance according to mosaik
        """
        proto_message, message_count = create_protobuf_msg(messages, self._current_time)
        msg = proto_message.SerializeToString()

        # send msg via omnetpp connection
        if self._omnetpp_connection.is_still_connected:
            self._omnetpp_connection.send_message(msg)
        else:
            log('Error with omnet connection')
            self.finalize()
        return message_count

    def send_initial_message(self, max_advance):
        initial_msg = {
            'msg_id': 'InitialMessage',
            'max_advance': max_advance,
            'until': self.mosaik.world.until,
            'stepSize': self._step_size,
        }
        log("send initial message")
        self.send_message_to_omnetpp([(initial_msg, InitialMessage)])

    def send_waiting_msg(self, sim_time, max_advance):
        # send out waiting message
        waiting_msg = {
            'msg_type': SynchronisationMessage.MsgType.WAITING,
            'msg_id': f'WaitingMessage_{self._waiting_msgs_counter}',
            'sim_time': self.calculate_to_ms(sim_time),
            'max_advance': max_advance,
        }
        log(f'Send WaitingMessage_{self._waiting_msgs_counter} at time {sim_time} with max advance {max_advance}')
        self._waiting_msgs_counter += 1
        self.send_message_to_omnetpp([(waiting_msg, SynchronisationMessage)])

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

    @staticmethod
    def check_msgs(messages, time, until):
        """
        Takes the latest time from all the messages and checks whether
        it is further than the mosaik time. Furthermore, checks whether
        the messages have the correct type.
        """
        message_time = None
        for message in messages:
            if type(message) is InfoMessage:
                if not message_time:
                    message_time = message.sim_time
                elif (message_time != message.sim_time) and (message_time < until) and (message.sim_time < until):
                    raise RuntimeError(f'Received messages with different times: {message_time} and {message.sim_time}')
            elif type(message) is not InfrastructureMessage:
                raise ValueError(
                    f'Message from OMNeT has invalid type {type(message)}')
        if message_time and time > message_time:
            raise RuntimeError(
                'Simulation time in OMNeT++ is is behind the simulation '
                'time in mosaik.')

        return messages

    def step(self, time, inputs, max_advance):
        self._steps += 1
        self._current_time = time

        if self._is_first_step and self._use_communication_simulation:
            self.send_initial_message(max_advance)
            self._is_first_step = False
        received_messages = []
        messages_to_send = []

        for eid, attr_names in inputs.items():
            for attribute, sources_to_values in attr_names.items():
                for values in sources_to_values.values():
                    if 'ict_message' in attribute:
                        for value in values:
                            messages_to_send.append((value, InfrastructureMessage))
                    elif type(values) is list:
                        for value in values:
                            if not self._use_communication_simulation:
                                self._models[value['receiver']].step(value)
                            else:
                                value['max_advance'] = max_advance
                                messages_to_send.append((value, InfoMessage))
                    else:
                        raise ValueError(
                            f'Type error of msgs! Received msg from mosaik'
                            f'with type {type(values)}')

        log(f'Communication Simulator steps in {time} with input {messages_to_send}')

        if not self._use_communication_simulation:
            return None

        messages_sent = False
        if len(messages_to_send) > 0:
            self._number_messages_sent += self.send_message_to_omnetpp(messages_to_send)
            messages_sent = True

        if not self.is_waiting_for_messages():
            return None

        answers, next_step = self.receive_messages_from_omnetpp(time, max_advance, messages_sent)
        received_messages.extend(answers)

        smallest_output_time = None

        if received_messages:
            # check whether there is a synchronization error
            received_messages = self.check_msgs(received_messages, time, self.mosaik.world.until)

            for reply in received_messages:
                # TODO: handle invalid messages
                output_time = self.process_msg_from_omnet(reply)
                if smallest_output_time is None or output_time < smallest_output_time:
                    smallest_output_time = output_time

        self.data = {}

        #if next_step and next_step >= self.mosaik.world.until:
        #    log('call finalize.')
        #    self.finalize()
        if smallest_output_time is not None and smallest_output_time == next_step:
            next_step += 1
            log(f'Earliest output time is {smallest_output_time}, therefore return {next_step} at time {time}.')
        return next_step

    def process_msg_from_omnet(self, reply):
        """processes answer messages from omnet, distinguishes between
        scheduler synchronization (FES) and communication delay """
        msg_as_dict = get_dict_from_protobuf_message(reply)
        msg_as_dict['steps'] = self._steps

        if type(reply) is InfrastructureMessage:
            self._models['ict_controller'].step(msg_as_dict)
        else:
            self._models[reply.receiver].step(msg_as_dict)
        return msg_as_dict['sim_time']

    def get_data(self, outputs):
        """gets data of entities for mosaik core"""
        data = {}
        time = None

        for eid, model in self._models.items():
            model_data = model.get_data()
            data.update(model_data)
            for msgs in model_data.values():
                for msg in msgs:
                    if msg['sim_time'] >= self.mosaik.world.until:
                        return {}
                    if time is None:
                        time = msg['sim_time']
                    elif time != msg['sim_time']:
                        log('There are messages with different output '
                            'times in get_data! ')
                        if time > msg['sim_time']:
                            time = msg['sim_time']
        # output time is the lowest value of all times in messages to
        # make sure the agents receive the messages in time
        output_object = {self._sid: data}
        if time is not None and self._current_time <= time:
            output_object['time'] = time
        return output_object

    def finalize(self):
        """last step of CommunicationSimulator"""
        if self._use_communication_simulation:
            self._omnetpp_connection.close_connection()


if __name__ == '__main__':
    mosaik_api.start_simulation(CommunicationSimulator())
