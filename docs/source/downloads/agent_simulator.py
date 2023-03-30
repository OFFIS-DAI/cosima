"""
    Simulator for a simple agent in mosaik.
"""

import mosaik_api

from cosima_core.config import CONNECT_ATTR
from cosima_core.util.util_functions import log

# The simulator meta data that we return in "init()":
META = {
    'api_version': '3.0',
    'type': 'event-based',
    'models': {
        'SimpleAgentModel': {
            'public': True,
            'params': [],
            'attrs': ['message'],
        },
    },
}


class SimpleAgent(mosaik_api.Simulator):

    def __init__(self):
        super().__init__(META)
        self._sid = None
        self._client_name = None
        self._msg_counter = 0
        self._outbox = []
        self._output_time = 0
        self._neighbor = None

    def init(self, sid, **sim_params):
        self._sid = sid
        if 'client_name' in sim_params.keys():
            self.meta['models']['SimpleAgentModel']['attrs'].append(f'{CONNECT_ATTR}{sim_params["client_name"]}')
            self._client_name = sim_params['client_name']
        if 'neighbor' in sim_params.keys():
            self._neighbor = sim_params['neighbor']
        return META

    def create(self, num, model, **model_conf):
        return [{'eid': self._sid, 'type': model}]

    def step(self, time, inputs, max_advance):
        log(f'{self._client_name} received input {inputs}')
        content = 'Hi neighbor!'
        self._outbox.append({'msg_id': f'{self._client_name}_{self._msg_counter}',
                             'max_advance': max_advance,
                             'sim_time': time + 1,
                             'sender': self._client_name,
                             'receiver': self._neighbor,
                             'content': content,
                             'creation_time': time,
                             })
        self._msg_counter += 1
        self._output_time = time + 1
        return None

    def get_data(self, outputs):
        data = {}
        if self._outbox:
            data = {self._sid: {f'message': self._outbox}, 'time': self._output_time}
            self._outbox = []
        return data

    def finalize(self):
        log('Finalize SimpleAgent')
