"""
    Scenario file for tutorial of cosima.
"""
from time import sleep

import mosaik

from cosima_core.util.util_functions import start_omnet, \
    check_omnet_connection, stop_omnet, \
    log

PORT = 4242
SIMULATION_END = 10000
START_MODE = 'cmd'
NETWORK = 'SimpleNetworkUDP'

# Simulation configuration -> tells mosaik where to find the simulators
SIM_CONFIG = {
    'SimpleAgent': {
        'python': 'cosima_core.simulators.tutorial.simple_agent_simulator:SimpleAgent',
    },
    'CommunicationSimulator': {
        'python': 'cosima_core.simulators.communication_simulator:CommunicationSimulator',
    }
}

omnet_process = start_omnet(START_MODE, NETWORK)
check_omnet_connection(PORT)

# Create mosaik World
world = mosaik.World(SIM_CONFIG, time_resolution=0.001, cache=False)

client_attribute_mapping = {
    'client0': 'message_with_delay_for_client0',
    'client1': 'message_with_delay_for_client1'
}

# Start agent simulator models
simple_agent_1 = world.start('SimpleAgent',
                             client_name='client0',
                             neighbor='client1').SimpleAgentModel()
simple_agent_2 = world.start('SimpleAgent',
                             client_name='client1',
                             neighbor='client0').SimpleAgentModel()

# start communication simulator
comm_sim = world.start('CommunicationSimulator',
                       step_size=1,
                       port=4242,
                       client_attribute_mapping=client_attribute_mapping).CommunicationModel()

# Connect entities of simple agents
world.connect(simple_agent_1, comm_sim, f'message', weak=True)
world.connect(comm_sim, simple_agent_1, client_attribute_mapping['client0'])
world.connect(simple_agent_2, comm_sim, f'message', weak=True)
world.connect(comm_sim, simple_agent_2, client_attribute_mapping['client1'])

# set initial event for simple agent
world.set_initial_event(simple_agent_1.sid, time=0)

# Run simulation
log(f'run until {SIMULATION_END}')
world.run(until=SIMULATION_END)
log("end of process")
sleep(5)
stop_omnet(omnet_process)
