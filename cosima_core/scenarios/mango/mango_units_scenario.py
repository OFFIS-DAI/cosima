from copy import deepcopy

from mango.messages.codecs import JSON

import cosima_core.util.general_config as cfg
from cosima_core.messages.mango_messages import SimulatorMessage
from cosima_core.simulators.mango_example.unit_role import UnitRole
from cosima_core.util.scenario_setup_util import ScenarioHelper
from cosima_core.util.util_functions import get_host_names
from scenario_config import NUMBER_OF_AGENTS
from cosima_core.util.general_config import PV_DATA, START

codec = JSON()
codec.add_serializer(*SimulatorMessage.__serializer__())


def main():
    # Sim config. and other parameters
    sim_config = {
        'ContainerSim': {
            'python': 'cosima_core.simulators.mango_example.container_sim:ContainerSimulator',
        },
        'CSV': {
            'python': 'cosima_core.simulators.tic_toc_example.mosaik_csv:CSV'
        },
    }
    scenario_helper = ScenarioHelper()
    world, communication_simulator, client_attribute_mapping, sim_config = \
        scenario_helper.prepare_scenario(sim_config=sim_config)

    # generate container simulators
    agent_models = {}
    client_names = get_host_names(num_hosts=NUMBER_OF_AGENTS)

    client_agent_mapping = {}
    for idx in range(NUMBER_OF_AGENTS):
        client_agent_mapping['client' + str(idx)] = 'unitAgent' + str(idx)

    port = 5876
    # Instantiate container sim model
    for idx in range(NUMBER_OF_AGENTS):
        current_container_name = 'client' + str(idx)

        neighbors = deepcopy(client_agent_mapping)
        del neighbors[current_container_name]
        neighbors = [(key, value) for key, value in neighbors.items()]

        # Start simulators
        agent_roles = [UnitRole(neighbors=neighbors)]

        agent_models[current_container_name] = \
            world.start('ContainerSim', client_name=current_container_name, port=port, agent_roles=agent_roles,
                        client_names=client_names, client_agent_mapping=client_agent_mapping,
                        conversion_factor=cfg.MANGO_CONVERSION_FACTOR, connect_attributes=['P', 'p_set_point'],
                        codec=codec).ContainerModel()
        port += 1

    pv_sim = world.start('CSV', sim_start=START, datafile=PV_DATA,
                         delimiter=',', mosaik_attrs=['p_set_point', 'P'])
    pv_models = pv_sim.PV.create(len(agent_models))

    for idx, agent in enumerate(agent_models.values()):
        world.connect(pv_models[idx], agent_models[agent.eid], 'P', time_shifted=True, initial_data={'P': 0})
        world.connect(agent_models[agent.eid], pv_models[idx], 'p_set_point')
        world.set_initial_event(pv_models[idx].sid)

    # Connect entities
    for name in agent_models:
        world.connect(agent_models[name], communication_simulator, f'message', weak=True)
        world.connect(communication_simulator, agent_models[name], client_attribute_mapping[name])

    # set initial event
    world.set_initial_event(agent_models['client0'].sid, time=1)

    scenario_helper.run_simulation()
    scenario_helper.shutdown_simulation()


if __name__ == '__main__':
    main()
