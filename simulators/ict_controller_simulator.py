"""
ICT Controller simulator for OMNeT++ scenarios.

"""
import mosaik_api

from message_pb2 import CosimaMsgGroup
from util_functions import log

META = {
    'type': 'event-based',
    'models': {
        'ICT': {
            'public': True,
            'params': [],
            'attrs': ['ctrl_message'],
        },
    },
}


class ICTController(mosaik_api.Simulator):
    def __init__(self):
        super().__init__(META)
        self._infrastructure_change_counter = 0
        self.eid = None
        self._infrastructure_changes = None
        self._current_time = None
        self._next_step = 0

    def init(self, sid, time_resolution, infrastructure_changes=[]):
        self.meta["models"]['ICT']['attrs'].append(f'message')
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
        return None

    def get_data(self, outputs):
        outputs = []
        for infrastructure_change in self._infrastructure_changes:
            if infrastructure_change['time'] == self._next_step:
                if infrastructure_change['type'] == 'Disconnect':
                    msg_type = CosimaMsgGroup.CosimaMsg.MsgType.DISCONNECT
                else:
                    msg_type = CosimaMsgGroup.CosimaMsg.MsgType.RECONNECT
                outputs.append({
                    'type': msg_type,
                    'module_name': infrastructure_change['module'],
                    'simTime': infrastructure_change['time'],
                    'msgId': f'ICTController{infrastructure_change["type"]}_{self._infrastructure_change_counter}'
                })
                self._infrastructure_change_counter += 1
        data = {self.eid: {f'message': outputs}, 'time': self._current_time + 1}

        return data
