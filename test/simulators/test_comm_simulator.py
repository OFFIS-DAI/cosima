import pytest

from simulators.comm_simulator import CommSimulator
from util import CONFIG_FILE, ROOT_PATH


# test step()


def mock_msg_from_agent_0(cls, message, message_size):
    """This methods mocks method "send_message_to_omnetpp" in CommSimulator.

    In this case, it returns an example message object from agent client0.
    """
    return {'sender': 'client0', 'receiver': 'client1',
            'message': 'Tic Toc on the clock!', 'delay': 0.0041964}


def mock_fes(cls, message, message_size):
    """
    This methods mocks method "send_message_to_omnetpp" in CommSimulator.

    In this case, it returns an example Feature Event Set.
    """
    return {'FES': 0.0041964}


def mock_error(cls, message, message_size):
    """
    This methods mocks method "send_message_to_omnetpp" in CommSimulator.

    In this case, it returns an invalid object.
    """
    return ''


@pytest.fixture
def send_messsage_to_omnetpp_patched(monkeypatch):
    """ Fixture for message send_message_to_omnetpp.

    Uses an example object from one agent. Passes mock: mock_msg_from_agent_0.
    """
    monkeypatch.setattr(CommSimulator, 'send_message_to_omnetpp',
                        mock_msg_from_agent_0)


@pytest.fixture
def send_message_to_omnetpp_fes_patched(monkeypatch):
    """Fixture for message send_message_to_omnetpp.

    Uses an example FES. Passes mock: mock_fes.
    """
    monkeypatch.setattr(CommSimulator, 'send_message_to_omnetpp',
                        mock_fes)


@pytest.fixture
def send_message_to_omnetpp_error_patched(monkeypatch):
    """Fixture for message send_message_to_omnetpp.

    Uses an invalid object. Passes mock: mock_error.
    """
    monkeypatch.setattr(CommSimulator, 'send_message_to_omnetpp',
                        mock_error)


def test_step_comm_simulator_w_o_inputs():
    """Test step method of CommSimulator without any inputs."""
    comm_sim = CommSimulator()
    next_step = comm_sim.step(time=1, inputs={}, max_advance=0)
    # Step method returns next_step for comm_sim.
    # Since comm_sim steps without any inputs, the next step is None.
    assert next_step is None


def test_step_comm_simulator_w_o_inputs_with_delay(
        send_messsage_to_omnetpp_patched):
    """Test step function of CommSimulator.

    Method is called without any inputs. but with a model waiting for a delay
    from omnetpp. In this case, the CommSimulator sends a message to omnetpp
    and receives a delay.
    """
    comm_sim = CommSimulator()
    comm_sim.init(config_file=ROOT_PATH / CONFIG_FILE,
                  sid='CommSim-0')

    # create a model for agent client0
    comm_sim.create(num=1, model='client0')

    # set expect_delay to True for agent client0
    comm_sim._models['client0'].expect_delay = True

    # comm_sim steps without inputs, but because expect_delay from one
    # model is true, it interacts with omnetpp regarding the delay.
    # The next_step is None because there was no FES received
    assert comm_sim.step(time=1, inputs={}, max_advance=0) is None

    # after step is done, the comm model for client0 saved the message
    # from omnetpp
    assert comm_sim._models['client0'].message is not None

    # sender of the message object is agent client0
    assert comm_sim._models['client0'].message['sender'] == 'client0'


def test_step_comm_simulator_inputs_agent_a(
        send_message_to_omnetpp_fes_patched):
    """Test Step method of CommSimulator with input from one agent. """
    comm_sim = CommSimulator()
    comm_sim.init(config_file=ROOT_PATH / CONFIG_FILE,
                  sid='CommSim-0')
    # create model for agent client0
    comm_sim.create(num=1, model='client0')

    # Inputs for step function of CommSimulator. Inputs contain a message
    # object with sender client0.
    inputs = {'client0': {'message': {
        'Agent-0.client0': {'type': 1, 'sender': 'client0',
                            'receiver': 'client1', 'max_advance': 1000000,
                            'message': 'Tic Toc on the clock!'}}}}

    # see mock method, delay is 0.0041964. Therefore next step should be 4.
    assert comm_sim.step(inputs=inputs, time=0, max_advance=0) == 4


def test_step_comm_simulator_inputs_both_agents(
        send_message_to_omnetpp_fes_patched):
    """Test Step method of CommSimulator with input from two agents. """
    comm_sim = CommSimulator()
    comm_sim.init(config_file=ROOT_PATH / CONFIG_FILE,
                  sid='CommSim-0')
    comm_sim.create(num=1, model='client0')
    comm_sim.create(num=1, model='client1')

    # Inputs for step function of CommSimulator. Inputs contain a message
    # object with sender client0.
    inputs_step_1 = {'client0': {'message': {
        'Agent-0.client0': {'type': 1, 'sender': 'client0',
                            'receiver': 'client1', 'max_advance': 1000000,
                            'message': 'Tic Toc on the clock!'}}}}
    # FES received for this message
    assert comm_sim.step(inputs=inputs_step_1, time=0, max_advance=0) == 4

    # Inputs for second step of CommSimulator. Inputs contain a message
    # object with sender client1.
    inputs_step_2 = {'client1': {'message': {
        'Agent-1.client1': {'type': 1, 'sender': 'client1',
                            'receiver': 'client0', 'max_advance': 1000000,
                            'message': 'Tic Toc on the clock!'}}}}

    # FES received for this message. Add 1, because CommSimulator is in step
    # 1 now.
    assert comm_sim.step(inputs=inputs_step_2, time=1, max_advance=0) == 4 + 1


def test_step_comm_simulator_with_exception(
        send_message_to_omnetpp_error_patched):
    """Test error handling of step method. """
    comm_sim = CommSimulator()
    comm_sim.init(config_file=ROOT_PATH / CONFIG_FILE,
                  sid='CommSim-0')
    comm_sim.create(num=1, model='client0')
    comm_sim.create(num=1, model='client1')

    # Inputs for step function of CommSimulator. Inputs contain a message
    # object with sender client0.
    inputs = {'client0': {'message': {
        'Agent-0.client0': {'type': 1, 'sender': 'client0',
                            'receiver': 'client1', 'max_advance': 1000000,
                            'message': 'Tic Toc on the clock!'}}}}
    # Since CommSimulator receives an invalid object from omnetpp
    # (patch: send_message_to_omnetpp_error_patched), it does not
    # process the object any further and returns None as next step.
    assert comm_sim.step(inputs=inputs, time=1, max_advance=0) is None


# test get_data


def test_get_data_comm_sim():
    """ Test get_data from CommSimulator. """
    comm_sim = CommSimulator()
    comm_sim.init(config_file=ROOT_PATH / CONFIG_FILE,
                  sid='CommSim-0')
    comm_sim.create(num=1, model='client0')

    # In get_data, CommSimulator calls get_data() for each of its models.
    # If there is no message set for the models, the output is an empty dict.
    assert comm_sim.get_data(outputs={}) == {}

    # To have an output for the models, model._current_message must be
    # not None.
    comm_sim._models['client0']._current_message = {'message': 'hi'}

    # If the data from the models is not None, the output from the
    # CommSimulator contains 'message_with_delay'.
    output = comm_sim.get_data(outputs={'client0': None})
    assert output != {}
    assert 'message_with_delay' in output['client0'].keys()

    # If the model data is None, the returned output from the CommSimulator
    # is an empty dict.
    comm_sim._models['client0']._current_message = None
    assert comm_sim.get_data(outputs={'client0': None}) == {}
