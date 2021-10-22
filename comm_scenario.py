import json

from mosaik import scenario

from config import STEP_SIZE_1_MS, NUMBER_OF_AGENTS, CONTENT_PATH, CONFIG_FILE, \
    PARALLEL, START_MODE, NETWORK, CONNECT_ATTR
from util_functions import start_omnet, check_omnet_connection

sim_config = {
    'Collector': {'python': 'simulators.collector:Collector'},
    'CommSim': {'python': 'simulators.comm_simulator:CommSimulator'},
    'Agent': {'python': 'simulators.agent_simulator:Agent'}
}

with open(CONFIG_FILE, "r") as jsonfile:
    config = json.load(jsonfile)

start_omnet(START_MODE, NETWORK)
check_omnet_connection(config)

world = scenario.World(sim_config, debug=True)

step_size = STEP_SIZE_1_MS

agents = {}
comm_models = {}
collector = world.start('Collector').Monitor()
agent_prefix = 'client'
comm_sim = world.start('CommSim',
                       config_file=CONFIG_FILE,
                       step_size=step_size).CommModel()

for idx in range(NUMBER_OF_AGENTS):
    current_agent_name = agent_prefix + str(idx)
    agents[current_agent_name] = world.start('Agent', step_type='event-based',
                                             config_file=CONFIG_FILE,
                                             agent_name=current_agent_name,
                                             content_path=CONTENT_PATH,
                                             step_size=step_size).A()

world.set_initial_event(agents['client0'].sid)
if PARALLEL:
    world.set_initial_event(agents['client1'].sid, time=1)

for agent_name, agent in agents.items():
    # connect agents with comm_models
    world.connect(agent, comm_sim, f'message', weak=True)

for agent_name, agent in agents.items():
    # connect according to config
    for entry in config['connections']:
        src, dest = entry.split('->')
        if src == agent_name:
            # connect comm_models with agents
            world.connect(comm_sim, agents[dest],
                          f'{CONNECT_ATTR}{dest}')
            world.connect(comm_sim, collector,
                          f'{CONNECT_ATTR}{dest}')

world.run(until=1000000 * step_size)
