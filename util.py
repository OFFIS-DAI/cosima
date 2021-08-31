from os.path import abspath
from pathlib import Path

# in milliseconds
STEP_SIZE_1_MS = 1
STEP_SIZE_10_MS = 10
STEP_SIZE_100_MS = 100

# number of agents
NUMBER_OF_AGENTS = 2

# path to load content from
ROOT_PATH = Path(abspath(__file__)).parent
CONTENT_PATH = ROOT_PATH / 'content.csv'

# config file
if NUMBER_OF_AGENTS == 2:
    CONFIG_FILE = 'network_config_basic_2_agents.json'
elif NUMBER_OF_AGENTS == 3:
    CONFIG_FILE = 'network_config_basic_3_agents.json'

# agents send messages parallel
PARALLEL = False
