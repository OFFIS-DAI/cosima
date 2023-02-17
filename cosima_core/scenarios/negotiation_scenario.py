"""
    Scenario file for tutorial of cosima.
"""
from time import sleep

import matplotlib.pyplot as plt
import mosaik
import networkx as nx
from householdsim.mosaik import meta as household_meta

from cosima_core.util.general_config import PV_DATA, START, HOUSEHOLD_DATA
from cosima_core.util.util_functions import start_omnet, \
    check_omnet_connection, stop_omnet, \
    log
from scenario_config import NUMBER_OF_AGENTS, USE_COMMUNICATION_SIMULATION

PORT = 4242
SIMULATION_END = 10000
START_MODE = 'cmd'
NETWORK = 'LargeLTENetwork'

# Simulation configuration -> tells mosaik where to find the simulators
SIM_CONFIG = {
    'Agent': {
        'python': 'cosima_core.simulators.negotiation_example.agent_simulator:Agent',
    },
    'Collector': {
        'python': 'cosima_core.simulators.collector:Collector'},
    'CommunicationSimulator': {
        'python': 'cosima_core.simulators.communication_simulator:CommunicationSimulator',
    },
    'CSV': {'python': 'cosima_core.simulators.tic_toc_example.mosaik_csv:CSV'},
    'Household': {'python': 'householdsim.mosaik:HouseholdSim'},
    'ICTController': {
        'python': 'cosima_core.simulators.ict_controller_simulator:ICTController'},

}
THRESHOLD = 700
CHECK_INBOX_INTERVAL = 50
PV_DATA = '../../data/pv_paper.csv'

INFRASTRUCTURE_CHANGES = [
    # {'type': 'Disconnect',
    #  'time': 10,
    #  'module': 'client1'}
]


def main(start_mode=START_MODE, number_of_agents=NUMBER_OF_AGENTS, network=NETWORK, simulation_end=SIMULATION_END,
         threshold=THRESHOLD, check_inbox_interval=CHECK_INBOX_INTERVAL, infrastructure_changes=INFRASTRUCTURE_CHANGES):
    if USE_COMMUNICATION_SIMULATION:
        omnet_process = start_omnet(start_mode, network)
        check_omnet_connection(PORT)

    # Create mosaik World
    world = mosaik.World(SIM_CONFIG, time_resolution=0.001, cache=False)

    G = nx.newman_watts_strogatz_graph(n=number_of_agents, k=2, p=0.5, seed=42)
    pos = nx.circular_layout(G)
    plt.figure(figsize=(16, 8))
    nx.draw_networkx(G, pos, with_labels=True, font_size=12, node_color="#b4c7e7", edgecolors="#44546a",
                     edge_color="#44546a", width=1.5, verticalalignment="center_baseline", font_weight='normal',
                     linewidths=1.5)
    # plt.show()

    collector = world.start('Collector').Monitor()
    agents = {}
    client_attribute_mapping = {}
    for i in range(number_of_agents):
        client_attribute_mapping[f'client{i}'] = f'message_with_delay_for_client{i}'
        neighbors = [f'client{x}' for x in list(nx.all_neighbors(G, i))]
        # Start agent simulator models
        start_negotiation = i == 0
        agents[f'client{i}'] = world.start('Agent',
                                           client_name=f'client{i}',
                                           neighbors=neighbors,
                                           threshold=threshold,
                                           check_inbox_interval=check_inbox_interval,
                                           start_negotiation=start_negotiation).AgentModel()

    # start communication simulator
    comm_sim = world.start('CommunicationSimulator',
                           step_size=1,
                           port=4242,
                           client_attribute_mapping=client_attribute_mapping,
                           use_communication_simulation=USE_COMMUNICATION_SIMULATION).CommunicationModel()

    if len(infrastructure_changes) > 0 and USE_COMMUNICATION_SIMULATION:
        ict_controller = world.start('ICTController',
                                     infrastructure_changes=infrastructure_changes).ICT()
    pv_sim = world.start('CSV', sim_start=START, datafile=PV_DATA,
                         delimiter=',')
    pv_sim.meta['models']['PV']['attrs'].append('ACK')
    pv_sim.meta['models']['PV']['attrs'].append('P')
    pv_models = pv_sim.PV.create(len(agents))

    for idx, agent in enumerate(agents.values()):
        world.connect(pv_models[idx], agents[agent.eid], 'P', time_shifted=True, initial_data={'P': 0})
        world.connect(agents[agent.eid], pv_models[idx], 'ACK')
        world.set_initial_event(pv_models[idx].sid)

        household_meta['models']['ResidentialLoads']['attrs'].append('LOAD_ACK')
        household_meta['models']['House']['attrs'].append('LOAD_ACK')
        household_meta['type'] = 'event-based'

        household_sim = world.start('Household')
        houses = household_sim.ResidentialLoads.create(1, sim_start=START,
                                                       profile_file=HOUSEHOLD_DATA, grid_name="medium grid")[0]
        # pick random house
        house_index = int(agent.eid[-1])
        world.connect(houses.children[house_index], agents[agent.eid], ('P_out', 'P'), weak=True)
        world.connect(agents[agent.eid], houses.children[house_index], 'LOAD_ACK')
        world.set_initial_event(houses.sid)

    # Connect entities of simple agents
    for agent in agents.values():
        world.connect(agent, comm_sim, f'message', weak=True)
        world.connect(comm_sim, agent, client_attribute_mapping[agent.eid])
        world.connect(comm_sim, collector, client_attribute_mapping[agent.eid])

    world.set_initial_event(agents['client0'].sid, time=CHECK_INBOX_INTERVAL)

    if len(INFRASTRUCTURE_CHANGES) > 0 and USE_COMMUNICATION_SIMULATION:
        world.set_initial_event(ict_controller.sid)
        # connect ict controller to collector
        world.connect(ict_controller, collector, f'ict_message')
        # connect ict controller with communication_simulator
        world.connect(ict_controller, comm_sim, f'ict_message')
        world.connect(comm_sim, ict_controller, f'ctrl_message', weak=True)

    # Run simulation
    log(f'run until {SIMULATION_END}')
    world.run(until=SIMULATION_END)
    log("end of process")
    sleep(5)

    if USE_COMMUNICATION_SIMULATION:
        stop_omnet(omnet_process)


if __name__ == '__main__':
    main()
