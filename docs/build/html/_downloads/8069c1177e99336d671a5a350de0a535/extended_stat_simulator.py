from pathlib import Path
import mosaik_api
from cosima_core.util.util_functions import log
import matplotlib.pyplot as plt

META = {
    'api_version': '3.0',
    'models': {
        'Statistics': {
            'public': True,
            'params': [],
            'attrs': ['stats', 'message'],
        },
    }, 'type': 'time-based'}


class StatisticsSimulator(mosaik_api.Simulator):
    def __init__(self):
        super().__init__(META)
        self._sid = None
        self.index = []
        self.stats = []
        self.network = ""
        self.path = None
        self.step_time = 0
        self.show_plots = True
        self.save_plots = False

    def init(self, sid, **sim_params):
        self._sid = sid
        self.path = Path(__file__).parents[2].joinpath('cosima_omnetpp_project/results/')

        if 'network' in sim_params:
            self.network = sim_params['network']

        if 'step_time' in sim_params:
            self.step_time = sim_params['step_time']

        if 'show_plots' in sim_params:
            self.show_plots = sim_params['show_plots']

        if 'save_plots' in sim_params:
            self.save_plots = sim_params['save_plots']

        return META

    def create(self, num, model, **model_conf):
        return [{'eid': self._sid, 'type': model}]

    def step(self, time, inputs, max_advance):
        log(f'StatisticsSimulator steps in {time}. ', log_type='info')
        if time == 0:
            if self.step_time == 0:
                self.step_time = self.mosaik.world.until - 1
                return self.step_time
            else:
                return self.step_time
        self.stats = []
        self.index = []

        vec_path = self.path.joinpath(f"{self.network}-#0.vec")

        with open(vec_path, 'r') as f:
            lines = f.readlines()[:-1]

        for line in lines:
            if line.split(' ')[0] == 'vector' and line.strip():
                client = line.split(' ')[2].split('.')[1]
                vector = line.split(' ')[3].split(':')[0]
                vec_id = line.split(' ')[1]
                self.index.append({
                    'client': client,
                    'vector': vector,
                    'index': vec_id,
                })
        f.close()

        for d in self.index:
            value_vec = []
            time_vec = []
            for line in lines:
                if line.strip() and line.split()[0] == d['index']:
                    value_vec.append(float(line.split()[3]))
                    time_vec.append(float(line.split()[2]))

            if value_vec:
                self.stats.append({
                    'client': d['client'],
                    'vector': d['vector'],
                    'value': value_vec,
                    'time': time_vec
                })

        return time + self.step_time

    def get_data(self, outputs):
        data = {}
        if self.stats:
            data = {self._sid: {'stats': self.stats}}
        return data

    def finalize(self):
        log(f'Finalize StatisticsSimulator')

        self.calc_client_sent()
        self.calc_client_received()
        self.calc_throughput()

        if self.show_plots:
            plt.show()

    def calc_client_sent(self):
        clients = []
        pkts = []

        for stats in self.stats:
            if stats['vector'] == 'packetSent':
                clients.append(stats['client'])
                pkts.append(len(stats['value']))

        fig, ax = plt.subplots()
        bar_container = ax.bar(clients, pkts)
        ax.set(ylabel='Number of Packets', title='Packets Sent')
        ax.grid(color='gray', linestyle='--', linewidth=0.5, axis='y')
        ax.bar_label(bar_container)
        if self.save_plots:
            plt.savefig('../results/PacketSent.png')

    def calc_client_received(self):
        clients = []
        pkts = []

        for stats in self.stats:
            if stats['vector'] == 'packetReceived':
                clients.append(stats['client'])
                pkts.append(len(stats['value']))

        fig, ax = plt.subplots()
        bar_container = ax.bar(clients, pkts)
        ax.set(ylabel='Number of Packets', title='Packets Received')
        ax.grid(color='gray', linestyle='--', linewidth=0.5, axis='y')
        ax.bar_label(bar_container)
        if self.save_plots:
            plt.savefig('../results/PacketReceived.png')

    def calc_throughput(self):
        clients = []
        pkts = []
        times = []

        for stats in self.stats:
            if stats['vector'] == 'packetReceived':
                clients.append(stats['client'])
                pkts.append(stats['value'])
                times.append(stats['time'])

        for i, client in enumerate(clients):
            fig, ax = plt.subplots()
            ax.boxplot(self.calc_throughput_per_client(pkts[i], times[i]))
            ax.set(ylabel='bits per second', title='Throughput ' + client)
            ax.grid(color='gray', linestyle='--', linewidth=0.5, axis='y')
            ax.set_xlabel(client)
            if self.save_plots:
                plt.savefig('../results/Throughput_' + client + '.png')

    def calc_throughput_per_client(self, pkt_values, pkt_times):
        output = []
        pkt_values_copy = pkt_values
        network_path_latency = 0.060

        if len(pkt_times) == 0:
            times = [0]
            pkt_times = times

        max_time = network_path_latency * round(pkt_times[-1] / network_path_latency)
        if max_time < pkt_times[-1]:
            max_time = max_time + network_path_latency

        time_vec = []
        for i in range(round(max_time / network_path_latency) + 1):
            time_vec.append(network_path_latency * i)

        for t in time_vec:
            sum_of_bps = 0
            for i, pkt in enumerate(pkt_times):
                if pkt < t:
                    sum_of_bps += pkt_values_copy[i]
                    pkt_values_copy[i] = 0
            output.append(sum_of_bps)

        return output
