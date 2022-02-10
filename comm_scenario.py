from mosaik import scenario

import config as cfg
from config import AGENTS_WITH_PV_PLANT
from util_functions import start_omnet, check_omnet_connection, stop_omnet, \
    get_agent_names, log


def main(num_agents=None, omnet_network=None, parallel=False, sim_end=None):
    if num_agents:
        cfg.NUMBER_OF_AGENTS = num_agents
    if omnet_network:
        cfg.START_MODE = "cmd"
        cfg.NETWORK = omnet_network
    if parallel:
        cfg.PARALLEL = parallel
    if sim_end:
        cfg.SIMULATION_END = sim_end

    sim_config = {
        'Collector': {'python': 'simulators.collector:Collector'},
        'CommSim': {'python': 'simulators.comm_simulator:CommSimulator'},
        'Agent': {'python': 'simulators.agent_simulator:Agent'},
        'ICTController': {
            'python': 'simulators.ict_controller_simulator:ICTController'},
        'CSV': {'python': 'simulators.mosaik_csv:CSV'}

    }

    omnet_process = start_omnet(cfg.START_MODE, cfg.NETWORK)
    check_omnet_connection(cfg.PORT)

    world = scenario.World(sim_config, debug=True, time_resolution=0.001,
                           cache=False)

    agents = {}
    collector = world.start('Collector').Monitor()
    agent_prefix = 'client'
    comm_sim = world.start('CommSim',
                           step_size=cfg.USED_STEP_SIZE,
                           port=cfg.PORT,
                           agent_names=get_agent_names(cfg.NUMBER_OF_AGENTS)).CommModel()

    ict_controller = world.start('ICTController',
                                 infrastructure_changes=cfg.
                                 INFRASTRUCTURE_CHANGES).ICT()

    for idx in range(cfg.NUMBER_OF_AGENTS):
        current_agent_name = agent_prefix + str(idx)
        agents[current_agent_name] = world.start('Agent',
                                                 step_type='event-based',
                                                 agent_name=current_agent_name,
                                                 content_path=cfg.CONTENT_PATH,
                                                 step_size=cfg.USED_STEP_SIZE,
                                                 agent_names=
                                                 get_agent_names(cfg.NUMBER_OF_AGENTS)).A()

    if AGENTS_WITH_PV_PLANT:
        pv_sim = world.start('CSV', sim_start=cfg.START, datafile=cfg.PV_DATA,
                             delimiter=',')
        pvs = pv_sim.PV.create(len(AGENTS_WITH_PV_PLANT))

    for agent in agents.values():
        world.set_initial_event(agent.sid, time=0)
    world.set_initial_event(ict_controller.sid)

    if cfg.PARALLEL:
        world.set_initial_event(agents['client1'].sid, time=1)

    for agent_name, agent in agents.items():
        # connect agents with comm_models
        world.connect(agent, comm_sim, f'message', weak=True)

    for agent_name, agent in agents.items():
        # connect comm_models with agents
        world.connect(comm_sim, agents[agent_name],
                      f'{cfg.CONNECT_ATTR}{agent_name}')
        world.connect(comm_sim, collector,
                      f'{cfg.CONNECT_ATTR}{agent_name}')

    # connect ict controller to collector
    world.connect(ict_controller, collector, f'message')
    # connect ict controller with comm_sim
    world.connect(ict_controller, comm_sim, f'message')
    world.connect(comm_sim, ict_controller, f'ctrl_message', weak=True)
    for counter, agent_name in enumerate(AGENTS_WITH_PV_PLANT):
        world.connect(agents[agent_name], pvs[counter], 'ACK', weak=True)
        world.connect(pvs[counter], agents[agent_name], 'P')
        world.set_initial_event(pvs[counter].sid)

    until = cfg.SIMULATION_END * cfg.USED_STEP_SIZE
    log(f'run until {until}')
    world.run(until=until)
    log("end of process")
    stop_omnet(omnet_process)


if __name__ == '__main__':
    main()
