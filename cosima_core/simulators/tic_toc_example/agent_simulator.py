"""
A test simulator for the cosima project.

"""

from time import sleep

import mosaik_api
import pandas as pd

from cosima_core.config import CONNECT_ATTR, PARALLEL, OFFSET, TIME_BASED
from cosima_core.util.util_functions import log

sim_meta = {
    'models': {
        'A': {
            'public': True,
            'params': [],
            'attrs': [
                'message_with_delay',  # input
                'next_step',  # output
                'ACK',  # output
            ],
        },
    },
}
if TIME_BASED:
    sim_meta['type'] = 'time-based'
else:
    sim_meta['type'] = 'event-based'


class Agent(mosaik_api.Simulator):
    """Simulator for simple agent behaviour"""

    def __init__(self):
        super().__init__(sim_meta)
        self.sid = None
        self.eid = None
        self.step_size = None
        self.value = None
        self.event_setter_wait = None
        self._max_advance = None
        self._agent_name = None
        self._neighbors = []
        # time for output messages
        self._output_time = None
        self._send_msg = True
        self._msgs = []
        self._content_to_send = None
        self._saved_inputs = dict()
        self._output_step = None
        self._msg_counter = 0
        self._calculating_time = 0
        self.event_queue = None
        self.parallel = False
        self._pv_output = None

    def init(self, sid, step_size=1,
             agent_name=None, content_path=None, calculating_time=0, agent_names=None):
        self.sid = sid
        if agent_names is None:
            agent_names = []
        self.step_size = step_size
        self._calculating_time = calculating_time

        self._agent_name = agent_name
        model_name = ''
        for model_name, information in self.meta["models"].items():
            for idx, attr in enumerate(
                    self.meta["models"][model_name]['attrs']):
                if attr.startswith('message_with_delay'):
                    self.meta["models"][model_name]['attrs'][
                        idx] = f'{CONNECT_ATTR}{self._agent_name}'

        self.meta["models"][model_name]['attrs'].append(
            f'message')
        self.meta["models"][model_name]['attrs'].append('P')

        for agent_name in agent_names:
            if not agent_name == self._agent_name:
                self._neighbors.append(agent_name)

        self.parallel = PARALLEL

        if content_path is not None:
            self._content_to_send = pd.read_csv(content_path, delimiter=';',
                                                encoding="utf-8-sig")[
                "content"].to_list()

        return self.meta

    def create(self, num, model, **model_params):
        if num > 1 or self.eid:
            raise Exception("Only one entity allowed for AgentSim.")
        self.eid = self._agent_name
        return [{'eid': self.eid, 'type': model}]

    def step(self, time, inputs, max_advance=None):
        self._pv_output = None
        log(f'{self._agent_name} steps in {time} with input {inputs}')
        if TIME_BASED:
            self._output_time = time
        else:
            # Output time needs to be time + 1 to make sure that mosaik considers
            # all outputs from all agents to prevent errors in stepping order.
            self._output_time = time + 1
        self._max_advance = max_advance
        if not inputs:
            inputs = self._saved_inputs
            self._saved_inputs = dict()

        for eid, attr_names_to_source_values in inputs.items():
            for attribute_name, sources_to_values in \
                    attr_names_to_source_values.items():
                for source, messages in sources_to_values.items():
                    if type(messages) is list:
                        for msg in messages:
                            if msg['receiver'] == self.eid:
                                if msg['sim_time'] > time:
                                    self._saved_inputs = inputs
                                    log(
                                        f'Agent {self.agent_name}'
                                        f' performs new step at time'
                                        f' {msg["sim_time"]}')
                                elif msg['sim_time'] < time:
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
                    elif messages:
                        log(f'{self._agent_name} has new input from PV plant.'
                            f' Input is {messages}.')
                        self._pv_output = 1

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
            self._output_time = self.mosaik.world.until - 1
        outputs = []
        if self._calculating_time != 0:
            log(f'{self._agent_name} starts calculating at {self._output_time} for {self._calculating_time} seconds')
            sleep(self._calculating_time)
            log(f'{self._agent_name} stops calculating')
        if self._agent_name == 'client0' and self._send_msg and (
                (TIME_BASED and self._output_time == 0) or (not TIME_BASED and self._output_time == 1)):
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
        elif self._agent_name == 'client1' and self._output_time == OFFSET + 1 and \
                self._send_msg:
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
        return data

    def finalize(self):
        log(f'finalize in agent called. {self._agent_name}')
