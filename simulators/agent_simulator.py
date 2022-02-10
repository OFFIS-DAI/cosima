"""
A generic test simulator for mosaik_omnetpp_files.

It can be configured in various ways via simulator parameters,
e.g. simulator_a = world.start('A', sim_param1='param1', ..):

step_type : string, {'time-based', 'event-based', 'hybrid'}, default 'time-based')
step_size : int, default=1, only used by time-based simulators
self_steps (dict of {int: int}, default {}), only used by event-based and hybrid simulators
    {step_time: next_step, ..}
    A next step will only be requested (at next_step) for time steps which are
    in self_steps.
wallclock_duration : float, default 0.
    If set, the simulator will sleep for this time (in seconds) during step.
output_timing : dict of {int: int}, optional
    {step_time: output_time, ..}
    If set, output will only be returned at time steps which are in
    output_timing.
events : dict of {float: int}, default {}
    {real_time: event_time, ..}
    An event will be requested for simulation time event_time after real_time
    seconds.
"""

import json
from time import sleep
import pandas as pd
import mosaik_api

from config import CONNECT_ATTR, PARALLEL
from message_pb2 import CosimaMsgGroup
from util_functions import log

sim_meta = {
    'type': 'event-based',
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


class Agent(mosaik_api.Simulator):
    """Simulator for simple agent behaviour"""

    def __init__(self):
        super().__init__(sim_meta)
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

    def init(self, sid, step_type='time-based', step_size=1, self_steps={},
             wallclock_duration=0., output_timing=None, events={},
             agent_name=None, content_path=None, calculating_time=0, agent_names=[]):
        self.sid = sid
        self.step_type = step_type
        self.meta['type'] = step_type
        self.step_size = step_size
        self.self_steps = {float(key): val for key, val in self_steps.items()}
        self._calculating_time = calculating_time
        self.wallclock_duration = wallclock_duration
        if output_timing:
            output_timing = {float(key): val
                             for key, val in output_timing.items()}
        self.output_timing = output_timing

        if step_type == 'hybrid':
            self.meta['models']['A']['persistent'] = ['message']

        self.events = {float(key): val for key, val in events.items()}
        if events:
            self.meta['set_events'] = True

        self._agent_name = agent_name
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

    def create(self, num, model):
        if num > 1 or self.eid:
            raise Exception("Only one entity allowed for AgentSim.")
        self.eid = self._agent_name
        return [{'eid': self.eid, 'type': model}]

    def step(self, time, inputs, max_advance=None):
        self._pv_output = None
        log(f'{self._agent_name} steps in {time} with input {inputs}')
        # Output time needs to be time + 1 to make sure that mosaik considers
        # all outputs from all agents to prevent errors in stepping order.
        self._output_time = time + 1
        self._max_advance = max_advance
        if self.wallclock_duration:
            sleep(self.wallclock_duration)
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
                                if msg['output_time'] > time:
                                    self._saved_inputs = inputs
                                    log(
                                        f'Agent {self.agent_name}'
                                        f' performs new step at time'
                                        f' {msg["output_time"]}')
                                elif msg['output_time'] < time:
                                    raise RuntimeError(
                                        f'Agent {self.agent_name} received message'
                                        f' at time {time}, expected time is '
                                        f'{msg["output_time"]}.')
                                elif msg['output_time'] == time and \
                                        msg is not None:
                                    log(
                                        f'{self.agent_name} received {msg}')
                                    # send reply
                                    message_text = self.get_next_content(
                                        msg['message'])
                                    self._msgs.append({
                                        'type': CosimaMsgGroup.CosimaMsg.MsgType.INFO,
                                        'sender': self._agent_name,
                                        'receiver': msg['sender'],
                                        'max_advance': self._max_advance,
                                        'message': message_text,
                                        'simTime': self._output_time,
                                        'stepSize': self.step_size,
                                        'msgId': f'AgentMessage_{self._agent_name}_{self._msg_counter}'})
                                    self._msg_counter += 1
                    else:
                        log(f'{self._agent_name} has new input from PV plant.'
                              f' Input is {messages}.')
                        self._pv_output = 1

        if self._agent_name == 'client1' and PARALLEL and time == 0:
            return time + 1

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
        if self._output_time == 1 and self._agent_name == 'client0' and \
                self._send_msg:
            self._send_msg = False
            for neighbor in self._neighbors:
                outputs.append({'type': CosimaMsgGroup.CosimaMsg.MsgType.INFO,
                                'sender': self._agent_name,
                                'receiver': neighbor,
                                'max_advance': self._max_advance,
                                'message': 'Tic Toc on the clock!',
                                'simTime': self._output_time,
                                'stepSize': self.step_size,
                                'msgId': f'AgentMessage_{self._agent_name}_'
                                         f'{self._msg_counter}'})
                self._msg_counter += 1
            data = {self.eid: {f'message': outputs}}
        elif self._agent_name == 'client1' and self._output_time == 2 and \
                self._send_msg:
            self._send_msg = False
            for neighbor in self._neighbors:
                outputs.append({'type': CosimaMsgGroup.CosimaMsg.MsgType.INFO,
                                'sender': self._agent_name,
                                'receiver': neighbor,
                                'max_advance': self._max_advance,
                                'message': 'Sending other side of '
                                           'parallel message',
                                'simTime': self._output_time,
                                'stepSize': self.step_size,
                                'msgId': f'AgentMessage_{self._agent_name}_'
                                         f'{self._msg_counter}'})
                self._msg_counter += 1
            data = {self.eid: {f'message': outputs}}

        elif len(self._msgs) != 0:
            data = {self.eid: {f'message': self._msgs}}
        else:
            data = {}
        self._msgs = []

        # If there are events left, send out next_step
        if self.event_queue:
            next_step = self.event_queue[0][0]
        else:
            next_step = None
        if data:
            data['time'] = self._output_time
            if self._pv_output is not None:
                data['ACK'] = self._pv_output
            if next_step is not None:
                data['next_step'] = next_step
        return data

    def setup_done(self):
        if self.event_setter_wait:
            self.event_setter_wait.succeed()

    def event_setter(self, env, message):
        last_time = 0
        wait_event = env.event()
        self.event_setter_wait = wait_event
        yield wait_event
        for real_time, event_time in self.events.items():
            yield env.timeout(real_time - last_time)
            yield message.send(["set_event", [event_time], {}])
            last_time = real_time

    def finalize(self):
        log(f'finalize in agent called. {self._agent_name}')
