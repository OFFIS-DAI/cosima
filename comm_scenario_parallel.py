import json

from mosaik import scenario

from util import STEP_SIZE_1_MS, NUMBER_OF_AGENTS, CONFIG_FILE

sim_config = {
    'Collector': {'python': 'simulators.collector:Collector'},
    'CommSim': {'python': 'simulators.comm_simulator:CommSimulator'},
    'Agent': {'python': 'simulators.agent_simulator_parallel:Agent'}
}

world = scenario.World(sim_config, debug=True)

with open(CONFIG_FILE, "r") as jsonfile:
    config = json.load(jsonfile)

agents = {}
comm_models = {}
collector = world.start('Collector').Monitor()
agent_prefix = 'client'
comm_sim = world.start('CommSim',
                       config_file=CONFIG_FILE)

for idx in range(NUMBER_OF_AGENTS):
    current_agent_name = agent_prefix + str(idx)
    agents[current_agent_name] = world.start('Agent', step_type='event-based',
                                             config_file=CONFIG_FILE,
                                             agent_name=current_agent_name,
                                             step_size=STEP_SIZE_1_MS).A()

    comm_models[current_agent_name] = comm_sim.CommModel()

world.set_initial_event(agents['client0'].sid)
world.set_initial_event(agents['client1'].sid, time=1)

for agent_name, agent in agents.items():
    world.connect(agent, comm_models[agent_name], 'message', weak=True)

for agent_name, agent in agents.items():
    # connect according to config
    for entry in config['connections']:
        src, dest = entry.split('->')
        if src == agent_name:
            world.connect(comm_models[src], agents[dest], 'message_with_delay')
            world.connect(comm_models[src], collector,
                          'message_with_delay')

world.run(until=1000000 * STEP_SIZE_1_MS)

# plot_execution_graph(world)
