import time
import asyncio
import os

from mango.messages.codecs import JSON
from mango_library.negotiation.util import cohda_serializers
from mango_library.coalition.core import CoalitionParticipantRole, CoalitionInitiatorRole
from mango_library.negotiation.cohda.cohda_negotiation import COHDANegotiationRole, CohdaNegotiationModel
from mango_library.negotiation.cohda.cohda_solution_aggregation import CohdaSolutionAggregationRole
from mango_library.negotiation.cohda.cohda_starting import CohdaNegotiationDirectStarterRole
from mango_library.negotiation.termination import NegotiationTerminationDetectorRole, \
    NegotiationTerminationParticipantRole
from mango import create_container, RoleAgent

import cosima_core.util.general_config as cfg
from cosima_core.mango_direct_connection.mango_communication_network import MangoCommunicationNetwork
from cosima_core.util.scenario_setup_util import ScenarioHelper
from cosima_core.util.util_functions import get_host_names, stop_omnet
import scenario_config

codec = JSON()
for serializer in cohda_serializers:
    codec.add_serializer(*serializer())

# target 20 agents Intraday:
# [3200, 3200, 3200, 3200, 4000], same weight
# Target 20 agents DayAhead:
# [4000, 4000, 4000, 4000, 4000], same weight
target = ([110, 110, 110, 110, 110], [1, 1, 1, 1, 1, ],)

s_array = [
    [
        [1, 1, 1, 1, 1],
        [4, 3, 3, 3, 3],
        [6, 6, 6, 6, 6],
        [9, 8, 8, 8, 8],
        [11, 11, 11, 11, 11],
    ]
]

# adapt maximal byte size per msg group, since message get bigger if more agents are involved
cfg.MAX_BYTE_SIZE_PER_MSG_GROUP = 30000


async def run_scenario_without_mosaik():
    controller_agent = None

    containers = list()
    cohda_agents = list()
    addrs = list()

    client_container_mapping = dict()

    for c_i in range(scenario_config.NUMBER_OF_AGENTS):
        current_container = await create_container(addr=f"client{c_i}", codec=codec, connection_type='mosaik')

        if c_i == 0:
            # is controller agent
            controller_agent = RoleAgent(current_container)
            controller_agent.add_role(NegotiationTerminationDetectorRole())
            aggregation_role = CohdaSolutionAggregationRole()
            controller_agent.add_role(aggregation_role)
        else:
            a = RoleAgent(current_container)
            addrs.append((current_container.addr, a.aid))
            cohda_role = COHDANegotiationRole(lambda: s_array[0], lambda s: True)
            a.add_role(cohda_role)
            a.add_role(CoalitionParticipantRole())
            a.add_role(NegotiationTerminationParticipantRole())
            if c_i == 1:
                a.add_role(
                    CohdaNegotiationDirectStarterRole(
                        (
                            [110, 110, 110, 110, 110],
                            [1, 1, 1, 1, 1, ],
                        )
                    )
                )
            cohda_agents.append(a)

        containers.append(current_container)
        client_container_mapping[f'client{c_i}'] = current_container

    mango_communication_network = MangoCommunicationNetwork(client_container_mapping=client_container_mapping,
                                                            port=cfg.PORT)

    controller_agent.add_role(
        CoalitionInitiatorRole(addrs, "cohda", "cohda-negotiation")
    )

    for a in cohda_agents + [controller_agent]:
        if a._check_inbox_task.done():
            if a._check_inbox_task.exception() is not None:
                raise a._check_inbox_task.exception()
            else:
                assert False, f"check_inbox terminated unexpectedly."

    await asyncio.wait_for(wait_for_solution_confirmed(aggregation_role), timeout=10000000000000)

    # gracefully shutdown
    for a in cohda_agents + [controller_agent]:
        await a.shutdown()
    for container in containers:
        await container.shutdown()

    mango_communication_network.omnetpp_connection.close_connection()
    stop_omnet(mango_communication_network.omnet_process)


async def wait_for_solution_confirmed(aggregation_role):
    while len(aggregation_role._confirmed_cohda_solutions) == 0:
        await asyncio.sleep(0.05)


def run_scenario_with_mosaik():
    # Simulation configuration -> tells mosaik where to find the simulators
    sim_config = {
        'ContainerSim': {
            'python': 'cosima_core.simulators.mango_example.container_sim:ContainerSimulator',
        },
    }

    scenario_helper = ScenarioHelper()
    world, communication_simulator, client_attribute_mapping, sim_config = \
        scenario_helper.prepare_scenario(sim_config=sim_config)

    # generate container simulators
    agent_models = {}
    client_names = get_host_names(num_hosts=scenario_config.NUMBER_OF_AGENTS)

    client_agent_mapping = {}
    for idx in range(scenario_config.NUMBER_OF_AGENTS):
        client_agent_mapping['client' + str(idx)] = 'cohdaAgent' + str(idx)

    port = 5876
    # Instantiate container sim model
    for idx in range(scenario_config.NUMBER_OF_AGENTS):
        current_container_name = 'client' + str(idx)

        # Start simulators

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

            cohda_roles.append(COHDANegotiationRole(lambda: s_array[0], lambda s: True))
            cohda_roles.append(CoalitionParticipantRole())

        agent_models[current_container_name] = \
            world.start('ContainerSim', client_name=current_container_name, port=port, agent_roles=cohda_roles,
                        client_names=client_names, client_agent_mapping=client_agent_mapping,
                        conversion_factor=cfg.MANGO_CONVERSION_FACTOR,
                        codec=codec).ContainerModel()
        port += 1

    # Connect entities
    for name in agent_models:
        world.connect(agent_models[name], communication_simulator, f'message', weak=True)
        world.connect(communication_simulator, agent_models[name], client_attribute_mapping[name])

    # set initial event
    world.set_initial_event(agent_models['client0'].sid, time=0)

    scenario_helper.run_simulation()
    scenario_helper.shutdown_simulation()


if __name__ == '__main__':
    runtimes_with_mosaik = list()
    runtimes_without_mosaik = list()

    scenario_config.NUMBER_OF_AGENTS = 5
    scenario_config.LOGGING_LEVEL = 'warnings'

    def kill():
        os.system('killall -9 cosima_omnetpp_project')
        os.system('fuser -k 4243/tcp')
        time.sleep(5)

    for _ in range(5):
        kill()

        start = time.time()
        try:
            run_scenario_with_mosaik()
            runtimes_with_mosaik.append(time.time() - start)
        except Exception as e:
            print('Exception: ', e)

        kill()

        start = time.time()
        asyncio.run(run_scenario_without_mosaik())
        runtimes_without_mosaik.append(time.time() - start)

    print(f'Execution took {runtimes_with_mosaik} seconds with mosaik '
          f'and {runtimes_without_mosaik} seconds without mosaik.')
