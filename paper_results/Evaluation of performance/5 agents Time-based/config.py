from os.path import abspath
from pathlib import Path

# in milliseconds
STEP_SIZE_1_MS = 1
STEP_SIZE_10_MS = 10
STEP_SIZE_100_MS = 100

USED_STEP_SIZE = STEP_SIZE_1_MS

# end of simulation
SIMULATION_END = 500

# number of agents (2-20 is possible)
NUMBER_OF_AGENTS = 5

# path to load content for agent messages from
ROOT_PATH = Path(abspath(__file__)).parent
CONTENT_PATH = ROOT_PATH / 'util' / 'content.csv'

# path to data for pv plant
PV_DATA = '../data/pv_10kw.csv'
START = '2014-01-01 00:00:00'

# path to store results to
RESULTS_FILENAME = 'results/result_'
WRITE_RESULTS = True

# port for OMNeT++ connection
PORT = 4242

# agents send messages parallel with given offset
PARALLEL = False
OFFSET = 1

# Run simulation in verbose mode or not
VERBOSE = True

# Track CPU and RAM during simulation run
TRACK_PERFORMANCE = True

# name for mosaik attribute which is exchanged between CommSim and Agents,
# containing the message from OMNeT++
CONNECT_ATTR = 'message_with_delay_for_'

# choose from: 'ide', 'qtenv', 'cmd'
# ide: run OMNeT++ ide by starting it separately
# qtenv: start simulation window in OMNeT++ when running mosaik
# cmd: start OMNeT++-scenario directly in command line by running
# mosaik scenario
START_MODE = 'qtenv'

# Network to simulate
# choose from EventBasedWinzentTCP, EventBasedWinzentUDP,
# EventBasedUDP_SimulaneousMessageReceiving, RealTimeWinzent, EventBasedWifi,
# EventBasedCloud, LTENetwork_TCP, LTENetwork_UDP
NETWORK = 'EventBasedWinzentTCP'

# connect pv plant to agent?
AGENTS_WITH_PV_PLANT = ['client0', 'client1', 'client2', 'client3', 'client4']

# each entry must contain values for type of infrastructure change
# (Disconnect, Connect), time in ms and module. Module can be switch, client
# or router. Also add the number of the certain module, f.e. client1
INFRASTRUCTURE_CHANGES = []

# configure how long an agent "calculates" (->sleeps) in its get data method
# (in seconds), default should be 0
CALCULATING_TIMES = {
    'client0': 0,
    'client1': 0,
    'client2': 0
}

# configure simulators as time-based
TIME_BASED = True