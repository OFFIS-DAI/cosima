from unittest.mock import MagicMock
import mosaik

import pytest

from message_pb2 import CosimaMsgGroup
from simulators.comm_simulator import CommSimulator, CommModel
from config import USED_STEP_SIZE
from simulators.omnetpp_connection import OmnetppConnection
from util_functions import get_agent_names


def set_up_comm_sim():
    comm_sim = CommSimulator()
    mock_init_and_create_client0_and_client1_model(comm_sim)
    mock_world(comm_sim)
    comm_sim.send_message_to_omnetpp = MagicMock()
    comm_sim._is_first_step = False
    return comm_sim


def mock_msg_from_agent_0(message, message_size):
    """This methods mocks method "receive_message_from_omnetpp"
    in CommSimulator.

    In this case, it returns an example message object from agent client0.
    """
    answers = list()
    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.INFO
    msg.msgId = 'Test_Message_0'
    msg.simTime = 10
    msg.creation_time = 5
    msg.sender = 'client0'
    msg.receiver = 'client1'
    msg.max_advance = 10
    msg.size = 150
    msg.content = 'Hi from client0'
    msg.stepSize = USED_STEP_SIZE
    msg.until = 1000
    msg.change_module = ""

    answers.append(msg)

    return answers, msg.simTime


def mock_error(message, message_size):
    """
    This methods mocks method "receive_messages_from_omnetpp" in CommSimulator.

    In this case, it returns an invalid object.
    """
    return "error", 10


def mock_send_waiting_msg(max_advance, sim_time):
    """
    Mock for method send_waiting_msg of CommSim. In this case, do nothing.
    """
    return None


def mock_info_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns an info message.
    """
    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.INFO
    msg.simTime = 10
    return [msg]


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
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.TRANSMISSION_ERROR
    msg_list.append(msg)

    # also add an info msg, to make sure the comm sim will not wait
    # forever for new msgs
    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.INFO
    msg.simTime = 10
    msg_list.append(msg)

    return msg_list


def mock_disconnect_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns a disconnect message.
    """
    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.DISCONNECT

    return [msg]


def mock_max_advance_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns a max_advance message.
    """
    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.MAX_ADVANCE

    return [msg]


def mock_max_advance_and_info_msg():
    """
    This methods mocks method "return_messages" in OmnetppConnection.

    In this case, it returns a list with a max_advance message and an info
    message.
    """
    msg_list = []

    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.MAX_ADVANCE
    msg_list.append(msg)

    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.INFO
    msg.simTime = 10
    msg_list.append(msg)

    return msg_list


def mock_world(simulator):
    simulator.mosaik = MagicMock(dict())
    simulator.mosaik.world = MagicMock(mosaik.scenario.World)
    simulator.mosaik.world.until = 1000


def mock_init_and_create_client0_and_client1_model(comm_sim):
    """
    This methods initializes a CommSim object without an OmnetppConnection
    and creates a model for client0.
    """

    comm_sim._model_names = get_agent_names(2)
    comm_sim._step_size = USED_STEP_SIZE

    comm_sim._models = {}
    for model in ['client0', 'client1']:
        # create a model for agent client0
        comm_sim.create(num=1, model=model)
        comm_sim.meta["models"][model] = dict()
        comm_sim.meta["models"][model]['attrs'] = list()
        comm_sim.meta["models"][model]['attrs'].append(
            f'message_with_delay_for_{model}')
        comm_sim._models[model] = CommModel(m_id=model)


def test_step_comm_simulator_w_o_inputs():
    """Test step method of CommSimulator without any inputs.
    Is not the first step -> don't send initial message."""
    comm_sim = set_up_comm_sim()
    next_step = comm_sim.step(time=1, inputs={}, max_advance=0)
    # Step method returns next_step for comm_sim.
    # Since comm_sim steps without any inputs, the next step is None.
    assert next_step is None


def test_step_comm_simulator_w_o_inputs_with_delay(
        monkeypatch):
    """Test step function of CommSimulator.

    Method is called without any inputs. but with a model waiting for a delay
    from omnetpp. In this case, the CommSimulator sends a message to omnetpp
    and receives a delay.
    """
    comm_sim = set_up_comm_sim()

    monkeypatch.setattr(comm_sim, 'receive_messages_from_omnetpp',
                        mock_msg_from_agent_0)

    # comm sim has not received all answers
    comm_sim._received_answer = False

    # comm_sim steps without inputs, but because expect_delay from one
    # model is true, it interacts with omnetpp regarding the delay.
    # The next_step is 10, because of the delay of the received message
    assert comm_sim.step(time=1, inputs={}, max_advance=0) == 10

    # after step is done, the comm model for client0 saved the message
    # from omnetpp
    assert comm_sim._models['client0'].messages is not {}

    # sender of the message object is agent client0
    assert comm_sim._models['client1'].messages[0]['sender'] == 'client0'


def test_step_comm_simulator_inputs_agent_a(
        monkeypatch):
    """Test Step method of CommSimulator with input from one agent. """
    comm_sim = set_up_comm_sim()

    monkeypatch.setattr(comm_sim, 'receive_messages_from_omnetpp',
                        mock_msg_from_agent_0)

    # Inputs for step function of CommSimulator. Inputs contain a message
    # object with sender client0.
    inputs = {'client0': {'message': {
        'Agent-0.client0': [{'type': 1, 'sender': 'client0',
                             'receiver': 'client1', 'max_advance': 1000000,
                             'message': 'Tic Toc on the clock!'}]}}}

    # see mock method, delay is 9 ms at time 1.
    # Therefore next step should be 10.
    assert comm_sim.step(inputs=inputs, time=0, max_advance=0) == 10


def test_step_comm_simulator_inputs_both_agents(
        monkeypatch):
    """Test Step method of CommSimulator with input from two agents. """
    comm_sim = set_up_comm_sim()

    monkeypatch.setattr(comm_sim, 'receive_messages_from_omnetpp',
                        mock_msg_from_agent_0)

    # Inputs for step function of CommSimulator. Inputs contain a message
    # object with sender client0.
    inputs_step_1 = {'client0': {'message': {
        'Agent-0.client0': [{'type': 1, 'sender': 'client0',
                             'receiver': 'client1', 'max_advance': 1000000,
                             'message': 'Tic Toc on the clock!'}]}}}
    # answer received for this message
    assert comm_sim.step(inputs=inputs_step_1, time=0, max_advance=0) == 10

    # Inputs for second step of CommSimulator. Inputs contain a message
    # object with sender client1.
    inputs_step_2 = {'client1': {'message': {
        'Agent-1.client1': [{'type': 1, 'sender': 'client1',
                             'receiver': 'client0', 'max_advance': 1000000,
                             'message': 'Tic Toc on the clock!'}]}}}

    # answer received for this message.
    assert comm_sim.step(inputs=inputs_step_2, time=1, max_advance=0) == 10


def test_step_comm_simulator_with_exception(
        monkeypatch):
    """Test error handling of step method. """
    comm_sim = set_up_comm_sim()

    monkeypatch.setattr(comm_sim, 'receive_messages_from_omnetpp',
                        mock_error)

    # Inputs for step function of CommSimulator. Inputs contain a message
    # object with sender client0.
    inputs = {'client0': {'message': {
        'Agent-0.client0': [{'type': 1, 'sender': 'client0',
                             'receiver': 'client1', 'max_advance': 1000000,
                             'message': 'Tic Toc on the clock!'}]}}}
    # Since CommSimulator receives an invalid object from omnetpp
    # (patch: receive_messages_from_omnetpp_error_patched), it does not
    # process the object any further and throws ValueError.
    with pytest.raises(ValueError):
        comm_sim.step(inputs=inputs, time=1, max_advance=0)


def test_get_data_comm_sim():
    """ Test get_data from CommSimulator. """
    comm_sim = set_up_comm_sim()

    # In get_data, CommSimulator calls get_data() for each of its models.
    # If there is no message set for the models, the output is an empty dict.
    assert comm_sim.get_data(outputs={})['CommSim'] == {}

    # To have an output for the models, model._current_message must be
    # not None.
    comm_sim._models['client0']._current_messages = [
        {'message': 'hi', 'output_time': 10}]

    # If the data from the models is not None, the output from the
    # CommSimulator contains 'message_with_delay_for_clientß'.
    output = comm_sim.get_data(outputs={'client0': None})
    assert output != {}
    assert 'message_with_delay_for_client0' in output['CommSim'].keys()

    # If the model data is None, the returned output from the CommSimulator
    # is an empty dict.
    comm_sim._models['client0']._current_message = None
    assert comm_sim.get_data(outputs={'client0': None})['CommSim'] == {}


def test_calculate_correct_msg_size():
    """
    Test method calculate_correct_msg_size of CommSim.
    """
    comm_sim = set_up_comm_sim()

    # create DisconnectMsg, in this case the method should return 0
    msg = {'type': CosimaMsgGroup.CosimaMsg.MsgType.DISCONNECT}
    assert comm_sim.calculate_correct_msg_size(msg) == 0

    # create correct Message to calculate size of
    msg = {'sender': 'client0', 'receiver': 'client1',
           'message': 'Hello Agent!',
           'type': CosimaMsgGroup.CosimaMsg.MsgType.INFO}
    assert comm_sim.calculate_correct_msg_size(msg) != 0


def test_remove_old_steps():
    """
    Test method remove_old_steps in CommSim, where old agents steps are
    removed if they are not up to date according to the given current time.
    """
    comm_sim = set_up_comm_sim()

    # Set agent steps to time 0 and then call method with time 1.
    # Therefore, the agent steps are removed
    comm_sim._next_agent_steps = [0]
    comm_sim.remove_old_steps(sim_time=1)

    assert len(comm_sim._next_agent_steps) == 0

    # Set agent steps to time 1 and then call method with time 1.
    # Therefore, no agent steps are removed
    comm_sim._next_agent_steps = [1]
    comm_sim.remove_old_steps(sim_time=1)

    assert len(comm_sim._next_agent_steps) == 1


def test_receive_messages_from_omnetpp_info_msg(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommSim. In this case,
    test several situations when an info message was received.
    """
    comm_sim = set_up_comm_sim()
    comm_sim._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(comm_sim._omnetpp_connection, 'return_messages',
                        mock_info_msg)
    monkeypatch.setattr(comm_sim, 'send_waiting_msg',
                        mock_send_waiting_msg)

    # in this case, number_of_messages_sent should equal
    # number_of_messages_received, since one message was received
    comm_sim._number_messages_sent = 1
    answers, next_step = comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)
    assert comm_sim._number_messages_sent == comm_sim._number_messages_received
    assert next_step is not None

    # in this case, number_of_messages_sent should not equal
    # number_of_messages_received, since one message was received and further
    # none was sent.
    comm_sim._number_messages_sent = 0
    answers, next_step = comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)
    assert comm_sim._number_messages_sent != comm_sim._number_messages_received
    assert next_step is None


def test_receive_messages_from_omnetpp_no_msgs(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommSim. In this case,
    test several situations when no messages were received.
    """
    comm_sim = set_up_comm_sim()
    comm_sim._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(comm_sim._omnetpp_connection, 'return_messages',
                        mock_no_msg)
    monkeypatch.setattr(comm_sim, 'send_waiting_msg',
                        mock_send_waiting_msg)

    comm_sim._received_answer = True
    # in this case, the comm_sim should return an empty list, because it
    # directly returns the method because received_answer is True
    answers, next_step = comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)
    assert len(answers) == 0
    assert next_step is None

    comm_sim._received_answer = False
    comm_sim._next_agent_steps = [0]
    # in this case, the comm_sim should return an empty list, because it
    # directly returns the method because the next agent step equals
    # max_advance
    answers, next_step = comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)
    assert len(answers) == 0
    assert next_step is None

    comm_sim._received_answer = False
    comm_sim._next_agent_steps = [10]
    # in this case, the comm_sim should stay in waiting_modus.
    comm_sim.receive_messages_from_omnetpp(sim_time=0, max_advance=0)
    assert comm_sim._is_in_waiting_modus


def test_receive_messages_from_omnetpp_transmission_error(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommSim. In this case,
    test several situations when a transmission error was received.
    """
    comm_sim = set_up_comm_sim()
    comm_sim._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(comm_sim._omnetpp_connection, 'return_messages',
                        mock_transmission_error)
    monkeypatch.setattr(comm_sim, 'send_waiting_msg',
                        mock_send_waiting_msg)

    comm_sim._number_messages_sent = 1
    # in this case, the number_of_messages_sent should be decreased,
    # because the transmission error states a missing message
    comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)
    assert comm_sim._number_messages_sent == 0


def test_receive_messages_from_omnetpp_disconnect(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommSim. In this case,
    test several situations when a disconnect message was received.
    """
    comm_sim = set_up_comm_sim()
    comm_sim._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(comm_sim._omnetpp_connection, 'return_messages',
                        mock_disconnect_msg)
    monkeypatch.setattr(comm_sim, 'send_waiting_msg',
                        mock_send_waiting_msg)

    # in this case, the comm sim only receives a disconnect msg and therefore
    # directly returns
    answers, next_step = comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)
    assert len(answers) == 1
    assert next_step is None


def test_receive_messages_from_omnetpp_max_advance(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommSim. In this case,
    test several situations when a max advance message was received.
    """
    comm_sim = set_up_comm_sim()
    comm_sim._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(comm_sim._omnetpp_connection, 'return_messages',
                        mock_max_advance_msg)
    monkeypatch.setattr(comm_sim, 'send_waiting_msg',
                        mock_send_waiting_msg)

    answers, next_step = comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)
    # the sim time is smaller then the messages sim time, therefore, the
    # comm sim directly returns. Next step will be the time in the message
    assert len(answers) == 0
    assert next_step is not None

    # set the sim time to a very large number, way further then message time,
    # therefore, an error is raised
    with pytest.raises(ValueError):
        comm_sim.receive_messages_from_omnetpp(sim_time=200, max_advance=0)


def test_receive_messages_from_omnetpp_max_advance_and_info(monkeypatch):
    """
    Test method receive_messages_from_omnetpp from CommSim. In this case,
    test several situations when max advance and an info messages were
    received.
    """
    comm_sim = set_up_comm_sim()
    comm_sim._omnetpp_connection = OmnetppConnection(None, None)

    monkeypatch.setattr(comm_sim._omnetpp_connection, 'return_messages',
                        mock_max_advance_and_info_msg)
    monkeypatch.setattr(comm_sim, 'send_waiting_msg',
                        mock_send_waiting_msg)

    comm_sim._number_messages_received = 0
    comm_sim.receive_messages_from_omnetpp(sim_time=0,
                                                                max_advance=0)

    # in this case, the info msgs is relevant and number_of_messages_received
    # should be increased, because one message was received
    assert comm_sim._number_messages_received == 1


def test_process_msg_from_omnet():
    comm_sim = set_up_comm_sim()

    comm_sim._models['ict_controller'] = CommModel(m_id='ict_controller')

    reply = {}
    # the method expects a protobuf-object
    with pytest.raises(ValueError):
        comm_sim.process_msg_from_omnet(reply=reply, time=0)

    reply = []
    # the method expects a protobuf-object
    with pytest.raises(ValueError):
        comm_sim.process_msg_from_omnet(reply=reply, time=0)

    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.INFO
    msg.msgId = 'Test_Message_0'
    msg.simTime = 10

    # the given time is larger than the msg.simTime
    with pytest.raises(RuntimeError):
        comm_sim.process_msg_from_omnet(reply=msg, time=20)

    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.RECONNECT
    msg.msgId = 'Test_Message_0'
    msg.simTime = 10

    # with reconnect, the ict_controller_model is stepped
    comm_sim.process_msg_from_omnet(reply=msg, time=2)
    assert len(comm_sim._models['ict_controller'].messages) != 0

    msg_group = CosimaMsgGroup()
    msg = msg_group.msg.add()
    msg.type = CosimaMsgGroup.CosimaMsg.MsgType.INFO
    msg.msgId = 'Test_Message_0'
    msg.simTime = 10
    msg.receiver = 'client0'

    # the model of the receving agent is stepped
    comm_sim.process_msg_from_omnet(reply=msg, time=2)
    assert len(comm_sim._models['client0'].messages) != 0
