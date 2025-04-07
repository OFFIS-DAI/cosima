"""
    Scenario file for tutorial of cosima.
"""
from cosima_core.util.scenario_setup_util import ScenarioHelper

import cosima_core.util.general_config as cfg
from scenario_config import NETWORK

CONTENT_PATH = cfg.ROOT_PATH / 'simulators' / 'tic_toc_example' / 'content.csv'

# Simulation configuration -> tells mosaik where to find the simulators
SIM_CONFIG = {
    'SimpleAgent': {
        'python': 'cosima_core.simulators.tutorial.simple_agent_simulator:SimpleAgent',
    },
    'StatisticsSimulator': {
        'python': 'cosima_core.simulators.statistics_simulator:StatisticsSimulator',
    }
}

scenario_helper = ScenarioHelper()
world, communication_simulator, client_attribute_mapping, sim_config = \
    scenario_helper.prepare_scenario(sim_config=SIM_CONFIG)

############################################################################
"""
Here comes your code.
"""
# Start agent simulator models
simple_agent_1 = world.start('SimpleAgent',
                             content_path=CONTENT_PATH,
                             client_name='client0',
                             neighbor='client1').SimpleAgentModel()
simple_agent_2 = world.start('SimpleAgent',
                             content_path=CONTENT_PATH,
                             client_name='client1',
                             neighbor='client0').SimpleAgentModel()

stat_sim = world.start('StatisticsSimulator', network=NETWORK, save_plots=True).Statistics()  # , step_time=200

# Connect entities of simple agents
world.connect(simple_agent_1, communication_simulator, f'message', weak=True)
world.connect(communication_simulator, simple_agent_1, client_attribute_mapping['client0'])
world.connect(simple_agent_2, communication_simulator, f'message', weak=True)
world.connect(communication_simulator, simple_agent_2, client_attribute_mapping['client1'])
world.connect(simple_agent_1, stat_sim, 'message', time_shifted=True, initial_data={'message': None})
world.connect(stat_sim, simple_agent_1, 'stats')
world.connect(stat_sim, simple_agent_2, 'stats')

# set initial event for simple agent
world.set_initial_event(simple_agent_1.sid, time=0)

"""
Here ends your code.
"""
############################################################################

scenario_helper.run_simulation()
scenario_helper.shutdown_simulation()
