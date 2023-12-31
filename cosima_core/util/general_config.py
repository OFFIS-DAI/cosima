from os.path import abspath
from pathlib import Path

# max size per message group
MAX_BYTE_SIZE_PER_MSG_GROUP = 30000

# units conversion factor for mango scenarios
MANGO_CONVERSION_FACTOR = 1000

# path to load content for agent messages from
ROOT_PATH = Path(abspath(__file__)).parent.parent

# path to data for pv plant
PV_DATA = str(ROOT_PATH.parent / 'data' / 'pv_10kw.csv')
START = '2014-01-01 10:00:00'

HOUSEHOLD_DATA = str(ROOT_PATH.parent / 'data' / 'household.data')
# choose between "small grid" or "medium grid"
GRID_NAME = "large grid"

CHP_DATA = str(ROOT_PATH.parent / 'data')

INET_INSTALLATION_PATH = '../inet4/src/inet'

# path to store results to
RESULTS_FILENAME = str(ROOT_PATH.parent / 'cosima_core' / 'results' / 'result_')

# port for OMNeT++ connection
PORT = 4242

# Run simulation in verbose mode or not
VERBOSE = True

# name for mosaik attribute which is exchanged between CommunicationSim and Agents,
# containing the message from OMNeT++
CONNECT_ATTR = 'message_with_delay_for_'
