import multiprocessing
import random
import pytest
import os

import scenario_config
from cosima_core.scenarios import communication_scenario

# change working directory because the main is called from the
# test folder now
from tests.integration_tests.util_functions_for_test import get_snapshot, terminate_test_process

cwd = os.path.abspath(os.path.dirname(__file__))
new_wd = os.path.abspath(cwd + "/../" + "/../cosima_core/scenarios")
os.chdir(new_wd)


class TestCommunicationScenario:

    @classmethod
    def setup_class(cls):
        scenario_config.LOGGING_LEVEL = "warning"
        scenario_config.LOG_TO_FILE = False

    def test_communication_scenario_simple_with_snapshot(self):
        print(f'------- Run communication scenario simple test case with 10 agents, parallel '
              f'and until 200. Communication simulation is set to True. -------')
        scenario_config.USE_COMMUNICATION_SIMULATION = True
        scenario_config.NUMBER_OF_AGENTS = 10
        scenario_config.NETWORK = 'StarTopologyNetwork'
        communication_scenario.PARALLEL = True
        communication_scenario.OFFSET = 1
        scenario_config.SIMULATION_END = 200
        p = multiprocessing.Process(target=communication_scenario.main)
        p.start()
        terminate_test_process(p)
        snapshot = get_snapshot()
        assert snapshot['events'] == 249416.0
        assert snapshot['messages'] == 2769.0
        assert snapshot['errors'] == 0.0
        assert snapshot['max_advance'] == 1.0
        assert snapshot['disconnect'] == 0.0
        assert snapshot['reconnect'] == 0.0
        assert snapshot['last_event'] == 0.142

    @pytest.mark.parametrize("use_communication_simulation", [False, True])
    @pytest.mark.parametrize("network", ["StarTopologyNetwork"])
    @pytest.mark.parametrize("parallel", [False, True])
    def test_communication_scenario_simple(self, use_communication_simulation, network, parallel):
        num_agents = random.randint(2, 50)
        until = random.randint(100, 200)
        print(f'------- Run communication scenario simple test case with {num_agents} agents, parallel is {parallel} '
              f'and until {until}. Communication simulation is set to {use_communication_simulation}. -------')
        scenario_config.USE_COMMUNICATION_SIMULATION = use_communication_simulation
        scenario_config.NUMBER_OF_AGENTS = num_agents
        scenario_config.SIMULATION_END = until
        scenario_config.NETWORK = network
        communication_scenario.PARALLEL = parallel
        communication_scenario.OFFSET = 1
        p = multiprocessing.Process(target=communication_scenario.main)
        p.start()
        terminate_test_process(p)

    @pytest.mark.parametrize("network", ["StarTopologyNetwork"])
    @pytest.mark.parametrize("agents_with_pv", [[], ['client0', 'client1'], ['client0', 'client1', 'client2']])
    @pytest.mark.parametrize("agents_with_household", [[], ['client0', 'client1'], ['client0', 'client1', 'client2']])
    def test_communication_scenario_with_pv_and_households(self, network, agents_with_pv, agents_with_household):
        num_agents = random.randint(3, 50)
        until = random.randint(100, 500)
        print(f'------- Run communication scenario pv/ household test case with '
              f'{num_agents} agents and until {until}. -------')
        scenario_config.NUMBER_OF_AGENTS = num_agents
        scenario_config.SIMULATION_END = until
        scenario_config.NETWORK = network
        communication_scenario.PARALLEL = False
        communication_scenario.AGENTS_WITH_PV_PLANT = agents_with_pv
        communication_scenario.AGENTS_WITH_HOUSEHOLDS = agents_with_household
        p = multiprocessing.Process(target=communication_scenario.main)
        p.start()
        terminate_test_process(p)

    def test_communication_scenario_disconnect_with_snapshot(self):
        infrastructure_changes = [{'type': 'Disconnect', 'time': 2, 'module': 'client1'}]
        print(f'------- Run communication scenario disconnect test case with 10 agents, parallel '
              f'and until 200. Communication simulation is set to True. -------')
        scenario_config.USE_COMMUNICATION_SIMULATION = True
        scenario_config.NUMBER_OF_AGENTS = 10
        scenario_config.SIMULATION_END = 200
        scenario_config.NETWORK = 'StarTopologyNetwork'
        scenario_config.INFRASTRUCTURE_CHANGES = infrastructure_changes
        p = multiprocessing.Process(target=communication_scenario.main)
        p.start()
        terminate_test_process(p)
        snapshot = get_snapshot()
        assert snapshot['events'] == 72376.0
        assert snapshot['messages'] == 811.0
        assert snapshot['errors'] == 1.0
        assert snapshot['max_advance'] == 1.0
        assert snapshot['disconnect'] == 1.0
        assert snapshot['reconnect'] == 0.0
        assert snapshot['last_event'] == 0.2

    @pytest.mark.parametrize("infrastructure_changes", [[{'type': 'Disconnect', 'time': 2, 'module': 'client1'}]])
    def test_communication_scenario_with_infrastructure_changes(self, infrastructure_changes):
        num_agents = random.randint(3, 50)
        until = random.randint(100, 500)
        print(f'------- Run communication scenario simple test case with infrastructure changes and  '
              f'{num_agents} agents and until {until}. -------')
        network = random.choice(['LargeLTENetwork', 'StarTopologyNetwork'])
        scenario_config.NUMBER_OF_AGENTS = num_agents
        scenario_config.SIMULATION_END = until
        scenario_config.NETWORK = network
        communication_scenario.PARALLEL = False
        scenario_config.INFRASTRUCTURE_CHANGES = infrastructure_changes
        p = multiprocessing.Process(target=communication_scenario.main)
        p.start()
        terminate_test_process(p)
