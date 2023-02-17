from unittest.mock import MagicMock
import mosaik

import pytest

from cosima_core.messages.message_pb2 import CosimaMsgGroup, SynchronisationMessage, InfrastructureMessage
from cosima_core.simulators.communication_simulator import CommunicationSimulator, CommunicationModel
from cosima_core.simulators.omnetpp_connection import OmnetppConnection
from cosima_core.util.util_functions import get_host_names


def set_up_communication_simulator():
    communication_simulator = CommunicationSimulator()
    mock_init_and_create_client0_and_client1_model(communication_simulator)
    mock_world(communication_simulator)
    communication_simulator.send_message_to_omnetpp = MagicMock()
    communication_simulator._is_first_step = False
    return communication_simulator


def mock_no_msgs(time, max_advance, messages_sent):
    return [], None


def mock_msg_from_agent_0(time, max_advance, messages_sent):
    """This methods mocks method "receive_message_from_omnetpp"
    in CommunicationSimulator.

    In this case, it returns an example message object from agent client0.
    """
    answers = list()
    msg_group = CosimaMsgGroup()
    msg = msg_group.info_messages.add()
    msg.msg_id = 'Test_Message_0'
    msg.sim_time = 10
    msg.creation_time = 5
    msg.sender = 'client0'
    msg.receiver = 'client1'
    msg.max_advance = 10
    msg.size = 150
    msg.content = 'Hi from client0'

    answers.append(msg)

    return answers, msg.sim_time


def mock_error(time, max_advance, messages_sent):
    """
    This methods mocks method "receive_messages_from_omnetpp" in CommunicationSimulator.

    In this case, it returns an invalid object.
    """
    return "error", 10


def mock_send_waiting_msg(sim_time, max_advance):
    """
    Mock for method send_waiting_msg of CommunicationSimulator. In this case, do nothing.
    """
    return None


def mock_info_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns an info message.
    """
    msg_group = CosimaMsgGroup()
    msg = msg_group.info_messages.add()
    msg.sim_time = 10
    return [msg]


def mock_is_not_waiting_for_messages():
    """
    This method mocks method "is_waiting_for_messages" in CommunicationSimulator.

    In this case, it returns False.
    """
    return False


def mock_is_waiting_for_messages():
    """
    This method mocks method "is_waiting_for_messages" in CommunicationSimulator.

    In this case, it returns True.
    """
    return True


def mock_no_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns an empty list for the messages.
    """
    return []


def mock_transmission_error():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns a message with type transmission error.
    """
    msg_list = []
    msg_group = CosimaMsgGroup()
    msg = msg_group.synchronisation_messages.add()
    msg.msg_type = SynchronisationMessage.MsgType.TRANSMISSION_ERROR
    msg_list.append(msg)

    # also add an info msg, to make sure the communication simulator will not wait
    # forever for new msgs
    msg_group = CosimaMsgGroup()
    msg = msg_group.info_messages.add()
    msg.sim_time = 10
    msg_list.append(msg)

    return msg_list


def mock_disconnect_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns a disconnect message.
    """
    msg_group = CosimaMsgGroup()
    msg = msg_group.infrastructure_messages.add()
    msg.msg_type = InfrastructureMessage.MsgType.DISCONNECT

    return [msg]


def mock_max_advance_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns a max_advance message.
    """
    msg_group = CosimaMsgGroup()
    msg = msg_group.synchronisation_messages.add()
    msg.msg_type = SynchronisationMessage.MsgType.MAX_ADVANCE

    return [msg]


def mock_max_advance_and_info_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns a list with a max_advance message and an info
    message.
    """
    msg_list = []

    msg_group = CosimaMsgGroup()
    msg = msg_group.synchronisation_messages.add()
    msg.msg_type = SynchronisationMessage.MsgType.MAX_ADVANCE
    msg_list.append(msg)

    msg_group = CosimaMsgGroup()
    msg = msg_group.info_messages.add()
    msg.sim_time = 10
    msg_list.append(msg)

    return msg_list


def mock_world(simulator):
    simulator.mosaik = MagicMock(dict())
    simulator.mosaik.world = MagicMock(mosaik.scenario.World)
    simulator.mosaik.world.until = 1000


def mock_init_and_create_client0_and_client1_model(communication_simulator):
    """
    This methods initializes a CommunicationSimulator object without an OmnetppConnection
    and creates a model for client0.
    """
    agent_names = get_host_names(num_hosts=2)
    communication_simulator._model_names = agent_names

    client_attribute_mapping = {}
    # for each client in OMNeT++, save the connect-attribute for mosaik
    for agent_name in agent_names:
        client_attribute_mapping[agent_name] = 'message_with_delay_for_' + agent_name

    for client, attr in client_attribute_mapping.items():
        communication_simulator._models[client] = CommunicationModel(m_id=client, connect_attr=attr)


def test_step_communication_simulator_w_o_inputs(monkeypatch):
    """Test step method of CommunicationSimulator without any inputs.
    Is not the first step -> don't send initial message."""
    communication_simulator = set_up_communication_simulator()
    monkeypatch.setattr(communication_simulator, 'receive_messages_from_omnetpp',
                        mock_no_msgs)
    next_step = communication_simulator.step(time=1, inputs={}, max_advance=0)
    # Step method returns next_step for communication_simulator.
    # Since communication_simulator steps without any inputs, the next step is None.
    assert next_step is None


def test_step_communication_simulator_w_o_inputs_with_delay(
        monkeypatch):
    """Test step function of CommunicationSimulator.

    Method is called without any inputs. but with a model waiting for a delay
    from omnetpp. In this case, the CommunicationSimulator sends a message to omnetpp
    and receives a delay.
    """
    communication_simulator = set_up_communication_simulator()

    monkeypatch.setattr(communication_simulator, 'is_waiting_for_messages',
                        mock_is_waiting_for_messages)

    monkeypatch.setattr(communication_simulator, 'receive_messages_from_omnetpp',
                        mock_msg_from_agent_0)

    # communication_simulator steps without inputs, but because expect_delay from one
    # model is true, it interacts with omnetpp regarding the delay.
    # current time equals output time of data dict, therefore add 1 to next_step
    # The next_step is 11, because of the delay of the received message
    assert communication_simulator.step(time=1, inputs={}, max_advance=0) == 11

    # after step is done, the communication model for client0 saved the message
    # from omnetpp
    assert communication_simulator._models['client0'].messages is not {}

    # sender of the message object is agent client0
    assert communication_simulator._models['client1'].messages[0]['sender'] == 'client0'


def test_step_communication_simulator_inputs_agent_a(
        monkeypatch):
    """Test Step method of CommunicationSimulator with input from one agent. """
    communication_simulator = set_up_communication_simulator()

    monkeypatch.setattr(communication_simulator, 'receive_messages_from_omnetpp',
                        mock_msg_from_agent_0)
    communication_simulator._received_answer = False

    # Inputs for step function of CommunicationSimulator. Inputs contain a message
    # object with sender client0.
    inputs = {'client0': {'message': {
        'Agent-0.client0': [{'sender': 'client0',
                             'receiver': 'client1', 'max_advance': 1000000,
                             'content': 'Tic Toc on the clock!'}]}}}

    # see mock method, delay is 9 ms at time 1.
    # current time equals output time of data dict, therefore add 1 to next_step
    # Therefore next step should be 11.
    assert communication_simulator.step(inputs=inputs, time=0, max_advance=1) == 11


def test_step_communication_simulator_inputs_both_agents(
        monkeypatch):
    """Test Step method of CommunicationSimulator with input from two agents. """
    communication_simulator = set_up_communication_simulator()

    monkeypatch.setattr(communication_simulator, 'receive_messages_from_omnetpp',
                        mock_msg_from_agent_0)
    communication_simulator._received_answer = False

    # Inputs for step function of CommunicationSimulator. Inputs contain a message
    # object with sender client0.
    inputs_step_1 = {'client0': {'message': {
        'Agent-0.client0': [{'sender': 'client0',
                             'receiver': 'client1', 'max_advance': 1000000,
                             'content': 'Tic Toc on the clock!'}]}}}
    # answer received for this message
    # current time equals output time of data dict, therefore add 1 to next_step
    assert communication_simulator.step(inputs=inputs_step_1, time=0, max_advance=1) == 11

    # Inputs for second step of CommunicationSimulator. Inputs contain a message
    # object with sender client1.
    inputs_step_2 = {'client1': {'message': {
        'Agent-1.client1': [{'sender': 'client1',
                             'receiver': 'client0', 'max_advance': 1000000,
                             'content': 'Tic Toc on the clock!'}]}}}

    # answer received for this message.
    # current time equals output time of data dict, therefore add 1 to next_step
    assert communication_simulator.step(inputs=inputs_step_2, time=1, max_advance=0) == 11


def test_step_communication_simulator_with_exception(
        monkeypatch):
    """Test error handling of step method. """
    communication_simulator = set_up_communication_simulator()

    monkeypatch.setattr(communication_simulator, 'receive_messages_from_omnetpp',
                        mock_error)
    communication_simulator._received_answer = False

    # Inputs for step function of CommunicationSimulator. Inputs contain a message
    # object with sender client0.
    inputs = {'client0': {'message': {
        'Agent-0.client0': [{'sender': 'client0',
                             'receiver': 'client1', 'max_advance': 1000000,
                             'content': 'Tic Toc on the clock!'}]}}}
    # Since CommunicationSimulator receives an invalid object from omnetpp
    # (patch: receive_messages_from_omnetpp_error_patched), it does not
    # process the object any further and throws ValueError.
    with pytest.raises(ValueError):
        communication_simulator.step(inputs=inputs, time=1, max_advance=0)


def test_get_data_communication_sim():
    """ Test get_data from CommunicationSimulator. """
    communication_simulator = set_up_communication_simulator()

    # In get_data, CommunicationSimulator calls get_data() for each of its models.
    # If there is no message set for the models, the output is an empty dict.
    assert not list(communication_simulator.get_data(outputs={}).values())[0]

    # To have an output for the models, model._current_message must be
    # not None.
    communication_simulator._models['client0']._current_messages = [
        {'content': 'hi', 'sim_time': 10}]

    # If the data from the models is not None, the output from the
    # CommunicationSimulator contains 'message_with_delay_for_clientß'.
    output = communication_simulator.get_data(outputs={'client0': None})
    assert output != {}
    assert 'message_with_delay_for_client0' in list(output.values())[0]

    # If the model data is None, the returned output from the CommunicationSimulator
    # is an empty dict.
    communication_simulator._models['client0']._current_message = None
    assert not list(communication_simulator.get_data(outputs={'client0': None}).values())[0]


def test_receive_messages_from_omnetpp_info_msg(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommunicationSimulator. In this case,
    test several situations when an info message was received.
    """
    communication_simulator = set_up_communication_simulator()
    communication_simulator._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(communication_simulator._omnetpp_connection, 'return_messages',
                        mock_info_msg)
    monkeypatch.setattr(communication_simulator, 'send_waiting_msg',
                        mock_send_waiting_msg)

    # in this case, number_of_messages_sent should equal
    # number_of_messages_received, since one message was received
    communication_simulator._number_messages_sent = 1
    answers, next_step = communication_simulator.receive_messages_from_omnetpp(time=0, max_advance=5,
                                                                               messages_sent=False)
    assert communication_simulator._number_messages_sent == communication_simulator._number_messages_received
    assert next_step is not None

    # in this case, number_of_messages_sent should not equal
    # number_of_messages_received, since one message was received and further
    # none was sent.
    communication_simulator._number_messages_sent = 0
    answers, next_step = communication_simulator.receive_messages_from_omnetpp(time=0, max_advance=5,
                                                                               messages_sent=True)
    assert communication_simulator._number_messages_sent != communication_simulator._number_messages_received


def test_receive_messages_from_omnetpp_transmission_error(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommunicationSimulator. In this case,
    test several situations when a transmission error was received.
    """
    communication_simulator = set_up_communication_simulator()
    communication_simulator._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(communication_simulator._omnetpp_connection, 'return_messages',
                        mock_transmission_error)
    monkeypatch.setattr(communication_simulator, 'send_waiting_msg',
                        mock_send_waiting_msg)

    communication_simulator._number_messages_sent = 1
    # in this case, the number_of_messages_sent should be decreased,
    # because the transmission error states a missing message
    communication_simulator.receive_messages_from_omnetpp(time=0, max_advance=5, messages_sent=False)
    assert communication_simulator._number_messages_sent == 0


def test_receive_messages_from_omnetpp_disconnect(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommunicationSimulator. In this case,
    test several situations when a disconnect message was received.
    """
    communication_simulator = set_up_communication_simulator()
    communication_simulator._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(communication_simulator._omnetpp_connection, 'return_messages',
                        mock_disconnect_msg)
    monkeypatch.setattr(communication_simulator, 'send_waiting_msg',
                        mock_send_waiting_msg)

    # in this case, the communication simulator only receives a disconnect msg and therefore
    # directly returns
    answers, next_step = communication_simulator.receive_messages_from_omnetpp(time=0, max_advance=5,
                                                                               messages_sent=False)
    assert len(answers) == 1
    assert next_step is None


def test_receive_messages_from_omnetpp_max_advance(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommunicationSimulator. In this case,
    test several situations when a max advance message was received.
    """
    communication_simulator = set_up_communication_simulator()
    communication_simulator._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(communication_simulator._omnetpp_connection, 'return_messages',
                        mock_max_advance_msg)
    monkeypatch.setattr(communication_simulator, 'send_waiting_msg',
                        mock_send_waiting_msg)

    answers, next_step = communication_simulator.receive_messages_from_omnetpp(time=0, max_advance=5,
                                                                               messages_sent=False)
    # the sim time is smaller than the messages sim time, therefore, the
    # communication simulator directly returns. Next step will be the time in the message
    assert len(answers) == 0
    assert next_step is not None

    # set the sim time to a very large number, way further then message time,
    # therefore, an error is raised
    with pytest.raises(ValueError):
        communication_simulator.receive_messages_from_omnetpp(time=200, max_advance=205, messages_sent=False)


def test_receive_messages_from_omnetpp_max_advance_and_info(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommunicationSimulator. In this case,
    test several situations when max advance and an info messages were
    received.
    """
    communication_simulator = set_up_communication_simulator()
    communication_simulator._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(communication_simulator._omnetpp_connection, 'return_messages',
                        mock_max_advance_and_info_msg)
    monkeypatch.setattr(communication_simulator, 'send_waiting_msg',
                        mock_send_waiting_msg)

    communication_simulator._number_messages_received = 0
    communication_simulator.receive_messages_from_omnetpp(time=0, max_advance=5, messages_sent=False)

    # in this case, the info msgs is relevant and number_of_messages_received
    # should be increased, because one message was received
    assert communication_simulator._number_messages_received == 1


def test_process_msg_from_omnet():
    communication_simulator = set_up_communication_simulator()

    communication_simulator._models['ict_controller'] = CommunicationModel(m_id='ict_controller',
                                                                           connect_attr='ctrl_message')

    msg_group = CosimaMsgGroup()
    msg = msg_group.infrastructure_messages.add()
    msg.msg_type = InfrastructureMessage.MsgType.RECONNECT
    msg.msg_id = 'Test_Message_0'
    msg.sim_time = 10

    # with reconnect, the ict_controller_model is stepped
    communication_simulator.process_msg_from_omnet(reply=msg)
    assert len(communication_simulator._models['ict_controller'].messages) != 0

    msg_group = CosimaMsgGroup()
    msg = msg_group.info_messages.add()
    msg.msg_id = 'Test_Message_0'
    msg.sim_time = 10
    msg.receiver = 'client0'

    # the model of the receiving agent is stepped
    communication_simulator.process_msg_from_omnet(reply=msg)
    assert len(communication_simulator._models['client0'].messages) != 0


def test_check_msgs():
    communication_simulator = set_up_communication_simulator()

    msg_group = CosimaMsgGroup()
    msg = msg_group.info_messages.add()
    msg.msg_id = 'Test_Message_0'
    msg.sim_time = 1

    msg_1 = msg_group.info_messages.add()
    msg_1.msg_id = 'Test_Message_1'
    msg_1.sim_time = 1

    msg_2 = msg_group.info_messages.add()
    msg_2.msg_id = 'Test_Message_2'
    msg_2.sim_time = 2

    # sim_time is further than last message time
    msgs = [msg, msg_1]
    with pytest.raises(RuntimeError):
        communication_simulator.check_msgs(msgs, time=3, until=100)

    # sim time is not further than last message time, therefore no
    # error is raised
    communication_simulator.check_msgs([msg, msg_1], time=0, until=100)

    # received messages with different output time
    msgs = [msg, msg_2]
    with pytest.raises(RuntimeError):
        communication_simulator.check_msgs(msgs, time=3, until=100)

    # msg_type is incorrect
    msgs = [{}, {}]
    with pytest.raises(ValueError):
        communication_simulator.check_msgs(msgs, time=1, until=100)
