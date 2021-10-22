import os
import time
import subprocess
from os.path import abspath
from pathlib import Path

# in milliseconds
STEP_SIZE_1_MS = 1
STEP_SIZE_10_MS = 10
STEP_SIZE_100_MS = 100

# number of agents
NUMBER_OF_AGENTS = 3

# path to load content from
ROOT_PATH = Path(abspath(__file__)).parent
CONTENT_PATH = ROOT_PATH / 'content.csv'

# config file
if NUMBER_OF_AGENTS == 2:
    CONFIG_FILE = 'network_config_basic_2_agents.json'
elif NUMBER_OF_AGENTS == 3:
    CONFIG_FILE = 'network_config_basic_3_agents.json'

# agents send messages parallel
PARALLEL = True

# choose from: 'ide', 'qtenv', 'cmd'
# ide: run OMNeT++ ide by starting it separately
# qtenv: start simulation window in OMNeT++ when running mosaik
# cmd: start OMNeT++-scenario directly in command line by running
# mosaik scenario
START_MODE = 'ide'

# Network to simulate
# choose from EventBasedWinzentTCP, EventBasedWinzentUDP,
# EventBasedUDP_SimulaneousMessageReceiving, RealTimeWinzent, EventBasedWifi,
# EventBasedCloud, EventBasedLTE, EventBasedMultiCellMeshMini,
# EventBasedMultiCellMesh
NETWORK = 'EventBasedWinzentTCP'

# name for mosaik attribute which is exchanged between CommSim and Agents,
# containing the message from OMNeT++
CONNECT_ATTR = 'message_with_delay_for_'
