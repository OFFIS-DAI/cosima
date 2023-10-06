import asyncio
import time

from mango import create_container
from mango import RoleAgent
import mango.messages.codecs
from mango_library.negotiation.cohda.cohda_negotiation import (
    COHDANegotiationRole,
    CohdaNegotiationModel,
)
from mango_library.negotiation.cohda.cohda_solution_aggregation import (
    CohdaSolutionAggregationRole,
)
from mango_library.negotiation.cohda.cohda_starting import (
    CohdaNegotiationDirectStarterRole,
)
from mango_library.negotiation.termination import (
    NegotiationTerminationParticipantRole,
    NegotiationTerminationDetectorRole,
)
from mango_library.coalition.core import (
    CoalitionParticipantRole,
    CoalitionInitiatorRole,
)
import mango_library.negotiation.util as util

from cosima_core.mango_direct_connection.mango_communication_network import MangoCommunicationNetwork
from cosima_core.util.general_config import PORT
from cosima_core.util.util_functions import stop_omnet

NUM_AGENTS = 41


async def run_scenario():
    start = time.time()
    # create containers
    codec = mango.messages.codecs.JSON()
    for serializer in util.cohda_serializers:
        codec.add_serializer(*serializer())

    controller_agent = None

    containers = list()
    cohda_agents = list()
    addrs = list()

    client_container_mapping = dict()

    s_array = [
        [
            [1, 1, 1, 1, 1],
            [4, 3, 3, 3, 3],
            [6, 6, 6, 6, 6],
            [9, 8, 8, 8, 8],
            [11, 11, 11, 11, 11],
        ]
    ]

    for c_i in range(NUM_AGENTS):
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
                                                            port=PORT)

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

    assert (
            len(asyncio.all_tasks()) == 1
    ), f"Too many Tasks are running{asyncio.all_tasks()}"
    cohda_negotiation = \
        list(cohda_agents[1]._role_context.get_or_create_model(CohdaNegotiationModel)._negotiations.values())[0]
    cluster_schedule = cohda_negotiation._memory.solution_candidate.cluster_schedule
    print('cluster schedule: ', cluster_schedule)

    print('time: ', time.time() - start)

    mango_communication_network.omnetpp_connection.close_connection()
    stop_omnet(mango_communication_network.omnet_process)


async def wait_for_solution_confirmed(aggregation_role):
    while len(aggregation_role._confirmed_cohda_solutions) == 0:
        await asyncio.sleep(0.05)


def main():
    asyncio.run(run_scenario())


if __name__ == '__main__':
    main()
