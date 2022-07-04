"""
ICT Controller simulator for OMNeT++ scenarios.

"""
import mosaik_api

from cosima_core.messages.message_pb2 import InfrastructureMessage
from cosima_core.util.util_functions import log
from cosima_core.config import TIME_BASED

META = {
    'models': {
        'ICT': {
            'public': True,
            'params': [],
            'attrs': ['ctrl_message'],
        },
    },
}

if TIME_BASED:
    META['type'] = 'time-based'
else:
    META['type'] = 'event-based'


class ICTController(mosaik_api.Simulator):
    def __init__(self):
        super().__init__(META)
        self._infrastructure_change_counter = 0
        self.eid = None
        self._infrastructure_changes = None
        self._current_time = None
        self._next_step = 0

    def init(self, sid, time_resolution, infrastructure_changes=[]):
        self.meta["models"]['ICT']['attrs'].append(f'ict_message')
        self.meta["models"]['ICT']['attrs'].append(f'ctrl_message')
        self._infrastructure_changes = infrastructure_changes
        return self.meta

    def create(self, num, model):
        if num > 1:
            raise Exception("Only one entity allowed for ICTController.")
        self.eid = "ict_controller"
        return [{'eid': self.eid, 'type': model}]

    def step(self, time, inputs, max_advance):
        log(f'ICT Controller steps in {time} with {inputs}')
        self._current_time = time
        next_step = None
        for infrastructure_change in self._infrastructure_changes:
            if (not next_step or infrastructure_change['time'] < next_step) and infrastructure_change['time'] > time:
                next_step = infrastructure_change['time']
        self._next_step = next_step
        if TIME_BASED:
            return time + 1
        else:
            return None

    def get_data(self, outputs):
        outputs = []
        current_changes = list()
        for infrastructure_change in self._infrastructure_changes:
            if infrastructure_change['time'] == self._next_step:
                if infrastructure_change['type'] == 'Disconnect':
                    msg_type = InfrastructureMessage.MsgType.DISCONNECT
                else:
                    msg_type = InfrastructureMessage.MsgType.RECONNECT
                outputs.append({
                    'msg_type': msg_type,
                    'msg_id': f'ICTController{infrastructure_change["type"]}_{self._infrastructure_change_counter}',
                    'sim_time': infrastructure_change['time'],
                    'change_module': infrastructure_change['module']
                })
                self._infrastructure_change_counter += 1
                current_changes.append(infrastructure_change)
        self._infrastructure_changes = [change for change in self._infrastructure_changes if change not in current_changes]
        data = {self.eid: {f'ict_message': outputs}, 'time': self._current_time + 1}

        return data
