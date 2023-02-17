import os
import time

import pandas as pd
import pytest_check as check
from termcolor import colored
import math

import cosima_core.scenarios.communication_scenario as communication_scenario
import cosima_core.scenarios.negotiation_scenario as negotiation_scenario
import scenario_config

from tests.integration_tests.update_snapshots import get_snapshot, \
    get_infrastructure_changes

# change working directory because the main is called from the
# test folder now
cwd = os.path.abspath(os.path.dirname(__file__))
new_wd = os.path.abspath(cwd + "/../" + "/../cosima_core/scenarios")
os.chdir(new_wd)

PATH_TO_TIC_TOC_SCENARIO_TEST_CASES = '../../tests/integration_tests/tictoc_scenarios.csv'
PATH_TO_NEGOTIATION_SCENARIO_TEST_CASES = '../../tests/integration_tests/negotiation_scenarios.csv'

RESULTS = dict()


@check.check_func
def check_snapshot(id, events, messages, errors, max_advance, disconnect,
                   reconnect, last_event):
    snapshot = get_snapshot()
    if snapshot['events'] != events \
            or snapshot['messages'] != messages \
            or snapshot['errors'] != errors \
            or snapshot['max_advance'] != max_advance \
            or snapshot['disconnect'] != disconnect \
            or snapshot['reconnect'] != reconnect \
            or snapshot['last_event'] != last_event:
        print(colored(f'Wrong snapshot!', 'red'))
        print(colored(f'Snapshot is {snapshot}. ', 'red'))
        print(colored(
            f'Expected values are events: {events}, messages: {messages}, errors: {errors}, max_advance: '
            f'{max_advance}, disconnect: {disconnect}, reconnect: {reconnect}, last_event: {last_event}',
            'red'))
        RESULTS['failed'].append(id)
        assert False
    else:
        print(colored(f'Correct snapshot!', 'green'))
        RESULTS['passed'].append(id)
        assert True


def test_negotiation_scenario():
    df = pd.read_csv(PATH_TO_NEGOTIATION_SCENARIO_TEST_CASES)
    RESULTS['passed'] = list()
    RESULTS['failed'] = list()
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
        scenario_config.LOGGING_LEVEL = "warning"
        negotiation_scenario.main(start_mode='cmd',
                                  number_of_agents=number_of_agents,
                                  network=network,
                                  simulation_end=simulation_end,
                                  threshold=threshold,
                                  check_inbox_interval=check_inbox_interval,
                                  infrastructure_changes=infrastructure_changes)
        time.sleep(5)
        check_snapshot(id, row['events'], row['messages'], row['errors'],
                       row['maxAdvance'], row['disconnect'],
                       row['reconnect'], row['lastEvent'])
        print("\n \n")
    print(colored(f'PASSED scenarios: {RESULTS["passed"]}', 'green'))
    print(colored(f'FAILED scenarios: {RESULTS["failed"]}', 'red'))


def test_tic_toc_scenarios():
    df = pd.read_csv(PATH_TO_TIC_TOC_SCENARIO_TEST_CASES)

    RESULTS['passed'] = list()
    RESULTS['failed'] = list()

    for index, row in df.iterrows():
        id = row["scenarioID"]
        num_agents = row["number of agents"]
        network = row["network"]
        until = row["until"]
        parallel = row["parallel"]
        agents_with_pv = row["agentsWithPV"]
        agents_with_household = row["agentsWithHousehold"]
        offset = row["offset"]
        infrastructure_changes = get_infrastructure_changes(row)
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

        if row["content_length"] != '' and not math.isnan(row["content_length"]):
            content_length = int(row["content_length"])
        else:
            content_length = None

        print(f'Simulate scenario {id} with {num_agents} agents, omnet network {network}, parallel= {parallel}, offset= '
              f'{offset}, infrastructure changes {infrastructure_changes} and content length={content_length} '
              f'until {until}')
        # other path necessary than the default one for the scenarios
        scenario_config.LOGGING_LEVEL = "warning"
        if len(infrastructure_changes) > 0:
            communication_scenario.main(num_agents=num_agents, omnet_network=network, parallel=parallel,
                                        agents_with_pv=agents_with_pv, agents_with_household=agents_with_household,
                                        offset=offset, sim_end=row["until"],
                                        infrastructure_changes=infrastructure_changes, content_length=content_length)
        else:
            communication_scenario.main(num_agents=num_agents, omnet_network=network, parallel=parallel,
                                        agents_with_pv=agents_with_pv, agents_with_household=agents_with_household,
                                        offset=offset, sim_end=row["until"], content_length=content_length)

        time.sleep(5)
        check_snapshot(id, row['events'], row['messages'], row['errors'],
                       row['maxAdvance'], row['disconnect'],
                       row['reconnect'], row['lastEvent'])
        print("\n \n")

    print(colored(f'PASSED scenarios: {RESULTS["passed"]}', 'green'))
    print(colored(f'FAILED scenarios: {RESULTS["failed"]}', 'red'))
