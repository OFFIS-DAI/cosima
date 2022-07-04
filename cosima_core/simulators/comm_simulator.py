import socket

import mosaik_api

from cosima_core.config import USED_STEP_SIZE, CONNECT_ATTR, TIME_BASED, SIMULATION_END
from cosima_core.messages.message_pb2 import InitialMessage, InfoMessage, SynchronisationMessage, \
    InfrastructureMessage
from cosima_core.simulators.omnetpp_connection import OmnetppConnection
from cosima_core.util.util_functions import log, create_protobuf_msg, get_dict_from_protobuf_message

META = {
    'models': {
        'CommModel': {
            'public': True,
            'any_inputs': False,
            'params': [],
            'attrs': ['message',  # input
                      'next_step',  # input
                      'ict_message'
                      ],
        },
    },
}
if TIME_BASED:
    META['type'] = 'time-based'
else:
    META['type'] = 'event-based'


class CommModel:
    """Model for Comm_Simulator"""

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
        self._waiting_msgs_counter = 0
        self._is_first_step = True
        self._is_in_waiting_modus = False
        self._current_time = 0
        self._steps = 0

    def init(self, sid, time_resolution=1., **sim_params):
        self.sid = sid

        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self._model_names = list(sim_params['client_attribute_mapping'].keys())

        self._step_size = sim_params['step_size']
        self._omnetpp_connection = OmnetppConnection(sim_params['port'],
                                                     client_socket)
        self._omnetpp_connection.start_connection()

        for client, attr in sim_params['client_attribute_mapping'].items():
            self._models[client] = CommModel(m_id=client, connect_attr=attr)
            self.meta["models"]["CommModel"]['attrs'].append(attr)

        self.meta["models"]["CommModel"]['attrs'].append(
            f'ctrl_message')
        self._models['ict_controller'] = CommModel(m_id='ict_controller', connect_attr='ctrl_message')

        return self.meta

    def create(self, num, model, **model_params):
        if num > 1:
            raise RuntimeError(
                'Can only create one instance of CommSimulator.')

        self.event_queue = []
        # own id
        self._sid = 'CommSim'

        return [{'eid': self._sid, 'type': model}]

    def is_waiting_for_messages(self):
        if self._number_messages_received == \
                self._number_messages_sent:
            self._number_messages_received = 0
            self._number_messages_sent = 0
            self._received_answer = True
            return False
        return True

    # TODO: durch neue Überlegungen ersetzen
    def receive_messages_from_omnetpp(self, sim_time, max_advance):
        """
        Method to connect to omnetpp to receive messages. If messages
        are received, they are returned. If the CommunicationSimulator
        is still waiting for further messages, this method also returns
        the delay of the received message.
        :param sim_time:
        :param max_advance:
        :return:
        """
        answers = []
        waiting_for_msg = True
        waiting_msg_sent = False
        next_step = None

        while waiting_for_msg:
            msgs = self._omnetpp_connection.return_messages()
            if len(msgs) == 0:
                if self._received_answer:
                    return answers, next_step
                if max_advance == sim_time and self._is_in_waiting_modus:
                    return answers, sim_time + 1
                else:
                    # wait
                    self._is_in_waiting_modus = True
                    if not waiting_msg_sent:
                        self.send_waiting_msg(sim_time)
                        waiting_msg_sent = True
            else:
                received_info_msg = False
                for idx, msg in enumerate(msgs):
                    if type(msg) == InfoMessage:
                        self._is_in_waiting_modus = False
                        received_info_msg = True
                        self._number_messages_received += 1
                        if not self.is_waiting_for_messages():
                            next_step = msg.sim_time + 1
                        answers.append(msg)
                    elif type(msg) == SynchronisationMessage \
                            and msg.msg_type == SynchronisationMessage.MsgType.TRANSMISSION_ERROR:
                        log('case received transmission error')
                        self._number_messages_sent -= 1
                        if not self.is_waiting_for_messages():
                            self.send_waiting_msg(SIMULATION_END)
                        else:
                            self.send_waiting_msg(sim_time + 1)
                        waiting_msg_sent = True
                    elif type(msg) == InfrastructureMessage:
                        log('case received disconnect or connect notification')
                        answers.append(msg)
                        if idx == len(msgs) - 1:
                            return answers, next_step
                    elif type(msg) == SynchronisationMessage \
                            and msg.msg_type == SynchronisationMessage.MsgType.MAX_ADVANCE:
                        # if max advance message is not the last message,
                        # ignore it
                        # TODO: check if 'and not received_info_msg' is necessary
                        if idx == len(msgs) - 1 and not received_info_msg:
                            log(f'received max advance event at time {sim_time}'
                                f' and max_advance {max_advance}. Event contains'
                                f' time: {msg.sim_time}')
                            if TIME_BASED and msg.sim_time == SIMULATION_END:
                                self.send_waiting_msg(SIMULATION_END)
                            # do we need waiting modus?
                            if sim_time < msg.sim_time:
                                # max advance already reached
                                return answers, msg.sim_time
                            elif sim_time == msg.sim_time:
                                log('sim_time == msg.sim_time')
                                if TIME_BASED:
                                    self.send_waiting_msg(SIMULATION_END)
                                return answers, msg.sim_time + 1
                            else:
                                log(f'mosaik time: {sim_time}, max advance'
                                    f' time from OMNeT: {msg.sim_time}')
                                raise ValueError
                    elif type(msg) == SynchronisationMessage \
                            and msg.msg_type == SynchronisationMessage.MsgType.WAITING:
                        log('received info about continued waiting in OMNeT++')
                        if msg.sim_time > sim_time:
                            log(f'set next step to {msg.sim_time}')
                            next_step = msg.sim_time
                            received_info_msg = True
                if received_info_msg:
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
            'is_time_based': TIME_BASED,
            'stepSize': self._step_size,
        }
        log("send initial message")
        self.send_message_to_omnetpp([(initial_msg, InitialMessage)])

    def send_waiting_msg(self, sim_time):
        # send out waiting message
        waiting_msg = {
            'msg_type': SynchronisationMessage.MsgType.WAITING,
            'msg_id': f'WaitingMessage_{self._waiting_msgs_counter}',
            'sim_time': self.calculate_to_ms(sim_time),
        }
        log(f'WaitingMessage_{self._waiting_msgs_counter}')
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
    def check_msgs(messages, time):
        """
        Takes the latest time from all the messages and checks whether
        it is further than the mosaik time. Furthermore, checks whether
        the messages have the correct type.
        """
        latest_message_time = 0
        send_waiting_msg = False
        for message in messages:
            if (type(message) is InfoMessage) or (type(message) is InfrastructureMessage):
                if message.sim_time > latest_message_time:
                    latest_message_time = message.sim_time
                if TIME_BASED and message.sim_time < time:
                    log(f'set message sim time to {time} instead of {message.sim_time}')
                    message.sim_time = time
            else:
                raise ValueError(
                    f'Message from OMNeT has invalid type {type(message)}')
        if time > latest_message_time:
            if not TIME_BASED:
                raise RuntimeError(
                    'Simulation time in OMNeT++ is is behind the simulation '
                    'time in mosaik.')
            else:
                send_waiting_msg = True
                print(
                    'Simulation time in OMNeT++ is is behind the simulation '
                    'time in mosaik.')
        return messages, send_waiting_msg

    def step(self, time, inputs, max_advance):
        self._steps += 1
        self._current_time = time
        if self._is_first_step:
            self.send_initial_message(max_advance)
            self._is_first_step = False
        next_step = None
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
                            value['max_advance'] = max_advance
                            messages_to_send.append((value, InfoMessage))
                    elif TIME_BASED and values is None:
                        pass
                    else:
                        raise ValueError(
                            f'Type error of msgs! Received msg from mosaik'
                            f'with type {type(values)}')

        log(f'Comm Sim steps in {time} with input {messages_to_send}')

        if len(messages_to_send) > 0:
            self._number_messages_sent += self.send_message_to_omnetpp(messages_to_send)
            self._received_answer = False
            if not TIME_BASED:
                answers, next_step = self.receive_messages_from_omnetpp(
                    time, max_advance)
                received_messages.extend(answers)
        elif not self._received_answer:
            # TODO: überarbeiten
            if not TIME_BASED:
                self.send_waiting_msg(time)
            answers, next_step = self.receive_messages_from_omnetpp(
                time, max_advance)
            received_messages.extend(answers)

        if received_messages:
            # check whether there is a synchronization error
            # TODO: return value von check_msgs anpassen
            received_messages, send_waiting_msg = self.check_msgs(received_messages, time)

            if send_waiting_msg:
                # TODO: nicht mehr senden
                self.send_waiting_msg(SIMULATION_END)

            for reply in received_messages:
                # TODO: handle invalid messages
                self.process_msg_from_omnet(reply)

        self.data = {}

        if next_step and next_step >= self.mosaik.world.until:
            log('call finalize.')
            self.finalize()
        if TIME_BASED:
            return time + 1
        else:
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

    def get_data(self, outputs):
        """gets data of entities for mosaik core"""
        data = {}
        time = None

        for eid, model in self._models.items():
            model_data = model.get_data()
            data.update(model_data)
            for msgs in model_data.values():
                for msg in msgs:
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
        """last step of comm-simulator"""
        self._omnetpp_connection.close_connection()


if __name__ == '__main__':
    mosaik_api.start_simulation(CommSimulator())
