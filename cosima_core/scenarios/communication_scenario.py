import random
from time import sleep

from mosaik import scenario

import cosima_core.config as cfg
from cosima_core.util.util_functions import start_omnet, \
    check_omnet_connection, stop_omnet, \
    get_host_names, log
from householdsim.mosaik import meta as household_meta


def main(num_agents=None, omnet_network=None, parallel=False, agents_with_pv=None, agents_with_household=None,
         offset=0, sim_end=None, infrastructure_changes=None, cwd=None):
    if num_agents:
        cfg.NUMBER_OF_AGENTS = num_agents
    if infrastructure_changes:
        cfg.INFRASTRUCTURE_CHANGES = infrastructure_changes
    if omnet_network:
        cfg.START_MODE = "cmd"
        cfg.NETWORK = omnet_network
    if parallel is None:
        parallel = cfg.PARALLEL
        offset = cfg.OFFSET
    if sim_end:
        cfg.SIMULATION_END = sim_end
    if agents_with_pv is not None:
        # could also be an empty list
        cfg.AGENTS_WITH_PV_PLANT = agents_with_pv
    if agents_with_household is not None:
        cfg.AGENTS_WITH_HOUSEHOLDS = agents_with_household

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
    omnet_process = start_omnet(cfg.START_MODE, cfg.NETWORK, cwd=cwd)
    check_omnet_connection(cfg.PORT)
    if cfg.TRACK_PERFORMANCE:
        from cosima_core.util.track_performance import PerformanceTracker
        tracker = PerformanceTracker()

    world = scenario.World(sim_config, debug=True, time_resolution=0.001,
                           cache=False)

    agent_names = get_host_names(num_hosts=cfg.NUMBER_OF_AGENTS)
    client_attribute_mapping = {}
    # for each client in OMNeT++, save the connect-attribute for mosaik
    for agent_name in agent_names:
        client_attribute_mapping[agent_name] = cfg.CONNECT_ATTR + agent_name

    agents = {}
    collector = world.start('Collector').Monitor()
    agent_prefix = 'client'
    communication_simulator = world.start('CommunicationSimulator',
                                          step_size=cfg.USED_STEP_SIZE,
                                          port=cfg.PORT,
                                          client_attribute_mapping=client_attribute_mapping).CommunicationModel()

    if len(cfg.INFRASTRUCTURE_CHANGES) > 0:
        ict_controller = world.start('ICTController',
                                     infrastructure_changes=cfg.
                                     INFRASTRUCTURE_CHANGES).ICT()

    for idx in range(cfg.NUMBER_OF_AGENTS):
        current_agent_name = agent_prefix + str(idx)
        agents[current_agent_name] = world.start('Agent',
                                                 agent_name=current_agent_name,
                                                 content_path=cfg.CONTENT_PATH,
                                                 step_size=cfg.USED_STEP_SIZE,
                                                 agent_names=
                                                 get_host_names(
                                                     num_hosts=cfg.NUMBER_OF_AGENTS),
                                                 parallel=parallel,
                                                 offset=offset).A()
    for agent in agents.values():
        if agent.eid in cfg.AGENTS_WITH_PV_PLANT:
            pv_sim = world.start('CSV', sim_start=cfg.START, datafile=cfg.PV_DATA,
                                 delimiter=',')
            pv_sim.meta['models']['PV']['attrs'].append('ACK')
            pv_sim.meta['models']['PV']['attrs'].append('P')

            pv = pv_sim.PV.create(1)[0]
            world.connect(pv, agents[agent.eid], 'P', weak=True)
            world.connect(agents[agent.eid], pv, 'ACK')

            world.set_initial_event(pv.sid)
        if agent.eid in cfg.AGENTS_WITH_HOUSEHOLDS:
            household_meta['models']['ResidentialLoads']['attrs'].append('LOAD_ACK')
            household_meta['type'] = 'event-based'

            household_sim = world.start('Household')
            houses = household_sim.ResidentialLoads.create(1, sim_start=cfg.START,
                                                           profile_file=cfg.HOUSEHOLD_DATA,
                                                           grid_name="medium grid")[0]
            # pick random house
            house_index = random.randint(0, len(houses.children) - 1)
            world.connect(houses.children[house_index], agents[agent.eid], ('P_out', 'P'), weak=True)
            world.connect(agents[agent.eid], houses, 'LOAD_ACK')
            world.set_initial_event(houses.sid)
        else:
            world.set_initial_event(agent.sid, time=0)

    if len(cfg.INFRASTRUCTURE_CHANGES) > 0:
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
    if len(cfg.INFRASTRUCTURE_CHANGES) > 0:
        # connect ict controller to collector
        world.connect(ict_controller, collector, f'ict_message')
        # connect ict controller with communication_simulator
        world.connect(ict_controller, communication_simulator, f'ict_message')
        world.connect(communication_simulator, ict_controller, f'ctrl_message', weak=True)

    until = cfg.SIMULATION_END * cfg.USED_STEP_SIZE
    log(f'run until {until}')
    world.run(until=until)
    log("end of process")
    sleep(5)
    stop_omnet(omnet_process)
    if cfg.TRACK_PERFORMANCE:
        tracker.save_results()


if __name__ == '__main__':
    main()
