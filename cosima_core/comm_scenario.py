import os
from time import sleep

from mosaik import scenario

from cosima_core.config import AGENTS_WITH_PV_PLANT, TIME_BASED
import cosima_core.config as cfg
from cosima_core.util.util_functions import start_omnet, \
    check_omnet_connection, stop_omnet, \
    get_host_names, log


def main(num_agents=None, omnet_network=None, parallel=False, offset=0, sim_end=None,
         infrastructure_changes=None):
    if num_agents:
        cfg.NUMBER_OF_AGENTS = num_agents
    if infrastructure_changes:
        cfg.INFRASTRUCTURE_CHANGES = infrastructure_changes
    if omnet_network:
        cfg.START_MODE = "cmd"
        cfg.NETWORK = omnet_network
    if parallel:
        cfg.PARALLEL = parallel
        cfg.OFFSET = offset
    if sim_end:
        cfg.SIMULATION_END = sim_end

    sim_config = {
        'Collector': {
            'python': 'cosima_core.simulators.collector:Collector'},
        'CommSim': {'python': 'cosima_core.simulators.comm_simulator:CommSimulator'},
        'Agent': {
            'python': 'cosima_core.simulators.tic_toc_example.agent_simulator:Agent'},
        'ICTController': {
            'python': 'cosima_core.simulators.ict_controller_simulator:ICTController'},
        'CSV': {'python': 'cosima_core.simulators.tic_toc_example.mosaik_csv:CSV'}

    }
    omnet_process = start_omnet(cfg.START_MODE, cfg.NETWORK)
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
    comm_sim = world.start('CommSim',
                           step_size=cfg.USED_STEP_SIZE,
                           port=cfg.PORT,
                           client_attribute_mapping=client_attribute_mapping).CommModel()

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
                                                     num_hosts=cfg.NUMBER_OF_AGENTS)).A()
    for agent in agents.values():
        if agent.eid in AGENTS_WITH_PV_PLANT:
            pv_sim = world.start('CSV', sim_start=cfg.START, datafile=cfg.PV_DATA,
                                 delimiter=',')
            pv = pv_sim.PV.create(1)[0]
            world.connect(pv, agents[agent.eid], 'P', weak=True)
            world.connect(agents[agent.eid], pv, 'ACK')

            world.set_initial_event(pv.sid)
        else:
            world.set_initial_event(agent.sid, time=0)


    if not TIME_BASED and len(cfg.INFRASTRUCTURE_CHANGES) > 0:
        world.set_initial_event(ict_controller.sid)

    if cfg.PARALLEL:
        world.set_initial_event(agents['client1'].sid, time=cfg.OFFSET)

    for agent_name, agent in agents.items():
        # connect agents with comm_models
        if TIME_BASED:
            world.connect(agent, comm_sim, f'message')
        else:
            world.connect(agent, comm_sim, f'message', weak=True)

    for client_name, attr in client_attribute_mapping.items():
        # connect comm_models with agents
        if TIME_BASED:
            world.connect(comm_sim, agents[client_name],
                          attr, time_shifted=True,
                          initial_data={attr: []})
        else:
            world.connect(comm_sim, agents[client_name],
                          attr)
        world.connect(comm_sim, collector,
                      attr)
    if len(cfg.INFRASTRUCTURE_CHANGES) > 0:
        # connect ict controller to collector
        world.connect(ict_controller, collector, f'ict_message')
        # connect ict controller with comm_sim
        if TIME_BASED:
            world.connect(ict_controller, comm_sim, f'ict_message', time_shifted=True, initial_data={'message': []})
            world.connect(comm_sim, ict_controller, f'ctrl_message')
        else:
            world.connect(ict_controller, comm_sim, f'ict_message')
            world.connect(comm_sim, ict_controller, f'ctrl_message', weak=True)

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
