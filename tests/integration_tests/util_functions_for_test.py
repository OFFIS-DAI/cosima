import os

from cosima_core.util.general_config import ROOT_PATH


def terminate_test_process(p, timeout=1500):
    p.join(timeout)
    # If thread is still active
    if p.is_alive():
        print('is still alive')
        p.terminate()
        p.join()
        os.system('killall -9 cosima_omnetpp_project')
        os.system('fuser -k 4243/tcp')
        assert False


def get_snapshot():
    snapshot_file = str(ROOT_PATH.parent / 'tests' / 'integration_tests' / 'snapshot.txt')
    with open(snapshot_file) as f:
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
