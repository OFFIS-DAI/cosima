"""
A test simulator for the cosima project.

"""
import json
from time import sleep

import mosaik_api
import numpy as np

from cosima_core.util.general_config import CONNECT_ATTR
from cosima_core.util.util_functions import log

sim_meta = {'models': {
    'AgentModel': {
        'public': True,
        'params': [],
        'attrs': [
            'message_with_delay',  # input
            'message',  # output
            'P',  # input
            'ACK',  # output
            'LOAD_ACK',  # output
        ],
    },
}, 'type': 'event-based'}


class Agent(mosaik_api.Simulator):
    """Simulator for simple agent behaviour"""

    def __init__(self):
        super().__init__(sim_meta)
        self.eid = None
        self._agent_name = None
        self._neighbors = []
        # time for output messages
        self._output_time = None
        self._outbox = []
        self._msg_counter = 0
        self._calculating_time = 0
        self._current_power = dict()
        self._power_product = {}
        self._threshold = np.inf
        self._check_inbox_interval = 1
        self._updated_power_product = False
        self._solution_found = False
        self._start_negotiation = False

    def init(self, sid, step_size=1, client_name=None, calculating_time=0, neighbors=None, threshold=np.inf,
             check_inbox_interval=1, start_negotiation=False):
        self._calculating_time = calculating_time

        self._agent_name = client_name
        self._power_product[self.agent_name] = 0
        for model_name, information in self.meta["models"].items():
            for idx, attr in enumerate(
                    self.meta["models"][model_name]['attrs']):
                if attr.startswith('message_with_delay'):
                    self.meta["models"][model_name]['attrs'][
                        idx] = f'{CONNECT_ATTR}{self._agent_name}'

        if neighbors is not None:
            self._neighbors = neighbors

        self._start_negotiation = start_negotiation
        self._threshold = threshold
        self._check_inbox_interval = check_inbox_interval

        return self.meta

    def create(self, num, model, **model_params):
        if num > 1 or self.eid:
            raise Exception("Only one entity allowed for AgentSim.")
        self.eid = self._agent_name
        return [{'eid': self.eid, 'type': model}]

    def step(self, time, inputs, max_advance=None):
        log(f'{self._agent_name} steps in {time} with input {inputs}')
        # Output time needs to be time + 1 to make sure that mosaik considers
        # all outputs from all agents to prevent errors in stepping order.

        if self._solution_found:
            return None

        for eid, attr_names_to_source_values in inputs.items():
            for attribute_name, sources_to_values in \
                    attr_names_to_source_values.items():
                for source, values in sources_to_values.items():
                    if type(values) is list:
                        for msg in values:
                            if msg['receiver'] == self.eid:
                                if msg['sim_time'] < time:
                                    raise RuntimeError(
                                        f'Agent {self.agent_name} received message'
                                        f' at time {time}, expected time is '
                                        f'{msg["sim_time"]}.')
                                elif msg['sim_time'] == time and \
                                        msg is not None:
                                    log(
                                        f'{self.agent_name} received {msg}')
                                    self._power_product.update(json.loads(msg['content']))
                                    self._power_product[self._agent_name] = sum(self._current_power.values())
                                    self._updated_power_product = True
                                    log(f'Step {time}: '
                                        f'{self.agent_name} updated his own power product to: {self._power_product}')

                    elif values is not None:
                        log(f'{self._agent_name} has new input from PV plant or household.'
                            f' Input is {values}.')
                        self._current_power[source] = values
                        self._power_product[self._agent_name] = sum(self._current_power.values())

        if (time % self._check_inbox_interval == 0 and self._updated_power_product and time != 0) or \
                (self._start_negotiation and time == self._check_inbox_interval):
            self._updated_power_product = False
            # agent is active
            power_sum = sum(self._power_product.values())
            if power_sum >= self._threshold:
                self._solution_found = True
                log(f'IMPORTANT: {self._agent_name} found a solution: {power_sum} with {self._power_product} at time '
                    f'{time}')
                self.mosaik.world.until = time
            if time % self._check_inbox_interval == 0:
                self._output_time = time + 1
                next_step = time + self._check_inbox_interval
            else:
                self._output_time = time + self._check_inbox_interval - (time % self._check_inbox_interval)
                next_step = self._output_time
            for neighbor in self._neighbors:
                self._outbox.append({'msg_id': f'AgentMessage_{self._agent_name}_'
                                               f'{self._msg_counter}',
                                     'max_advance': max_advance,
                                     'sim_time': self._output_time,
                                     'sender': self._agent_name,
                                     'receiver': neighbor,
                                     'content': json.dumps(self._power_product),
                                     'creation_time': self._output_time,
                                     })
                self._msg_counter += 1
            return next_step
        else:
            return time + self._check_inbox_interval - (time % self._check_inbox_interval)

    @property
    def agent_name(self):
        """get agent name"""
        return self._agent_name

    def get_data(self, outputs):
        if not self._output_time or self._output_time >= self.mosaik.world.until:
            return {}
        outputs = []
        if self._calculating_time != 0:
            log(f'{self._agent_name} starts calculating at {self._output_time} for {self._calculating_time} seconds')
            sleep(self._calculating_time)
            log(f'{self._agent_name} stops calculating')

            data = {self.eid: {f'message': outputs}}

        elif len(self._outbox) != 0:
            data = {self.eid: {f'message': self._outbox}}
        else:
            data = {}
        self._outbox = []

        if data:
            data['time'] = self._output_time
        return data

    def finalize(self):
        pass  # log(f'finalize in agent called. {self._agent_name}')
