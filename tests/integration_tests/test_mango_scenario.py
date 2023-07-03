import multiprocessing
import os
import random

import pytest

import scenario_config
import cosima_core.scenarios.mango.mango_cohda_scenario as mango_cohda_scenario
import cosima_core.scenarios.mango.mango_units_scenario as mango_units_scenario

# change working directory because the main is called from the
# test folder now
from cosima_core.scenarios.mango import mango_simple_scenario
from tests.integration_tests.util_functions_for_test import terminate_test_process

cwd = os.path.abspath(os.path.dirname(__file__))
new_wd = os.path.abspath(cwd + "/../" + "/../cosima_core/scenarios")
os.chdir(new_wd)


class TestMangoScenario:

    @classmethod
    def setup_class(cls):
        scenario_config.LOGGING_LEVEL = "warning"
        scenario_config.LOG_TO_FILE = False

    @pytest.mark.parametrize("network", ["StarTopologyNetwork"])
    def test_smaces_scenario(self, network):
        num_agents = 2
        until = random.randint(100, 500)

        print(f'------- Run simple mango scenario with {num_agents} agents and until {until}.  -------')
        scenario_config.NUMBER_OF_AGENTS = num_agents
        scenario_config.NETWORK = network
        scenario_config.SIMULATION_END = until
        p = multiprocessing.Process(target=mango_simple_scenario.main)
        p.start()
        terminate_test_process(p)

    @pytest.mark.parametrize("num_agents", [5, 10, 20, 50, 60])
    @pytest.mark.parametrize("network", ["StarTopologyNetwork"])
    def test_smaces_cohda_scenario(self, num_agents, network):
        print(f'------- Run mango COHDA scenario with {num_agents} agents.  -------')
        scenario_config.NUMBER_OF_AGENTS = num_agents
        scenario_config.NETWORK = network
        p = multiprocessing.Process(target=mango_cohda_scenario.main)
        p.start()
        terminate_test_process(p)

    @pytest.mark.parametrize("num_agents", [5, 10, 20, 50, 60])
    @pytest.mark.parametrize("network", ["StarTopologyNetwork"])
    def test_smaces_unit_scenario(self, num_agents, network):
        scenario_config.NUMBER_OF_AGENTS = num_agents
        scenario_config.NETWORK = network
        scenario_config.SIMULATION_END = random.randint(100, 500)

        print(f'------- Run mango unit scenario with {num_agents} agents and until {scenario_config.SIMULATION_END}.  '
              f'-------')
        p = multiprocessing.Process(target=mango_units_scenario.main)
        p.start()
        terminate_test_process(p)
