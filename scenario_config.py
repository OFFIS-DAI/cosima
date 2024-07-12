from data.traffic_configurations.traffic_config import ddos_traffic_configuration as traffic_configuration

# indicates if message are sent over OMNeT++ network
USE_COMMUNICATION_SIMULATION = True

# can be 'warnings' (only warnings), 'info' (only important information), 'debug' (all information)
LOGGING_LEVEL = 'debug'
LOG_TO_FILE = False

# end of simulation (in milliseconds)
SIMULATION_END = 1000000

# number of agents (depends on chosen network in OMNeT++)
NUMBER_OF_AGENTS = 21

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
# Configurations can be found in cosima_omnetpp_project/cosima.ini.
NETWORK = 'SimbenchNetwork'

# each entry must contain values for type of infrastructure change
# (Disconnect, Connect), time in ms and module. Module can be switch, client
# or router. Also add the number of the certain module, f.e. client1
INFRASTRUCTURE_CHANGES = [
    #     {'type': 'Disconnect',
    #      'start': 2,
    #      'module': 'client1'}
]

# The traffic configuration can either be done directly in this file or can be imported from a traffic configuration
# file. A traffic configuration may look like this:
# TRAFFIC_CONFIGURATION = [
#   {
#         'source': 'traffic_device_0',
#         'destination': 'client1',
#         'start': 1,
#         'stop': 100,
#         'interval': 10,
#         'packet_length': 1000
#     }
# ]
# Always make sure that your chosen network contains the needed traffic devices.
TRAFFIC_CONFIGURATION = []

ATTACK_CONFIGURATION = []
