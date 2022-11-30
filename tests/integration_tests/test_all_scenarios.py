import os
import time

import pandas as pd
import pytest_check as check
from termcolor import colored

import cosima_core.scenarios.communication_scenario as communication_scenario
from tests.integration_tests.update_snapshots import get_snapshot, \
    get_infrastructure_changes

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
        print(f'Simulate scenario {id} with {n_agents} agents, omnet network {network}, parallel= {parallel}, '
            f'agents with pv plant: {agents_with_pv} and infrastructure changes {infrastructure_changes}')
        # other path necessary than the default one for the scenarios
        cwd = '../cosima_omnetpp_project/'
        if len(infrastructure_changes) > 0:
            communication_scenario.main(num_agents=n_agents, omnet_network=network, parallel=parallel,
                                        agents_with_pv=agents_with_pv, agents_with_household=agents_with_household,
                                        offset=offset, sim_end=row["until"],
                                        infrastructure_changes=infrastructure_changes, cwd=cwd)
        else:
            communication_scenario.main(num_agents=n_agents, omnet_network=network, parallel=parallel,
                                        agents_with_pv=agents_with_pv, agents_with_household=agents_with_household,
                                        offset=offset, sim_end=row["until"], cwd=cwd)

        time.sleep(5)
        check_snapshot(id, row['events'], row['messages'], row['errors'],
                       row['maxAdvance'], row['disconnect'],
                       row['reconnect'], row['lastEvent'])
        print("\n \n")

    print(colored(f'PASSED scenarios: {RESULTS["passed"]}', 'green'))
    print(colored(f'FAILED scenarios: {RESULTS["failed"]}', 'red'))
