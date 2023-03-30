from copy import deepcopy
from os import listdir
from time import sleep

import mosaik.util
import pandas as pd

import cosima_core.util.general_config as cfg
from cosima_core.util.util_functions import get_host_names, log, start_omnet, check_omnet_connection, \
    set_up_file_logging, stop_omnet
from mango.agent.role import RoleAgent
from mango.messages.codecs import JSON
from mango_library.negotiation.util import cohda_serializers
from scenario_config import START_MODE, USE_COMMUNICATION_SIMULATION
import scenario_config

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
for serializer in cohda_serializers:
    codec.add_serializer(*serializer())

USE_COMMUNICATION_SIMULATION = True

# conversion factor to convert mango time in mosaik time and vice versa. For example, mango uses seconds and mosaik
# milliseconds, use 1000.
conversion_factor = 1000
# target 20 agents Intraday:
# [3200, 3200, 3200, 3200, 4000], same weight
# Target 20 agents DayAhead:
# [4000, 4000, 4000, 4000, 4000], same weight
target = ([1000, 1000, 1000, 1000], [1, 1, 1, 1])


def main(num_mango_agents=85, omnet_network='StarTopologyNetwork', sim_end=10000000, logging_level="debug"):
    # adapt maximal byte size per msg group, since message get bigger if more agents are involved
    # TODO: change for other number of agents
    if 5 <= num_mango_agents < 10:
        cfg.MAX_BYTE_SIZE_PER_MSG_GROUP = 3000
    elif 10 <= num_mango_agents < 20:
        cfg.MAX_BYTE_SIZE_PER_MSG_GROUP = 6000
    elif 20 <= num_mango_agents < 50:
        cfg.MAX_BYTE_SIZE_PER_MSG_GROUP = 8000
    elif num_mango_agents >= 50:
        cfg.MAX_BYTE_SIZE_PER_MSG_GROUP = 30000

    scenario_config.LOGGING_LEVEL = logging_level

    filenames = listdir(cfg.CHP_DATA)
    all_schedule_names = [filename for filename in filenames if filename.startswith("CHP")]
    all_schedules = {}
    idx = 0

    for filename in all_schedule_names:
        df = pd.read_csv(cfg.CHP_DATA + '/' + filename, index_col=None, header=0)
        all_schedules[idx] = []

        for i in range(df.shape[1]):
            all_schedules[idx].append(df[str(i)].to_list())
        idx += 1
    prec = len(all_schedules)
    # TODO
    for i in range(prec, num_mango_agents):
        all_schedules[i] = deepcopy(all_schedules[0])

    if USE_COMMUNICATION_SIMULATION:
        # start connection to OMNeT++
        omnet_process = start_omnet(START_MODE, omnet_network)
        check_omnet_connection(cfg.PORT)

    set_up_file_logging()

    # Create World
    world = mosaik.World(SIM_CONFIG, time_resolution=0.001, cache=False)

    # generate container simulators
    agent_models = {}
    client_names = get_host_names(num_hosts=num_mango_agents)

    client_agent_mapping = {}
    for idx in range(num_mango_agents):
        client_agent_mapping['client' + str(idx)] = 'cohdaAgent' + str(idx)

    client_attribute_mapping = {}
    client_prefix = 'client'
    # for each client in OMNeT++, save the connect-attribute for mosaik
    for client_name in client_names:
        client_attribute_mapping[client_name] = cfg.CONNECT_ATTR + client_name

    port = 5876
    # Instantiate container sim model
    for idx in range(num_mango_agents):
        current_container_name = client_prefix + str(idx)
        # Start simulators
        agent_type = RoleAgent

        def schedule_provider(i):
            return deepcopy(lambda: all_schedules[i])

        if idx == 0:
            agent_models[current_container_name] = \
                world.start('ContainerSim', client_name=current_container_name, port=port, agent_type=agent_type,
                            client_names=client_names, client_agent_mapping=client_agent_mapping,
                            conversion_factor=conversion_factor,
                            schedule_provider=schedule_provider(idx), is_controller_agent=True,
                            target=target, codec=codec).ContainerModel()
        elif idx == 1:
            agent_models[current_container_name] = \
                world.start('ContainerSim', client_name=current_container_name, port=port, agent_type=agent_type,
                            client_names=client_names, client_agent_mapping=client_agent_mapping,
                            conversion_factor=conversion_factor,
                            schedule_provider=schedule_provider(idx), is_controller_agent=False, is_starter=True,
                            target=target, codec=codec).ContainerModel()
        else:
            agent_models[current_container_name] = \
                world.start('ContainerSim', client_name=current_container_name, port=port, agent_type=agent_type,
                            client_names=client_names, client_agent_mapping=client_agent_mapping,
                            conversion_factor=conversion_factor,
                            schedule_provider=schedule_provider(idx), codec=codec).ContainerModel()
        port += 1

    # start communication simulator
    comm_sim = world.start('CommunicationSimulator',
                           port=4242,
                           client_attribute_mapping=client_attribute_mapping,
                           use_communication_simulation=USE_COMMUNICATION_SIMULATION).CommunicationModel()
    # Connect entities
    for name in agent_models:
        world.connect(agent_models[name], comm_sim, f'message', weak=True)
        world.connect(comm_sim, agent_models[name], client_attribute_mapping[name])

    # set initial event
    world.set_initial_event(agent_models['client0'].sid, time=0)

    # Run simulation
    log(f'run until {sim_end}')
    world.run(until=sim_end)
    log("end of process")
    sleep(5)

    if USE_COMMUNICATION_SIMULATION:
        stop_omnet(omnet_process)


if __name__ == '__main__':
    main()
