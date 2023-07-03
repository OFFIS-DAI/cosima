"""
    Scenario file for negotiation scenario.
"""
import matplotlib.pyplot as plt
import networkx as nx
from householdsim.mosaik import meta as household_meta

from cosima_core.util.general_config import START, HOUSEHOLD_DATA, ROOT_PATH
from cosima_core.util.scenario_setup_util import ScenarioHelper
from scenario_config import NUMBER_OF_AGENTS, USE_COMMUNICATION_SIMULATION, INFRASTRUCTURE_CHANGES

THRESHOLD = 700
CHECK_INBOX_INTERVAL = 50
PV_DATA = str(ROOT_PATH.parent / 'data' / 'pv_paper.csv')


def main():
    # Simulation configuration -> tells mosaik where to find the simulators
    sim_config = {
        'Agent': {
            'python': 'cosima_core.simulators.negotiation_example.agent_simulator:Agent',
        },
        'Collector': {
            'python': 'cosima_core.simulators.collector:Collector'},
        'CSV': {'python': 'cosima_core.simulators.tic_toc_example.mosaik_csv:CSV'},
        'Household': {'python': 'householdsim.mosaik:HouseholdSim'},
        'ICTController': {
            'python': 'cosima_core.simulators.ict_controller_simulator:ICTController'},
    }
    scenario_helper = ScenarioHelper()
    world, communication_simulator, client_attribute_mapping, sim_config = \
        scenario_helper.prepare_scenario(sim_config=sim_config)

    G = nx.newman_watts_strogatz_graph(n=NUMBER_OF_AGENTS, k=2, p=0.5, seed=42)
    pos = nx.circular_layout(G)
    plt.figure(figsize=(16, 8))
    nx.draw_networkx(G, pos, with_labels=True, font_size=12, node_color="#b4c7e7", edgecolors="#44546a",
                     edge_color="#44546a", width=1.5, verticalalignment="center_baseline", font_weight='normal',
                     linewidths=1.5)
    # plt.show()

    collector = world.start('Collector').Monitor()
    agents = {}
    for i in range(NUMBER_OF_AGENTS):
        neighbors = [f'client{x}' for x in list(nx.all_neighbors(G, i))]
        # Start agent simulator models
        start_negotiation = i == 0
        agents[f'client{i}'] = world.start('Agent',
                                           client_name=f'client{i}',
                                           neighbors=neighbors,
                                           threshold=THRESHOLD,
                                           check_inbox_interval=CHECK_INBOX_INTERVAL,
                                           start_negotiation=start_negotiation).AgentModel()
    if len(INFRASTRUCTURE_CHANGES) > 0 and USE_COMMUNICATION_SIMULATION:
        ict_controller = world.start('ICTController',
                                     infrastructure_changes=INFRASTRUCTURE_CHANGES).ICT()
    pv_sim = world.start('CSV', sim_start=START, datafile=PV_DATA,
                         delimiter=',', mosaik_attrs=['ACK', 'P'])
    pv_models = pv_sim.PV.create(len(agents))

    for idx, agent in enumerate(agents.
                                        values()):
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
        world.connect(agent, communication_simulator, f'message', weak=True)
        world.connect(communication_simulator, agent, client_attribute_mapping[agent.eid])
        world.connect(communication_simulator, collector, client_attribute_mapping[agent.eid])

    world.set_initial_event(agents['client0'].sid, time=CHECK_INBOX_INTERVAL)

    if len(INFRASTRUCTURE_CHANGES) > 0 and USE_COMMUNICATION_SIMULATION:
        world.set_initial_event(ict_controller.sid)
        # connect ict controller to collector
        world.connect(ict_controller, collector, f'ict_message')
        # connect ict controller with communication_simulator
        world.connect(ict_controller, communication_simulator, f'ict_message')
        world.connect(communication_simulator, ict_controller, f'ctrl_message', weak=True)

    scenario_helper.run_simulation()
    scenario_helper.shutdown_simulation()


if __name__ == '__main__':
    main()
