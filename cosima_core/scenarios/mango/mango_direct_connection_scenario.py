import asyncio

from mango.messages.codecs import JSON
from mango import RoleAgent, create_container

from cosima_core.mango_direct_connection.mango_communication_network import MangoCommunicationNetwork
from cosima_core.messages.mango_messages import AlphabetMessage
from cosima_core.simulators.tutorial.simple_roles import ActiveRole, PassiveRole
from cosima_core.util.general_config import PORT
from cosima_core.util.util_functions import stop_omnet


async def main():
    codec = JSON()
    codec.add_serializer(*AlphabetMessage.__serializer__())

    active_container = await create_container(addr='client0', codec=codec, connection_type='mosaik')
    reply_container = await create_container(addr='client1', codec=codec, connection_type='mosaik')

    active_agent = RoleAgent(active_container, suggested_aid='activeAgent')
    reply_agent = RoleAgent(reply_container, suggested_aid='replyAgent')

    client_container_mapping = {'client0': active_container, 'client1': reply_container}

    active_agent.add_role(ActiveRole(neighbors=[('client1', reply_agent.aid)]))
    reply_agent.add_role(PassiveRole(neighbors=[('client0', active_agent.aid)]))

    mango_communication_network = MangoCommunicationNetwork(client_container_mapping=client_container_mapping,
                                                            port=PORT)

    # no more run call since everything now happens automatically within the roles
    await asyncio.sleep(20)

    # always properly shut down your containers
    await active_container.shutdown()
    await reply_container.shutdown()

    mango_communication_network.omnetpp_connection.close_connection()
    stop_omnet(mango_communication_network.omnet_process)


if __name__ == "__main__":
    asyncio.run(main())
