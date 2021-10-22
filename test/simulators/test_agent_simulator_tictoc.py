from simulators.agent_simulator_tictoc import Agent
from config import CONFIG_FILE, ROOT_PATH, CONTENT_PATH, STEP_SIZE_1_MS


def test_agent_get_data_client0():
    """ Test get data for agent client0. """
    agent = Agent()
    agent._agent_name = 'client0'

    # agent client0 returns output in time 0 if send_msg is True or it saved
    # a message as self._answer. Otherwise, output should always be an empty
    # dict.

    # Set time to 1 and send_msg to False,
    # therefore, agent should return an empty dict.
    agent._time = 1
    agent._answer = None
    agent._send_msg = False
    assert agent.get_data(outputs={}) == {}

    # Time is 1 and send_msg is True, output should still be an empty dict.
    agent._time = 1
    agent._answer = None
    agent._send_msg = True
    assert agent.get_data(outputs={}) == {}

    # When the message saved in agent._answer is not None, the agent returns
    # a message. Therefore, the output of get_data is not an empty dict
    # anymore.
    agent._answer = {'message': 'Tic'}
    assert agent.get_data(outputs={}) != {}

    # If the message saved in agent._answer is None again, but send_msg
    # is True and the time is 0, the agent should return something.
    agent._time = 0
    agent._answer = None
    agent._send_msg = True
    assert agent.get_data(outputs={}) != {}

    # If the time is correct but send_msg is False, the agent should
    # return an empty dict.
    agent._time = 0
    agent._answer = None
    agent._send_msg = False
    assert agent.get_data(outputs={}) == {}

    # If send_msg is True and the time is 0, it does not matter whether
    # parallel_msg is set to True or not, therefore, output should not
    # be empty in both cases.
    agent._send_msg = True
    agent._time = 0
    agent._answer = None
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) != {}

    agent._send_msg = True
    agent._time = 0
    agent._answer = None
    agent.parallel_msg = False
    assert agent.get_data(outputs={}) != {}


def test_agent_get_data_client1():
    """ Test get data for agent client0. """
    agent = Agent()
    agent._agent_name = 'client1'

    # Agent 1 returns something if the time is 0 and parallel_msg and
    # send_msg is set to True.
    agent._send_msg = True
    agent._time = 0
    agent._answer = None
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) != {}

    # If the time is 0 but one of parallel_msg and send_msg is not True,
    # the output is an empty dict.
    agent._time = 0
    agent._answer = None
    agent._send_msg = False
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) == {}

    agent._time = 0
    agent._answer = None
    agent._send_msg = True
    agent.parallel_msg = False
    assert agent.get_data(outputs={}) == {}

    # If send_msg and parallel_msg are set to True but the time is not 0,
    # the agent only returns something if there is a message saved in
    # agent_1._answer.
    agent._time = 1
    agent._answer = None
    agent._send_msg = True
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) == {}

    # If the agent saves a message in agent_1._answer, the output is not
    # empty.
    agent._answer = {'message': 'Tic'}
    assert agent.get_data(outputs={}) != {}


def test_agent_get_data_unknown_agent():
    """ Test get data for agent which is not client0 and not client1. """
    agent = Agent()
    agent._agent_name = 'clientx'

    # If the name of the agent is not client0 and also not client1, the
    # agent only returns something, if there is a message in self._answer.
    agent._time = 0
    agent._answer = None
    agent._send_msg = True
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) == {}

    agent._time = 1
    agent._answer = {'message': 'Tic'}
    agent._send_msg = None
    agent.parallel_msg = None
    assert agent.get_data(outputs={}) != {}


def test_agent_sim_w_o_inputs():
    agent_sim = Agent()
    agent_sim.init(sid='Agent-0', step_type='event-based',
                   config_file=ROOT_PATH / CONFIG_FILE,
                   agent_name='client0',
                   content_path=CONTENT_PATH,
                   step_size=STEP_SIZE_1_MS,
                   parallel_msg=False)
    agent_sim.create(num=1, model='A')

    agent_sim.step(time=1, inputs={}, max_advance=0)
    assert agent_sim._answer is None


def test_agent_sim_w_inputs():
    agent_sim = Agent()
    agent_sim.init(sid='Agent-0', step_type='event-based',
                   config_file=ROOT_PATH / CONFIG_FILE,
                   agent_name='client0',
                   content_path=CONTENT_PATH,
                   step_size=STEP_SIZE_1_MS,
                   parallel_msg=False)
    agent_sim.create(num=1, model='A')
    inputs = {'client0': {'message_with_delay': {
        'CommSim-0.client-0': {'sender': 'client1', 'receiver': 'client0', 'message': 'Tic toc on the clock!',
                               'delay': 0}}}}
    agent_sim.step(time=1, inputs=inputs, max_advance=0)
    expected_answer = {'type': 1,
                       'sender': 'client0',
                       'receiver': 'client1',
                       'max_advance': 0,
                       'message': 'Dont stop, make it pop'}
    assert agent_sim._answer == expected_answer


