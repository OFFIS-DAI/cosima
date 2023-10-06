import sys
from pathlib import Path
import argparse

import scenario_config
from cosima_core.scenarios import communication_scenario, negotiation_scenario
from cosima_core.scenarios.mango import mango_cohda_scenario, mango_simple_scenario, mango_units_scenario, \
    mango_direct_connection_scenario, mango_direct_connection_cohda_scenario


def boolean_string(s):
    if s not in {'False', 'True'}:
        raise ValueError('Not a valid boolean string')
    return s == 'True'


BASE_DIR = Path(__file__)
sys.path.append(str(BASE_DIR))

argParser = argparse.ArgumentParser()
argParser.add_argument("-s", "--scenario", type=str, help="name of scenario")
argParser.add_argument("-c", "--communication_simulation", type=boolean_string,
                       help="use communication simulation (False, True)")
argParser.add_argument("-e", "--end", type=int, help="simulation end in ms")
argParser.add_argument("-n", "--network", type=str, help="name of network in OMNeT++")
argParser.add_argument("-a", "--agents", type=int, help="number of agents")
argParser.add_argument("-l", "--logging_level", type=str, help="logging level (debug, info, warning)")
args = argParser.parse_args()

scenario = None

for arg in vars(args):
    value = getattr(args, arg)
    if not value:
        continue

    if arg == 'scenario':
        scenario = value
    if arg == 'communication_simulation':
        scenario_config.USE_COMMUNICATION_SIMULATION = value
    if arg == 'end':
        scenario_config.SIMULATION_END = value
    if arg == 'network':
        scenario_config.NETWORK = value
    if arg == 'agents':
        scenario_config.NUMBER_OF_AGENTS = value
    if arg == 'logging':
        scenario_config.LOGGING_LEVEL = value

if not scenario:
    # default option
    communication_scenario.main()
elif scenario == 'communication_scenario':
    communication_scenario.main()
elif scenario == 'negotiation_scenario':
    negotiation_scenario.main()
elif scenario == 'mango_cohda_scenario':
    mango_cohda_scenario.main()
elif scenario == 'mango_simple_scenario':
    mango_simple_scenario.main()
elif scenario == 'mango_units_scenario':
    mango_units_scenario.main()
elif scenario == 'mango_direct_connection_scenario':
    mango_direct_connection_scenario.main()
elif scenario == 'mango_direct_connection_cohda_scenario':
    mango_direct_connection_cohda_scenario.main()
else:
    raise Exception('Scenario not known!')
