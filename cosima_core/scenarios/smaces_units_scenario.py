from time import sleep

import mosaik.util
from mango.agent.role import RoleAgent
from mango.messages.codecs import JSON
from mango_library.negotiation.util import cohda_serializers

import cosima_core.util.general_config as cfg
from cosima_core.util.util_functions import get_host_names, log, start_omnet, check_omnet_connection, \
    set_up_file_logging, stop_omnet
from scenario_config import START_MODE, USE_COMMUNICATION_SIMULATION, NETWORK, NUMBER_OF_AGENTS, SIMULATION_END
from cosima_core.util.general_config import PV_DATA, START

# Sim config. and other parameters
SIM_CONFIG = {
    'ContainerSim': {
        'python': 'cosima_core.simulators.mango_example.container_sim:UnitContainerSimulator',
    },
    'CommunicationSimulator': {
        'python': 'cosima_core.simulators.communication_simulator:CommunicationSimulator',
    },
    'CSV': {
        'python': 'cosima_core.simulators.tic_toc_example.mosaik_csv:CSV'
    },
}
codec = JSON()
for serializer in cohda_serializers:
    codec.add_serializer(*serializer())

USE_COMMUNICATION_SIMULATION = True

# conversion factor to convert mango time in mosaik time and vice versa. For example, mango uses seconds and mosaik
# milliseconds, use 1000.
conversion_factor = 1000


def main():
    if USE_COMMUNICATION_SIMULATION:
        # start connection to OMNeT++
        omnet_process = start_omnet(START_MODE, NETWORK)
        check_omnet_connection(cfg.PORT)

    set_up_file_logging()

    # Create World
    world = mosaik.World(SIM_CONFIG, time_resolution=0.001, cache=False)

    # generate container simulators
    agent_models = {}
    client_names = get_host_names(num_hosts=NUMBER_OF_AGENTS)

    client_agent_mapping = {}
    for idx in range(NUMBER_OF_AGENTS):
        client_agent_mapping['client' + str(idx)] = 'unitAgent' + str(idx)

    client_attribute_mapping = {}
    client_prefix = 'client'
    # for each client in OMNeT++, save the connect-attribute for mosaik
    for client_name in client_names:
        client_attribute_mapping[client_name] = cfg.CONNECT_ATTR + client_name

    port = 5876
    # Instantiate container sim model
    for idx in range(NUMBER_OF_AGENTS):
        current_container_name = client_prefix + str(idx)
        # Start simulators
        agent_type = RoleAgent

        agent_models[current_container_name] = \
            world.start('ContainerSim', client_name=current_container_name, port=port, agent_type=agent_type,
                        client_names=client_names, client_agent_mapping=client_agent_mapping,
                        conversion_factor=conversion_factor, connect_attributes=['P', 'p_set_point'],
                        codec=codec).ContainerModel()
        port += 1

    pv_sim = world.start('CSV', sim_start=START, datafile=PV_DATA,
                         delimiter=',', mosaik_attrs=['p_set_point', 'P'])
    pv_models = pv_sim.PV.create(len(agent_models))

    for idx, agent in enumerate(agent_models.values()):
        world.connect(pv_models[idx], agent_models[agent.eid], 'P', time_shifted=True, initial_data={'P': 0})
        world.connect(agent_models[agent.eid], pv_models[idx], 'p_set_point')
        world.set_initial_event(pv_models[idx].sid)

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
    world.set_initial_event(agent_models['client0'].sid, time=1)

    # Run simulation
    log(f'run until {SIMULATION_END}')
    world.run(until=SIMULATION_END)
    log("end of process")
    sleep(5)

    if USE_COMMUNICATION_SIMULATION:
        stop_omnet(omnet_process)


if __name__ == '__main__':
    main()
