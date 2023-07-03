import random
import string

import cosima_core.util.general_config as cfg
import scenario_config
from cosima_core.util.scenario_setup_util import ScenarioHelper
from cosima_core.util.util_functions import get_host_names
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


def main():
    # Simulation configuration -> tells mosaik where to find the simulators
    sim_config = {
        'Collector': {
            'python': 'cosima_core.simulators.collector:Collector'},
        'Agent': {
            'python': 'cosima_core.simulators.tic_toc_example.agent_simulator:Agent'},
        'ICTController': {
            'python': 'cosima_core.simulators.ict_controller_simulator:ICTController'},
        'CSV': {'python': 'cosima_core.simulators.tic_toc_example.mosaik_csv:CSV'},
        'Household': {'python': 'householdsim.mosaik:HouseholdSim'},
    }
    scenario_helper = ScenarioHelper()
    world, communication_simulator, client_attribute_mapping, sim_config = \
        scenario_helper.prepare_scenario(sim_config=sim_config)

    if CONTENT_LENGTH:
        message_content = ''.join(random.choices(string.ascii_lowercase + string.digits, k=CONTENT_LENGTH))
    else:
        message_content = None

    use_ict_simulator = (len(scenario_config.INFRASTRUCTURE_CHANGES) > 0
                         or len(scenario_config.TRAFFIC_CONFIGURATION) > 0
                         or len(scenario_config.ATTACK_CONFIGURATION) > 0) and \
                        scenario_config.USE_COMMUNICATION_SIMULATION

    agents = {}
    collector = world.start('Collector').Monitor()
    agent_prefix = 'client'

    if use_ict_simulator:
        ict_controller = world.start('ICTController',
                                     infrastructure_changes=scenario_config.INFRASTRUCTURE_CHANGES,
                                     traffic_configuration=scenario_config.TRAFFIC_CONFIGURATION).ICT()

    for idx in range(scenario_config.NUMBER_OF_AGENTS):
        current_agent_name = agent_prefix + str(idx)
        agents[current_agent_name] = world.start('Agent',
                                                 agent_name=current_agent_name,
                                                 content_path=CONTENT_PATH,
                                                 agent_names=
                                                 get_host_names(
                                                     num_hosts=scenario_config.NUMBER_OF_AGENTS),
                                                 parallel=PARALLEL,
                                                 offset=OFFSET,
                                                 message_content=message_content).A()

    if len(AGENTS_WITH_PV_PLANT) > 0:
        pv_sim = world.start('CSV', sim_start=cfg.START, datafile=cfg.PV_DATA,
                             delimiter=',', mosaik_attrs=['ACK', 'P'])
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

    if use_ict_simulator:
        world.set_initial_event(ict_controller.sid)

    if PARALLEL:
        world.set_initial_event(agents['client1'].sid, time=OFFSET)

    for agent_name, agent in agents.items():
        # connect agents with communication_models
        world.connect(agent, communication_simulator, f'message', weak=True)

    if use_ict_simulator:
        # connect ict controller to collector
        world.connect(ict_controller, collector, f'ict_message')
        # connect ict controller with communication_simulator
        world.connect(ict_controller, communication_simulator, f'ict_message', weak=True)
        world.connect(communication_simulator, ict_controller, f'ctrl_message')

    for client_name, attr in client_attribute_mapping.items():
        # connect communication_models with agents
        world.connect(communication_simulator, agents[client_name],
                      attr)
        world.connect(communication_simulator, collector,
                      attr)

    scenario_helper.run_simulation()
    scenario_helper.shutdown_simulation()


if __name__ == '__main__':
    main()
