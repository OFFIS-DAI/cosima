"""
A test simulator for the cosima project.

"""

from time import sleep

import mosaik_api
import pandas as pd

from cosima_core.config import CONNECT_ATTR
from cosima_core.util.util_functions import log

sim_meta = {'models': {
    'A': {
        'public': True,
        'params': [],
        'attrs': [
            'message_with_delay',  # input
            'P',  # input
            'ACK',  # output
            'LOAD_ACK',  # output
            'message',  # output
        ],
    },
}, 'type': 'event-based'}


class Agent(mosaik_api.Simulator):
    """Simulator for simple agent behaviour"""

    def __init__(self):
        super().__init__(sim_meta)
        self.eid = None
        self._max_advance = None
        self._agent_name = None
        self._neighbors = []
        # time for output messages
        self._output_time = None
        self._send_msg = True
        self._msgs = []
        self._content_to_send = None
        self._msg_counter = 0
        self._calculating_time = 0
        self.parallel = False
        self.offset = 0
        self._pv_output = None
        self._household_output = None

    def init(self, sid, step_size=1,
             agent_name=None, content_path=None, calculating_time=0, agent_names=None, parallel=False, offset=0):
        if agent_names is None:
            agent_names = []
        self._calculating_time = calculating_time

        self._agent_name = agent_name
        model_name = ''
        for model_name, information in self.meta["models"].items():
            for idx, attr in enumerate(
                    self.meta["models"][model_name]['attrs']):
                if attr.startswith('message_with_delay'):
                    self.meta["models"][model_name]['attrs'][
                        idx] = f'{CONNECT_ATTR}{self._agent_name}'

        for agent_name in agent_names:
            if not agent_name == self._agent_name:
                self._neighbors.append(agent_name)

        if content_path is not None:
            self._content_to_send = pd.read_csv(content_path, delimiter=';',
                                                encoding="utf-8-sig")[
                "content"].to_list()

        self.parallel = parallel
        self.offset = offset

        return self.meta

    def create(self, num, model, **model_params):
        if num > 1 or self.eid:
            raise Exception("Only one entity allowed for AgentSim.")
        self.eid = self._agent_name
        return [{'eid': self.eid, 'type': model}]

    def step(self, time, inputs, max_advance=None):
        self._pv_output = None
        self._household_output = None
        log(f'{self._agent_name} steps in {time} with input {inputs}')
        # Output time needs to be time + 1 to make sure that mosaik considers
        # all outputs from all agents to prevent errors in stepping order.
        self._output_time = time + 1
        self._max_advance = max_advance

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
                                    # send reply
                                    message_text = self.get_next_content(
                                        msg['content'])
                                    self._msgs.append({'msg_id': f'AgentMessage_{self._agent_name}_'
                                                                 f'{self._msg_counter}',
                                                       'max_advance': self._max_advance,
                                                       'sim_time': self._output_time,
                                                       'sender': self._agent_name,
                                                       'receiver': msg['sender'],
                                                       'content': message_text,
                                                       'creation_time': self._output_time,
                                                       })
                                    self._msg_counter += 1
                    elif values is not None:
                        log(f'{self._agent_name} has new input from {source}.'
                            f' Input is {values}.')
                        if source.startswith('CSV'):
                            self._pv_output = 1
                        else:
                            self._household_output = 1

    def get_next_content(self, message):
        """
        Select next message content for a given content (message) according
        to content saved in content_to_send.
        """
        if message in self._content_to_send:
            index = self._content_to_send.index(message)
            if index + 1 >= len(self._content_to_send):
                content = self._content_to_send[0]
            else:
                content = self._content_to_send[index + 1]
        else:
            # No matching entry found in content list,
            # therefore start with line 0.
            content = self._content_to_send[0]
        return content

    @property
    def agent_name(self):
        """get agent name"""
        return self._agent_name

    def get_data(self, outputs):
        if self._output_time >= self.mosaik.world.until:
            return {}
        outputs = []
        if self._calculating_time != 0:
            log(f'{self._agent_name} starts calculating at {self._output_time} for {self._calculating_time} seconds')
            sleep(self._calculating_time)
            log(f'{self._agent_name} stops calculating')
        if self._agent_name == 'client0' and self._send_msg and self._output_time == 1:
            self._send_msg = False
            for neighbor in self._neighbors:
                outputs.append({'msg_id': f'AgentMessage_{self._agent_name}_'
                                          f'{self._msg_counter}',
                                'max_advance': self._max_advance,
                                'sim_time': self._output_time,
                                'sender': self._agent_name,
                                'receiver': neighbor,
                                'content': 'Lets go through the alphabet!',
                                'creation_time': self._output_time,
                                })
                self._msg_counter += 1
            data = {self.eid: {f'message': outputs}}
        elif self._agent_name == 'client1' and self._output_time == self.offset + 1 and \
                self._send_msg and self.parallel:
            self._send_msg = False
            for neighbor in self._neighbors:
                outputs.append({'msg_id': f'AgentMessage_{self._agent_name}_'
                                          f'{self._msg_counter}',
                                'max_advance': self._max_advance,
                                'sim_time': self._output_time,
                                'sender': self._agent_name,
                                'receiver': neighbor,
                                'content': 'Sending parallel message.',
                                'creation_time': self._output_time,
                                })
                self._msg_counter += 1
            data = {self.eid: {f'message': outputs}}

        elif len(self._msgs) != 0:
            data = {self.eid: {f'message': self._msgs}}
        else:
            data = {}
        self._msgs = []

        if data:
            data['time'] = self._output_time
            if self._pv_output is not None:
                data['ACK'] = self._pv_output
            if self._household_output is not None:
                data['LOAD_ACK'] = self._household_output
        return data

    def finalize(self):
        log(f'finalize in agent called. {self._agent_name}')
