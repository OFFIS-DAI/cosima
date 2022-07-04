"""
A simple data collector that prints all data when the simulation finishes.

"""
import collections
import time

import mosaik_api
import pandas as pd

import cosima_core.config as cfg

META = {
    'type': 'event-based',
    'models': {
        'Monitor': {
            'public': True,
            'any_inputs': True,
            'params': [],
            'attrs': [],
        },
    },
}


class Collector(mosaik_api.Simulator):
    def __init__(self):
        super().__init__(META)
        self.eid = None
        self.data = collections.defaultdict(lambda:
                                            collections.defaultdict(dict))
        self.start_time = time.time()
        self.comm_sim_steps = 0

    def init(self, sid):
        return self.meta

    def create(self, num, model):
        if num > 1 or self.eid is not None:
            raise RuntimeError('Can only create one instance of Monitor.')

        self.eid = 'Monitor'
        return [{'eid': self.eid, 'type': model}]

    def step(self, time, inputs, max_advance):
        data = inputs.get(self.eid, {})
        for attr, values in data.items():
            for src, value in values.items():
                try:
                    self.data[src][attr][time].append(value)
                except KeyError:
                    self.data[src][attr][time] = []
                    self.data[src][attr][time].append(value)

        return None

    def finalize(self):
        print('Collected data:')
        final_data = pd.DataFrame(
            columns=["Network", "Number of agents", "Simulation end", "Simulation duration in seconds", "Number of steps",
                     "is parallel", "Simulator", "message id", "creation time", "output time", "sender", "receiver",
                     "message", "delay", "infrastructure change", "changed module"])
        if cfg.START_MODE == 'cmd':
            new_row = {"Network": cfg.NETWORK, "Number of agents": cfg.NUMBER_OF_AGENTS,
                       "Simulation end": cfg.SIMULATION_END, "is parallel": {cfg.PARALLEL}}
        else:
            new_row = {"Number of agents": cfg.NUMBER_OF_AGENTS,
                       "Simulation end": cfg.SIMULATION_END, "is parallel": {cfg.PARALLEL}}
        if cfg.WRITE_RESULTS:
            final_data = final_data.append(new_row, ignore_index=True)
            for sim, sim_data in sorted(self.data.items()):
                print('- %s:' % sim)
                for attr, values in sorted(sim_data.items()):
                    print('  - %s: %s' % (attr, values))
                    for value in values.values():
                        if 'ICTController' in sim and len(value[0]) > 0:
                            value = value[0][0]
                            if value['steps'] > self.comm_sim_steps:
                                self.comm_sim_steps = value['steps']
                            change_type = ""
                            if value['type'] == 5:
                                change_type = "DISCONNECT"
                            elif value['type'] == 6:
                                change_type = "RECONNECT"
                            new_row = {"Simulator": sim, "message id": value['msg_id'], "creation time": value['sim_time'],
                                       "infrastructure change": change_type, "changed module": value['module_name']}
                            final_data = final_data.append(new_row, ignore_index=True)
                        elif 'CommSim' in sim:
                            if value is not None and type(value) is list and type(value[0]) is list:
                                value = value[0][0]
                                if value['steps'] > self.comm_sim_steps:
                                    self.comm_sim_steps = value['steps']
                                new_row = {"Simulator": sim, "message id": value['msg_id'],
                                           "creation time": value['creation_time'], "output time": value['sim_time'],
                                           "sender": value['sender'], "receiver": value['receiver'],
                                           "message": value['content'],
                                           "delay": value['sim_time']-value['creation_time']}
                                final_data = final_data.append(new_row, ignore_index=True)
            # set simulation duration
            final_data.iloc[0, 3] = round(time.time() - self.start_time, 2)
            # set number of steps of commsim
            final_data.iloc[0, 4] = self.comm_sim_steps
            final_data.to_csv(cfg.RESULTS_FILENAME + str(time.time()) + ".csv", index=False)


if __name__ == '__main__':
    mosaik_api.start_simulation(Collector())
