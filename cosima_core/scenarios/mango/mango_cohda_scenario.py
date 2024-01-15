from copy import deepcopy
from os import listdir
import pandas as pd

from mango.messages.codecs import JSON
from mango_library.negotiation.util import cohda_serializers
from mango_library.coalition.core import CoalitionParticipantRole, CoalitionInitiatorRole
from mango_library.negotiation.cohda.cohda_negotiation import COHDANegotiationRole
from mango_library.negotiation.cohda.cohda_solution_aggregation import CohdaSolutionAggregationRole
from mango_library.negotiation.cohda.cohda_starting import CohdaNegotiationDirectStarterRole
from mango_library.negotiation.termination import NegotiationTerminationDetectorRole, \
    NegotiationTerminationParticipantRole

import cosima_core.util.general_config as cfg
from cosima_core.util.scenario_setup_util import ScenarioHelper
from cosima_core.util.util_functions import get_host_names
from scenario_config import NUMBER_OF_AGENTS, TRAFFIC_CONFIGURATION, USE_COMMUNICATION_SIMULATION

codec = JSON()
for serializer in cohda_serializers:
    codec.add_serializer(*serializer())

# target 20 agents Intraday:
# [3200, 3200, 3200, 3200, 4000], same weight
# Target 20 agents DayAhead:
# [4000, 4000, 4000, 4000, 4000], same weight
target = ([3200 * NUMBER_OF_AGENTS/20 for _ in range(4)], [1, 1, 1, 1])


def main():
    # Simulation configuration -> tells mosaik where to find the simulators
    sim_config = {
        'ContainerSim': {
            'python': 'cosima_core.simulators.mango_example.container_sim:ContainerSimulator',
        },
        'ICTController': {
            'python': 'cosima_core.simulators.ict_controller_simulator:ICTController'},
    }

    scenario_helper = ScenarioHelper()
    world, communication_simulator, client_attribute_mapping, sim_config = \
        scenario_helper.prepare_scenario(sim_config=sim_config)

    # adapt maximal byte size per msg group, since message get bigger if more agents are involved
    # TODO: change for other number of agents
    cfg.MAX_BYTE_SIZE_PER_MSG_GROUP = 30000

    use_ict_simulator = (len(TRAFFIC_CONFIGURATION) > 0 and USE_COMMUNICATION_SIMULATION)

    if use_ict_simulator:
        ict_controller = world.start('ICTController',
                                     infrastructure_changes=[],
                                     traffic_configuration=TRAFFIC_CONFIGURATION).ICT()


    filenames = listdir(cfg.CHP_DATA)
    all_schedule_names = [filename for filename in filenames if filename.startswith("CHP")]
    all_schedules = {}
    idx = 0

    for filename in all_schedule_names:
        df = pd.read_csv(cfg.CHP_DATA + '/' + filename, index_col=None, header=0)
        all_schedules[idx] = []

        for i in range(df.shape[1]):
            all_schedules[idx].append(df[str(i)].to_list())
        idx += 1
    prec = len(all_schedules)
    for i in range(prec, NUMBER_OF_AGENTS):
        all_schedules[i] = deepcopy(all_schedules[0])

    # generate container simulators
    agent_models = {}
    client_names = get_host_names(num_hosts=NUMBER_OF_AGENTS)

    client_agent_mapping = {}
    for idx in range(NUMBER_OF_AGENTS):
        client_agent_mapping['client' + str(idx)] = 'cohdaAgent' + str(idx)

    port = 5876
    # Instantiate container sim model
    for idx in range(NUMBER_OF_AGENTS):
        current_container_name = 'client' + str(idx)
        # Start simulators

        def schedule_provider(i):
            return deepcopy(lambda: all_schedules[i])

        cohda_roles = []
        if idx == 0:
            # is controller agent
            addrs = [(key, value) for key, value in client_agent_mapping.items() if key != 'client0']
            cohda_roles.append(CoalitionInitiatorRole(addrs, 'cohda', 'cohda-negotiation'))
            cohda_roles.append(NegotiationTerminationDetectorRole())
            cohda_roles.append(CohdaSolutionAggregationRole())
        elif idx == 1:
            # is negotiation starter
            cohda_roles.append(CohdaNegotiationDirectStarterRole(target_params=target))

        if idx != 0:
            # is normal participant
            cohda_roles.append(NegotiationTerminationParticipantRole())

            cohda_roles.append(COHDANegotiationRole(schedules_provider=schedule_provider(idx),
                                                    local_acceptable_func=lambda s: True))
            cohda_roles.append(CoalitionParticipantRole())

        agent_models[current_container_name] = \
            world.start('ContainerSim', client_name=current_container_name, port=port, agent_roles=cohda_roles,
                        client_names=client_names, client_agent_mapping=client_agent_mapping,
                        conversion_factor=cfg.MANGO_CONVERSION_FACTOR,
                        schedule_provider=schedule_provider(idx), is_controller_agent=False, is_starter=True,
                        target=target, codec=codec).ContainerModel()
        port += 1

    # Connect entities
    for name in agent_models:
        world.connect(agent_models[name], communication_simulator, f'message', weak=True)
        world.connect(communication_simulator, agent_models[name], client_attribute_mapping[name])

    if use_ict_simulator:
        # connect ict controller with communication_simulator
        world.connect(ict_controller, communication_simulator, f'ict_message', weak=True)
        world.connect(communication_simulator, ict_controller, f'ctrl_message')

    # set initial event
    world.set_initial_event(agent_models['client0'].sid, time=0)

    if use_ict_simulator:
        world.set_initial_event(ict_controller.sid)

    scenario_helper.run_simulation()
    scenario_helper.shutdown_simulation()


if __name__ == '__main__':
    main()
