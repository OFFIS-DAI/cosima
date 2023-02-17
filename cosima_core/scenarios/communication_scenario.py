import random
from time import sleep
import string

from mosaik import scenario

import cosima_core.util.general_config as cfg
import scenario_config
from cosima_core.util.util_functions import start_omnet, \
    check_omnet_connection, stop_omnet, \
    get_host_names, log
from householdsim.mosaik import meta as household_meta

# agents send messages parallel with given offset
PARALLEL = True
OFFSET = 1

# connect pv plant to agent?
AGENTS_WITH_PV_PLANT = ['client0', 'client1']

# connect household to agent
AGENTS_WITH_HOUSEHOLDS = ['client1']

# configure how long an agent "calculates" (->sleeps) in its get data method
# (in seconds), default should be 0
CALCULATING_TIMES = {
    'client0': 0,
    'client1': 0,
    'client2': 0
}

# length of the content string sent by the agents
CONTENT_LENGTH = None
CONTENT_PATH = cfg.ROOT_PATH / 'simulators' / 'tic_toc_example' / 'content.csv'


def main(num_agents=None, omnet_network=None, parallel=False, agents_with_pv=None, agents_with_household=None,
         offset=0, sim_end=None, infrastructure_changes=None, content_length=None):
    if num_agents:
        scenario_config.NUMBER_OF_AGENTS = num_agents
    if infrastructure_changes:
        scenario_config.INFRASTRUCTURE_CHANGES = infrastructure_changes
    if omnet_network:
        scenario_config.START_MODE = "cmd"
        scenario_config.NETWORK = omnet_network
    if parallel is None:
        parallel = PARALLEL
        offset = OFFSET
    if sim_end:
        scenario_config.SIMULATION_END = sim_end
    if agents_with_pv is not None:
        # could also be an empty list
        scenario_config.AGENTS_WITH_PV_PLANT = agents_with_pv
    if agents_with_household is not None:
        scenario_config.AGENTS_WITH_HOUSEHOLDS = agents_with_household

    sim_config = {
        'Collector': {
            'python': 'cosima_core.simulators.collector:Collector'},
        'CommunicationSimulator': {'python': 'cosima_core.simulators.communication_simulator:CommunicationSimulator'},
        'Agent': {
            'python': 'cosima_core.simulators.tic_toc_example.agent_simulator:Agent'},
        'ICTController': {
            'python': 'cosima_core.simulators.ict_controller_simulator:ICTController'},
        'CSV': {'python': 'cosima_core.simulators.tic_toc_example.mosaik_csv:CSV'},
        'Household': {'python': 'householdsim.mosaik:HouseholdSim'},
    }
    if scenario_config.USE_COMMUNICATION_SIMULATION:
        omnet_process = start_omnet(scenario_config.START_MODE, scenario_config.NETWORK)
        check_omnet_connection(cfg.PORT)
    if scenario_config.TRACK_PERFORMANCE:
        from cosima_core.util.track_performance import PerformanceTracker
        tracker = PerformanceTracker()

    if CONTENT_LENGTH:
        message_content = ''.join(random.choices(string.ascii_lowercase + string.digits, k=CONTENT_LENGTH))
    else:
        message_content = None

    world = scenario.World(sim_config, time_resolution=0.001,
                           cache=False)

    agent_names = get_host_names(num_hosts=scenario_config.NUMBER_OF_AGENTS)
    client_attribute_mapping = {}
    # for each client in OMNeT++, save the connect-attribute for mosaik
    for agent_name in agent_names:
        client_attribute_mapping[agent_name] = cfg.CONNECT_ATTR + agent_name

    agents = {}
    collector = world.start('Collector').Monitor()
    agent_prefix = 'client'
    communication_simulator = world.start('CommunicationSimulator',
                                          port=cfg.PORT,
                                          client_attribute_mapping=client_attribute_mapping,
                                          use_communication_simulation=scenario_config.USE_COMMUNICATION_SIMULATION)\
        .CommunicationModel()

    if len(scenario_config.INFRASTRUCTURE_CHANGES) > 0 and scenario_config.USE_COMMUNICATION_SIMULATION:
        ict_controller = world.start('ICTController',
                                     infrastructure_changes=scenario_config.INFRASTRUCTURE_CHANGES).ICT()

    for idx in range(scenario_config.NUMBER_OF_AGENTS):
        current_agent_name = agent_prefix + str(idx)
        agents[current_agent_name] = world.start('Agent',
                                                 agent_name=current_agent_name,
                                                 content_path=CONTENT_PATH,
                                                 agent_names=
                                                 get_host_names(
                                                     num_hosts=scenario_config.NUMBER_OF_AGENTS),
                                                 parallel=parallel,
                                                 offset=offset,
                                                 message_content=message_content).A()

    if len(AGENTS_WITH_PV_PLANT) > 0:
        pv_sim = world.start('CSV', sim_start=cfg.START, datafile=cfg.PV_DATA,
                             delimiter=',')
        pv_sim.meta['models']['PV']['attrs'].append('ACK')
        pv_sim.meta['models']['PV']['attrs'].append('P')
        pv_models = pv_sim.PV.create(len(AGENTS_WITH_PV_PLANT))

    if len(AGENTS_WITH_HOUSEHOLDS) > 0:
        household_meta['models']['ResidentialLoads']['attrs'].append('LOAD_ACK')
        household_meta['type'] = 'event-based'

        household_sim = world.start('Household')
        houses = household_sim.ResidentialLoads.create(1, sim_start=cfg.START,
                                                       profile_file=cfg.HOUSEHOLD_DATA,
                                                       grid_name="medium grid")[0]


    for idx, agent in enumerate(agents.values()):
        if agent.eid in AGENTS_WITH_PV_PLANT:
            world.connect(pv_models[idx], agents[agent.eid], 'P', time_shifted=True, initial_data={'P': 0})
            world.connect(agents[agent.eid], pv_models[idx], 'ACK')

            world.set_initial_event(pv_models[idx].sid)
        if agent.eid in AGENTS_WITH_HOUSEHOLDS:
            world.connect(houses.children[idx], agents[agent.eid], ('P_out', 'P'), weak=True)
            world.connect(agents[agent.eid], houses, 'LOAD_ACK')
            world.set_initial_event(houses.sid)
        else:
            world.set_initial_event(agent.sid, time=0)

    if len(scenario_config.INFRASTRUCTURE_CHANGES) > 0 and scenario_config.USE_COMMUNICATION_SIMULATION:
        world.set_initial_event(ict_controller.sid)

    if parallel:
        world.set_initial_event(agents['client1'].sid, time=offset)

    for agent_name, agent in agents.items():
        # connect agents with communication_models
        world.connect(agent, communication_simulator, f'message', weak=True)

    for client_name, attr in client_attribute_mapping.items():
        # connect communication_models with agents
        world.connect(communication_simulator, agents[client_name],
                      attr)
        world.connect(communication_simulator, collector,
                      attr)
    if len(scenario_config.INFRASTRUCTURE_CHANGES) > 0 and scenario_config.USE_COMMUNICATION_SIMULATION:
        # connect ict controller to collector
        world.connect(ict_controller, collector, f'ict_message')
        # connect ict controller with communication_simulator
        world.connect(ict_controller, communication_simulator, f'ict_message')
        world.connect(communication_simulator, ict_controller, f'ctrl_message', weak=True)

    until = scenario_config.SIMULATION_END
    log(f'run until {until}')
    world.run(until=until)
    log("end of process")
    sleep(5)
    if scenario_config.USE_COMMUNICATION_SIMULATION:
        stop_omnet(omnet_process)
    if scenario_config.TRACK_PERFORMANCE:
        tracker.save_results()


if __name__ == '__main__':
    main()
