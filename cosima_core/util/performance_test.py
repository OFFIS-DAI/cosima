import math
import multiprocessing
import os
import time
import pandas as pd
import warnings
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

import cosima_core.scenarios.communication_scenario as communication_scenario
import scenario_config

warnings.filterwarnings("ignore", category=FutureWarning)
path = '../results/performance_results.csv'


def execute_performance_test():
    # TODO: complex messages, mango

    df = pd.DataFrame(columns=['scenario_identifier', 'elapsed_time'])

    traffic_configuration = [{
        'source': f'traffic_device_{n_traffic_device}',
        'destination': 'client0',
        'start': start,
        'stop': 250,
        'interval': 10,
        'packet_length': 1000
    } for start in [5] for n_traffic_device in range(2)]

    configurations = [
        {
            'scenario_identifier': 'standard'
        },
        {
            'scenario_identifier': 'increased number of messages',
            'PARALLEL': True,
            'OFFSET': 1
        },
        {
            'scenario_identifier': 'medium number of agents',
            'NUMBER_OF_AGENTS': 40,
        },
        {
            'scenario_identifier': 'high number of agents',
            'NUMBER_OF_AGENTS': 100,
        },
        {
            'scenario_identifier': 'increased calculating times',
            'CALCULATING_TIMES': 10,
        },
        {
            'scenario_identifier': 'increased message sizes',
            'CONTENT_LENGTH': 2000,
        },
        {
            'scenario_identifier': 'strongly increased message sizes',
            'CONTENT_LENGTH': 5000,
        },
        {
            'scenario_identifier': 'without communication simulation',
            'USE_COMMUNICATION_SIMULATION': False,
        },
        {
            'scenario_identifier': 'logging info',
            'LOGGING_LEVEL': 'info',
        },
        {
            'scenario_identifier': 'logging debug',
            'LOGGING_LEVEL': 'debug',
        },
        {
            'scenario_identifier': 'file logging',
            'LOG_TO_FILE': True,
        },
        {
            'scenario_identifier': 'performance tracking',
            'TRACK_PERFORMANCE': True,
        },
        {
            'scenario_identifier': 'lte network',
            'NETWORK': 'LargeLTENetwork',
        },
        {
            'scenario_identifier': 'multiple simulators',
            'AGENTS_WITH_PV_PLANT': [f'client{i}' for i in range(5)],
            'AGENTS_WITH_HOUSEHOLD': [f'client{i}' for i in range(5)],
        },
        {
            'scenario_identifier': 'attack',
            'TRAFFIC_CONFIGURATION': {'attacked_module': 'client1',
                                      'type': 'PacketDelay',
                                      'start': 1,
                                      'stop': 100,
                                      'probability': 0.5},
        },
        {
            'scenario_identifier': 'traffic',
            'TRAFFIC_CONFIGURATION': traffic_configuration,
            'NETWORK': 'TrafficNetwork'
        },
        {
            'scenario_identifier': 'dis-and reconnect',
            'INFRASTRUCTURE_CHANGES': [
                {'type': 'Disconnect',
                 'time': 2,
                 'module': 'client1'},
                {'type': 'Reconnect',
                 'time': 5,
                 'module': 'client1'},
            ]
        }
    ]

    for scenario_i in range(10):
        for config in configurations:
            # Standard configuration
            if 'NUMBER_OF_AGENTS' in config:
                scenario_config.NUMBER_OF_AGENTS = config['NUMBER_OF_AGENTS']
            else:
                scenario_config.NUMBER_OF_AGENTS = 5
            if 'USE_COMMUNICATION_SIMULATION' in config:
                scenario_config.USE_COMMUNICATION_SIMULATION = config['USE_COMMUNICATION_SIMULATION']
            else:
                scenario_config.USE_COMMUNICATION_SIMULATION = True
            if 'NETWORK' in config:
                scenario_config.NETWORK = config['NETWORK']
            else:
                scenario_config.NETWORK = 'StarTopologyNetwork'
            if 'SIMULATION_END' in config:
                scenario_config.SIMULATION_END = config['SIMULATION_END']
            else:
                scenario_config.SIMULATION_END = 500
            if 'LOGGING_LEVEL' in config:
                scenario_config.LOGGING_LEVEL = config['LOGGING_LEVEL']
            else:
                scenario_config.LOGGING_LEVEL = 'warning'
            if 'LOG_TO_FILE' in config:
                scenario_config.LOG_TO_FILE = config['LOG_TO_FILE']
            else:
                scenario_config.LOG_TO_FILE = False
            if 'TRACK_PERFORMANCE' in config:
                scenario_config.TRACK_PERFORMANCE = config['TRACK_PERFORMANCE']
            else:
                scenario_config.TRACK_PERFORMANCE = False
            if 'INFRASTRUCTURE_CHANGES' in config:
                scenario_config.INFRASTRUCTURE_CHANGES = config['INFRASTRUCTURE_CHANGES']
            else:
                scenario_config.INFRASTRUCTURE_CHANGES = []
            if 'ATTACK_CONFIGURATION' in config:
                scenario_config.ATTACK_CONFIGURATION = config['ATTACK_CONFIGURATION']
            else:
                scenario_config.ATTACK_CONFIGURATION = []
            if 'TRAFFIC_CONFIGURATION' in config:
                scenario_config.TRAFFIC_CONFIGURATION = config['TRAFFIC_CONFIGURATION']
            else:
                scenario_config.TRAFFIC_CONFIGURATION = []

            if 'PARALLEL' in config and 'OFFSET' in config:
                communication_scenario.PARALLEL = config['PARALLEL']
                communication_scenario.OFFSET = config['OFFSET']
            else:
                communication_scenario.PARALLEL = False
            communication_scenario.AGENTS_WITH_PV_PLANT = []
            communication_scenario.AGENTS_WITH_HOUSEHOLDS = []
            communication_scenario.CONTENT_LENGTH = None
            communication_scenario.CALCULATING_TIMES = 0

            start = time.time()
            p = multiprocessing.Process(target=communication_scenario.main)
            p.start()
            p.join(1500)
            elapsed_time = time.time() - start
            # If thread is still active
            if p.is_alive():
                p.terminate()
                p.join()
                os.system('killall -9 cosima_omnetpp_project')
                os.system('fuser -k 4243/tcp')
                elapsed_time = math.inf

            # Insert Dict to the dataframe using DataFrame.append()
            new_row = {'scenario_identifier': config['scenario_identifier'], 'elapsed_time': elapsed_time}
            df = df.append(new_row, ignore_index=True)

    print(df)
    df.to_csv(path)


def analyse_performance_results():
    df = pd.read_csv(path)

    # Filter out infinite values
    df = df.replace([np.inf, -np.inf], np.nan)
    df = df.dropna(subset=['elapsed_time'])

    # Group the data by scenario identifier
    # grouped_data = df.groupby('scenario_identifier')['elapsed_time'].apply(list)

    # Create a box plot
    plt.figure(figsize=(10, 6))
    sns.set(font_scale=1.5)
    sns.barplot(data=df, y='elapsed_time', x='scenario_identifier')
    # plt.boxplot(list(grouped_data))
    plt.ylabel('Elapsed Time')
    plt.xlabel('')

    # Set x-axis tick labels
    plt.xticks(rotation=90)

    # Display the plot
    plt.tight_layout()
    plt.show()


if __name__ == '__main__':
    # execute_performance_test()
    analyse_performance_results()
