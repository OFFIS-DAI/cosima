# indicates if message are sent over OMNeT++ network
USE_COMMUNICATION_SIMULATION = True

# can be 'warnings' (only warnings), 'info' (only important information), 'debug' (all information)
LOGGING_LEVEL = 'info'
LOG_TO_FILE = True

# end of simulation (in milliseconds)
SIMULATION_END = 100

# number of agents (depends on chosen network in OMNeT++)
NUMBER_OF_AGENTS = 2

WRITE_RESULTS = False

# Track CPU and RAM during simulation run
TRACK_PERFORMANCE = False

# choose from: 'ide', 'qtenv', 'cmd'
# ide: run OMNeT++ ide by starting it separately
# qtenv: start simulation window in OMNeT++ when running mosaik
# cmd: start OMNeT++-scenario directly in command line by running
# mosaik scenario
START_MODE = 'cmd'

# Network to simulate
# Networks are stored in cosima_omnetpp_project/networks.
# Configurations can be found in cosima_omnetpp_project/mosaik.ini.
NETWORK = 'SimpleNetworkTCP'

# each entry must contain values for type of infrastructure change
# (Disconnect, Connect), time in ms and module. Module can be switch, client
# or router. Also add the number of the certain module, f.e. client1
INFRASTRUCTURE_CHANGES = [
    #     {'type': 'Disconnect',
    #      'time': 2,
    #      'module': 'client1'}
]

