from unittest.mock import MagicMock
import mosaik

from simulators.agent_simulator import Agent
from config import CONTENT_PATH, USED_STEP_SIZE


def mock_world(agent):
    agent.mosaik = MagicMock(dict())
    agent.mosaik.world = MagicMock(mosaik.scenario.World)
    agent.mosaik.world.until = 1000


def test_agent_get_data_client0():
    """ Test get data for agent client0. """

    agent = Agent()
    agent._agent_name = 'client0'
    mock_world(agent)

    # agent client0 returns output in time 0 if send_msg is True or it saved
    # a message as self._answer. Otherwise, output should always be an empty
    # dict.

    # Set output time to 1, send_msg to False,
    # therefore, agent should return an empty dict.
    agent._output_time = 1
    agent._send_msg = False
    assert agent.get_data(outputs={}) == {}

    # If the time is 0 and send_msg
    # is True and the time is 0, the agent should return something.
    agent._time = 0
    agent._send_msg = True
    assert agent.get_data(outputs={}) != {}

    # When the output time of the agent is 1 and send_msg is set to True, the agent returns
    # a message. Therefore, the output of get_data is not an empty dict
    # anymore.
    agent._output_time = 1
    agent._time = 1
    agent._send_msg = True
    assert agent.get_data(outputs={}) != {}

    # If the time is correct but send_msg is False, the agent should
    # return an empty dict.
    agent._time = 0
    agent._send_msg = False
    assert agent.get_data(outputs={}) == {}

    # If send_msg is True and the time is 0, it does not matter whether
    # parallel_msg is set to True or not, therefore, output should not
    # be empty in both cases.
    agent._send_msg = True
    agent._time = 0
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) != {}

    agent._send_msg = True
    agent._time = 0
    agent.parallel_msg = False
    assert agent.get_data(outputs={}) != {}


def test_agent_get_data_client1():
    """ Test get data for agent client0. """
    agent = Agent()
    agent._agent_name = 'client1'
    mock_world(agent)

    agent.mosaik = MagicMock(dict())
    agent.mosaik.world = MagicMock(mosaik.scenario.World)
    agent.mosaik.world.until = 1000

    # agent client1 returns output in time 1 if send_msg is True and the parallel scenario is executed.
    # Otherwise, output should always be an empty dict.

    # Set output time to 1, send_msg to False,
    # therefore, agent should return an empty dict.
    agent._output_time = 1
    agent._send_msg = False
    assert agent.get_data(outputs={}) == {}

    # If the time is 0 and send_msg
    # is True and the time is 0, the agent should return nothing.
    agent._output_time = 0
    agent._send_msg = True
    assert agent.get_data(outputs={}) == {}

    # If the time is correct but send_msg is False, the agent should
    # return an empty dict.
    agent._output_time = 2
    agent._send_msg = False
    assert agent.get_data(outputs={}) == {}

    # If send_msg is True and the time is 1, the agent should have an output if parallel_msg ist True
    # and the agent should have no output if parallel_msg is False
    agent._send_msg = True
    agent._output_time = 2
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) != {}

    agent._send_msg = True
    agent._output_time = 2
    agent.parallel_msg = False
    assert len(agent.get_data({})) != 0


def test_agent_get_data_unknown_agent():
    """ Test get data for agent which is not client0 and not client1. """
    agent = Agent()
    agent._agent_name = 'clientx'
    mock_world(agent)

    # If the name of the agent is not client0 and also not client1, the
    # agent only returns something, if there is a message in self._msgs.
    agent._output_time = 0
    agent._msgs = []
    agent._send_msg = True
    agent.parallel_msg = True
    assert agent.get_data(outputs={}) == {}

    agent._output_time = 1
    agent._msgs = {'message': 'Tic'}
    agent._send_msg = None
    agent.parallel_msg = None
    assert agent.get_data(outputs={}) != {}


def test_agent_sim_w_o_inputs():
    agent_sim = Agent()
    mock_world(agent_sim)
    agent_sim.init(sid='Agent-0', step_type='event-based',
                   agent_name='client0',
                   content_path=CONTENT_PATH,
                   step_size=USED_STEP_SIZE)
    agent_sim.create(num=1, model='A')

    agent_sim.step(time=1, inputs={}, max_advance=0)
    assert len(agent_sim._msgs) == 0


def test_agent_sim_w_inputs():
    agent_sim = Agent()
    agent_sim.init(sid='Agent-0', step_type='event-based',
                   agent_name='client0',
                   content_path=CONTENT_PATH,
                   step_size=USED_STEP_SIZE)
    agent_sim.create(num=1, model='A')
    agent_sim._output_time = 1
    inputs = {'client0': {'message_with_delay': {
        'CommSim-0.client-0': [{'sender': 'client1', 'receiver': 'client0', 'message': 'Tic toc on the clock!',
                                'output_time': 1}]}}}
    agent_sim.step(time=1, inputs=inputs, max_advance=0)
    expected_answer = [{'max_advance': 0,
                        'message': 'Dont stop, make it pop',
                        'msgId': 'AgentMessage_client0_0',
                        'receiver': 'client1',
                        'sender': 'client0',
                        'simTime': 2,
                        'stepSize': 1,
                        'type': 1}]
    assert agent_sim._msgs == expected_answer
