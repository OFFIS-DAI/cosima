from time import sleep

import mosaik.util
from mango.role.core import RoleAgent
from mango.messages.codecs import JSON
from mango_library.negotiation.util import extra_serializers

from cosima_core.util.general_config import CONNECT_ATTR, PORT
from cosima_core.util.util_functions import start_omnet, \
    check_omnet_connection, stop_omnet, \
    get_host_names, log
from scenario_config import START_MODE, USE_COMMUNICATION_SIMULATION, NETWORK, SIMULATION_END

# Sim config. and other parameters
SIM_CONFIG = {
    'ContainerSim': {
        'python': 'cosima_core.simulators.mango_example.container_sim:COHDAContainerSimulator',
    },
    'CommunicationSimulator': {
        'python': 'cosima_core.simulators.communication_simulator:CommunicationSimulator',
    },
}
codec = JSON()
for serializer in extra_serializers:
    codec.add_serializer(*serializer())

NUMBER_OF_MANGO_AGENTS = 10

# conversion factor to convert mango time in mosaik time and vice versa. For example, mango uses seconds and mosaik
# milliseconds, use 1000.
conversion_factor = 1000
target = ([110, 110, 110, 110, 110], [1, 1, 1, 1, 1])
s_array = [[[1, 1, 1, 1, 1], [4, 3, 3, 3, 3], [6, 6, 6, 6, 6], [9, 8, 8, 8, 8], [11, 11, 11, 11, 11]]]

if USE_COMMUNICATION_SIMULATION:
    # start connection to OMNeT++
    omnet_process = start_omnet(START_MODE, NETWORK)
    check_omnet_connection(PORT)

# Create World
world = mosaik.World(SIM_CONFIG, time_resolution=0.001, cache=False)

# generate container simulators
agent_models = {}
client_names = get_host_names(num_hosts=NUMBER_OF_MANGO_AGENTS)

client_agent_mapping = {}
for idx in range(NUMBER_OF_MANGO_AGENTS):
    client_agent_mapping['client'+str(idx)] = 'cohdaAgent' + str(idx)

client_attribute_mapping = {}
client_prefix = 'client'
# for each client in OMNeT++, save the connect-attribute for mosaik
for client_name in client_names:
    client_attribute_mapping[client_name] = CONNECT_ATTR + client_name

port = 5876
# Instantiate container sim model
for idx in range(NUMBER_OF_MANGO_AGENTS):
    current_container_name = client_prefix + str(idx)
    # Start simulators
    agent_type = RoleAgent

    if idx == 0:
        agent_models[current_container_name] = \
            world.start('ContainerSim', client_name=current_container_name, port=port, agent_type=agent_type,
                        client_names=client_names, client_agent_mapping=client_agent_mapping,
                        conversion_factor=conversion_factor,
                        schedules_for_agents=s_array, is_controller_agent=True, target=target,
                        codec=codec).ContainerModel()
    else:
        agent_models[current_container_name] = \
            world.start('ContainerSim', client_name=current_container_name, port=port, agent_type=agent_type,
                        client_names=client_names, client_agent_mapping=client_agent_mapping,
                        conversion_factor=conversion_factor,
                        schedules_for_agents=s_array, codec=codec).ContainerModel()
    port += 1

# start communication simulator
comm_sim = world.start('CommunicationSimulator',
                       step_size=1,
                       port=PORT,
                       client_attribute_mapping=client_attribute_mapping,
                       use_communication_simulation=USE_COMMUNICATION_SIMULATION).CommunicationModel()
# Connect entities
for name in agent_models:
    world.connect(agent_models[name], comm_sim, f'message', weak=True)
    world.connect(comm_sim, agent_models[name], client_attribute_mapping[name])

# set initial event
world.set_initial_event(agent_models['client0'].sid, time=0)

# Run simulation
log(f'run until {SIMULATION_END}')
world.run(until=SIMULATION_END)
log("end of process")
sleep(5)

if USE_COMMUNICATION_SIMULATION:
    stop_omnet(omnet_process)
