from time import sleep
import mosaik.util
from cosima_core.util.util_functions import start_omnet, \
    check_omnet_connection, stop_omnet, \
    get_host_names, log, set_up_file_logging
from cosima_core.simulators.mango_example.active_agent import ActiveAgent
from cosima_core.simulators.mango_example.reply_agent import ReplyAgent
from mango.messages.codecs import JSON
from mango_library.negotiation.util import cohda_serializers

# Sim config. and other parameters
from scenario_config import USE_COMMUNICATION_SIMULATION, START_MODE, NETWORK, SIMULATION_END
import cosima_core.util.general_config as cfg

SIM_CONFIG = {
    'ContainerSim': {
        'python': 'cosima_core.simulators.mango_example.container_sim:SimpleContainerSimulator',
    },
    'CommunicationSimulator': {
        'python': 'cosima_core.simulators.communication_simulator:CommunicationSimulator',
    },
}

NUMBER_OF_MANGO_AGENTS = 2

codec = JSON()
for serializer in cohda_serializers:
    codec.add_serializer(*serializer())

# conversion factor to convert mango time in mosaik time and vice versa. For example, mango uses seconds and mosaik
# milliseconds, use 1000.
conversion_factor = 1000

if USE_COMMUNICATION_SIMULATION:
    # start connection to OMNeT++
    omnet_process = start_omnet(START_MODE, NETWORK)
    check_omnet_connection(cfg.PORT)

# Create World
world = mosaik.World(SIM_CONFIG, time_resolution=0.001, cache=False)

set_up_file_logging()

# generate container simulators
agent_models = {}
client_names = get_host_names(num_hosts=NUMBER_OF_MANGO_AGENTS)
client_agent_mapping = {
    'client0': 'activeAgent',
    'client1': 'replyAgent'
}
client_attribute_mapping = {}
client_prefix = 'client'
# for each client in OMNeT++, save the connect-attribute for mosaik
for client_name in client_names:
    client_attribute_mapping[client_name] = cfg.CONNECT_ATTR + client_name

port = 5876
# Instantiate container sim model
for idx in range(NUMBER_OF_MANGO_AGENTS):
    current_container_name = client_prefix + str(idx)
    # Start simulators
    if idx == 0:
        agent_type = ActiveAgent
    else:
        agent_type = ReplyAgent
    agent_models[current_container_name] = \
        world.start('ContainerSim', client_name=current_container_name, port=port, agent_type=agent_type,
                    client_names=client_names, client_agent_mapping=client_agent_mapping,
                    conversion_factor=conversion_factor, codec=codec).ContainerModel()
    port += 1

if USE_COMMUNICATION_SIMULATION:
    # start communication simulator
    comm_sim = world.start('CommunicationSimulator',
                           port=cfg.PORT,
                           client_attribute_mapping=client_attribute_mapping).CommunicationModel()
    # Connect entities
    for name in agent_models:
        world.connect(agent_models[name], comm_sim, f'message', weak=True)
        world.connect(comm_sim, agent_models[name], client_attribute_mapping[name])
else:
    world.connect(agent_models['client0'], agent_models['client1'], f'message', weak=True)
    world.connect(agent_models['client1'], agent_models['client0'], f'message')

# set initial event
world.set_initial_event(agent_models['client0'].sid, time=0)

# Run simulation
log(f'run until {SIMULATION_END}')
world.run(until=SIMULATION_END)
log("end of process")
sleep(5)
if USE_COMMUNICATION_SIMULATION:
    stop_omnet(omnet_process)
