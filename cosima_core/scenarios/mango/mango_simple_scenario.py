from copy import deepcopy

from cosima_core.messages.mango_messages import AlphabetMessage
from cosima_core.simulators.tutorial.simple_roles import ActiveRole, PassiveRole
from cosima_core.util.scenario_setup_util import ScenarioHelper
from cosima_core.util.util_functions import get_host_names
from mango.messages.codecs import JSON
from mango_library.negotiation.util import cohda_serializers

# Sim config. and other parameters
import scenario_config
import cosima_core.util.general_config as cfg

codec = JSON()
for serializer in cohda_serializers:
    codec.add_serializer(*serializer())
codec.add_serializer(*AlphabetMessage.__serializer__())


def main():
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
    client_agent_mapping = {
        'client0': 'activeAgent',
        'client1': 'replyAgent'
    }

    port = 5876
    # Instantiate container sim model
    for idx in range(scenario_config.NUMBER_OF_AGENTS):
        current_container_name = 'client' + str(idx)

        agent_roles = list()
        neighbors = deepcopy(client_agent_mapping)
        del neighbors[current_container_name]
        neighbors = [(key, value) for key, value in neighbors.items()]

        # Start simulators
        if idx == 0:
            agent_roles.append(ActiveRole(neighbors))
        else:
            agent_roles.append(PassiveRole(neighbors))
        agent_models[current_container_name] = \
            world.start('ContainerSim', client_name=current_container_name, port=port, agent_roles=agent_roles,
                        client_names=client_names, client_agent_mapping=client_agent_mapping,
                        conversion_factor=cfg.MANGO_CONVERSION_FACTOR, codec=codec).ContainerModel()
        port += 1

    if scenario_config.USE_COMMUNICATION_SIMULATION:
        # Connect entities
        for name in agent_models:
            world.connect(agent_models[name], communication_simulator, f'message', weak=True)
            world.connect(communication_simulator, agent_models[name], client_attribute_mapping[name])
    else:
        world.connect(agent_models['client0'], agent_models['client1'], f'message', weak=True)
        world.connect(agent_models['client1'], agent_models['client0'], f'message')

    # set initial event
    world.set_initial_event(agent_models['client0'].sid, time=0)

    scenario_helper.run_simulation()
    scenario_helper.shutdown_simulation()


if __name__ == '__main__':
    main()
