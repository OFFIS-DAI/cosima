import os
import time

import pandas as pd
import pytest_check as check
from termcolor import colored

import cosima_core.comm_scenario as comm_scenario
from tests.integration_tests.update_snapshots import get_snapshot, \
    get_infrastructure_changes
from cosima_core.util.util_functions import log

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


def test_scenarios():
    # change working directory because the main is called from the
    # test folder now
    cwd = os.path.abspath(os.path.dirname(__file__))
    new_wd = os.path.abspath(cwd + "/../" + "/../cosima_core")
    os.chdir(new_wd)

    df = pd.read_csv('../tests/integration_tests/scenarios.csv')

    RESULTS['passed'] = list()
    RESULTS['failed'] = list()

    for index, row in df.iterrows():
        id = row["scenarioID"]
        n_agents = row["number of agents"]
        network = row["network"]
        parallel = row["parallel"]
        offset = row["offset"]
        infrastructure_changes = get_infrastructure_changes(row)
        log(f'Simulate scenario {id} with {n_agents} agents, omnet network {network}, parallel= {parallel} and '
            f'infrastructure changes {infrastructure_changes}')
        if len(infrastructure_changes) > 0:
            comm_scenario.main(n_agents, network, parallel, offset, row["until"],
                               infrastructure_changes)
        else:
            comm_scenario.main(n_agents, network, parallel, offset, row["until"])
        time.sleep(5)
        check_snapshot(id, row['events'], row['messages'], row['errors'],
                       row['maxAdvance'], row['disconnect'],
                       row['reconnect'], row['lastEvent'])
        print("\n \n")

    print(colored(f'PASSED scenarios: {RESULTS["passed"]}', 'green'))
    print(colored(f'FAILED scenarios: {RESULTS["failed"]}', 'red'))
