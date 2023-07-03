import multiprocessing
import os
import random

import pytest

import scenario_config
import cosima_core.scenarios.negotiation_scenario as negotiation_scenario

# change working directory because the main is called from the
# test folder now
from tests.integration_tests.util_functions_for_test import terminate_test_process

cwd = os.path.abspath(os.path.dirname(__file__))
new_wd = os.path.abspath(cwd + "/../" + "/../cosima_core/scenarios")
os.chdir(new_wd)


class TestNegotiationScenario:

    @classmethod
    def setup_class(cls):
        scenario_config.LOGGING_LEVEL = "warning"
        scenario_config.LOG_TO_FILE = False

    @pytest.mark.parametrize("num_agents", [2, 4, 20, 50])
    @pytest.mark.parametrize("network", ["StarTopologyNetwork"])
    @pytest.mark.parametrize("until", [100, 130, 200])
    @pytest.mark.parametrize("threshold", [700, 1000])
    def test_negotiation_scenario(self, num_agents, network, until, threshold):
        check_inbox_interval = random.randint(1, 200)
        scenario_config.NUMBER_OF_AGENTS = num_agents
        scenario_config.NETWORK = network
        scenario_config.SIMULATION_END = until
        negotiation_scenario.THRESHOLD = threshold
        negotiation_scenario.CHECK_INBOX_INTERVAL = check_inbox_interval
        print(f'------- Run negotiation scenario with {num_agents} agents, a threshold of {threshold} and check inbox'
              f'interval of {check_inbox_interval} and until {until}.  -------')
        p = multiprocessing.Process(target=negotiation_scenario.main)
        p.start()
        terminate_test_process(p)
