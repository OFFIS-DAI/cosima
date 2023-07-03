from pathlib import Path
import mosaik_api
from cosima_core.util.util_functions import log

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

        return time + self.step_time

    def get_data(self, outputs):
        data = {}
        if self.stats:
            data = {self._sid: {'stats': self.stats}}
        return data

    def finalize(self):
        log(f'Finalize StatisticsSimulator')
