from time import sleep

from mosaik import scenario

import scenario_config
from cosima_core.util.general_config import PORT, CONNECT_ATTR
from cosima_core.util.util_functions import check_omnet_connection, start_omnet, set_up_file_logging, get_host_names, \
    stop_omnet, log


class ScenarioHelper:
    def __init__(self):
        self._tracker = None
        self._omnet_process = None
        self._world = None

    def prepare_scenario(self, sim_config):
        sim_config['CommunicationSimulator'] = {
            'python': 'cosima_core.simulators.communication_simulator:CommunicationSimulator'}
        if scenario_config.USE_COMMUNICATION_SIMULATION:
            self._omnet_process = start_omnet(scenario_config.START_MODE, scenario_config.NETWORK)
            check_omnet_connection(PORT)
        if scenario_config.TRACK_PERFORMANCE:
            from cosima_core.util.track_performance import PerformanceTracker
            self._tracker = PerformanceTracker()
        set_up_file_logging()

        # create mosaik world
        self._world = scenario.World(sim_config, time_resolution=0.001,
                                     cache=False)

        agent_names = get_host_names(num_hosts=scenario_config.NUMBER_OF_AGENTS)
        client_attribute_mapping = {}
        # for each client in OMNeT++, save the connect-attribute for mosaik
        for agent_name in agent_names:
            client_attribute_mapping[agent_name] = CONNECT_ATTR + agent_name

        communication_simulator = \
            self._world.start('CommunicationSimulator',
                              port=PORT,
                              client_attribute_mapping=client_attribute_mapping,
                              use_communication_simulation=scenario_config.USE_COMMUNICATION_SIMULATION) \
                .CommunicationModel()

        return self._world, communication_simulator, client_attribute_mapping, sim_config

    def run_simulation(self):
        until = scenario_config.SIMULATION_END
        log(f'run until {until}')
        self._world.run(until=until)

    def shutdown_simulation(self):
        sleep(5)
        if scenario_config.USE_COMMUNICATION_SIMULATION:
            stop_omnet(self._omnet_process)
        if scenario_config.TRACK_PERFORMANCE:
            self._tracker.save_results()
