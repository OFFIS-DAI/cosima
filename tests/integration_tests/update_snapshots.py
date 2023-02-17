import math
import os
import pandas as pd
import re
import time

import cosima_core.scenarios.communication_scenario as communication_scenario
import cosima_core.scenarios.negotiation_scenario as negotiation_scenario
import scenario_config
from cosima_core.util.util_functions import log

# change working directory because the main is called from the
# test folder now
cwd = os.path.abspath(os.path.dirname(__file__))
new_wd = os.path.abspath(cwd + "/../" + "/../cosima_core/scenarios")
os.chdir(new_wd)

PATH_TO_TIC_TOC_SCENARIO_TEST_CASES = '../../tests/integration_tests/tictoc_scenarios.csv'
PATH_TO_NEGOTIATION_SCENARIO_TEST_CASES = '../../tests/integration_tests/negotiation_scenarios.csv'


def get_snapshot():
    with open('../../tests/integration_tests/snapshot.txt') as f:
        expected_data = f.readline().split(" ")
        if len(expected_data) < 7:
            return
        snapshot = dict()
        snapshot['events'] = float(expected_data[0])
        snapshot['messages'] = float(expected_data[1])
        snapshot['errors'] = float(expected_data[2])
        snapshot['max_advance'] = float(expected_data[3])
        snapshot['disconnect'] = float(expected_data[4])
        snapshot['reconnect'] = float(expected_data[5])
        snapshot['last_event'] = float(expected_data[6])
        return snapshot



def update_snapshot(df, scenarioID, path):
    snapshot = get_snapshot()
    for index, row in df.iterrows():
        id = row["scenarioID"]
        if id is scenarioID:
            df.at[index, 'events'] = snapshot['events']
            df.at[index, 'messages'] = snapshot['messages']
            df.at[index, 'errors'] = snapshot['errors']
            df.at[index, 'maxAdvance'] = snapshot['max_advance']
            df.at[index, 'disconnect'] = snapshot['disconnect']
            df.at[index, 'reconnect'] = snapshot['reconnect']
            df.at[index, 'lastEvent'] = snapshot['last_event']
    df.to_csv(path, index=False)


def get_infrastructure_changes(row):
    changes = list()
    if (type(row["disconnects"]) == str and row["disconnects"] != "") \
            or (type(row["disconnects"]) == float and not math.isnan(row["disconnects"])):
        string_value = re.split(':|;', row["disconnects"])
        for index in range(int(len(string_value) / 2)):
            entry = dict()
            entry["type"] = "Disconnect"
            entry["time"] = int(string_value[2 * index])
            entry["module"] = string_value[2 * index + 1]
            changes.append(entry)
    if (type(row["reconnects"]) == str and row["reconnects"] != "") \
            or (type(row["reconnects"]) == float and not math.isnan(row["reconnects"])):
        string_value = re.split(':|;', row["reconnects"])
        for index in range(int(len(string_value) / 2)):
            entry = dict()
            entry["type"] = "Connect"
            entry["time"] = int(string_value[2 * index])
            entry["module"] = string_value[2 * index + 1]
            changes.append(entry)
    return changes


def update_negotiation_scenarios():
    df = pd.read_csv(PATH_TO_NEGOTIATION_SCENARIO_TEST_CASES)
    for index, row in df.iterrows():
        id = row["scenarioID"]
        number_of_agents = row["number of agents"]
        network = row["network"]
        simulation_end = row["until"]
        threshold = row["threshold"]
        check_inbox_interval = row["checkInboxInterval"]
        infrastructure_changes = get_infrastructure_changes(row)
        print(f'Simulate negotiation scenario {id} with {number_of_agents} agents, omnet network {network} '
              f'and until {simulation_end}')
        scenario_config.LOGGING_LEVEL = "info"
        negotiation_scenario.main(start_mode='cmd',
                                  number_of_agents=number_of_agents,
                                  network=network,
                                  simulation_end=simulation_end,
                                  threshold=threshold,
                                  check_inbox_interval=check_inbox_interval,
                                  infrastructure_changes=infrastructure_changes)
        time.sleep(5)
        update_snapshot(df, id, PATH_TO_NEGOTIATION_SCENARIO_TEST_CASES)
        print("\n \n")


def update_tic_toc_scenarios():
    df = pd.read_csv(PATH_TO_TIC_TOC_SCENARIO_TEST_CASES)

    for index, row in df.iterrows():
        id = row["scenarioID"]
        n_agents = row["number of agents"]
        network = row["network"]
        until = row["until"]
        parallel = row["parallel"]
        agents_with_pv = row["agentsWithPV"]
        agents_with_household = row["agentsWithHousehold"]
        offset = row["offset"]
        if isinstance(agents_with_pv, str):
            agents_with_pv = agents_with_pv.split(";")
        else:
            # agents_with_pv is nan, therefore no agents with pv plants are given
            agents_with_pv = []

        if isinstance(agents_with_household, str):
            agents_with_household = agents_with_household.split(";")
        else:
            # agents_with_household is nan, therefore no agents with households are given
            agents_with_household = []

        infrastructure_changes = get_infrastructure_changes(row)

        if row["content_length"] != '' and not math.isnan(row["content_length"]):
            content_length = int(row["content_length"])
        else:
            content_length = None

        log(f'Simulate scenario {id} with {n_agents} agents, omnet network {network}, parallel= {parallel}, offset= '
            f'{offset} and infrastructure changes {infrastructure_changes}')
        scenario_config.LOGGING_LEVEL = "info"
        if len(infrastructure_changes) > 0:
            communication_scenario.main(num_agents=n_agents, omnet_network=network, parallel=parallel,
                                        agents_with_pv=agents_with_pv, agents_with_household=agents_with_household,
                                        offset=offset, sim_end=until,
                                        infrastructure_changes=infrastructure_changes, content_length=content_length)
        else:
            communication_scenario.main(num_agents=n_agents, omnet_network=network, parallel=parallel,
                                        agents_with_pv=agents_with_pv, agents_with_household=agents_with_household,
                                        offset=offset, sim_end=until, content_length=content_length)
        time.sleep(5)
        update_snapshot(df, id, PATH_TO_TIC_TOC_SCENARIO_TEST_CASES)
        print("\n \n")


if __name__ == "__main__":
    update_negotiation_scenarios()
    update_tic_toc_scenarios()

