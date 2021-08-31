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

from message_pb2 import CosimaMsgGroup

sim_meta = {
    'type': 'event-based',
    'models': {
        'A': {
            'public': True,
            'params': [],
            'attrs': ['message',  # output
                      'message_with_delay'],  # input
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
        self._time = None
        self._send_msg = True
        self._answer = None
        self._content_to_send = None
        self._saved_inputs = dict()

    def init(self, sid, step_type='time-based', step_size=1, self_steps={},
             wallclock_duration=0., output_timing=None, events={},
             config_file=None, agent_name=None,
             content_path=None):
        for model_name, information in self.meta["models"].items():
            self.meta["models"][model_name]['attrs'].append(agent_name)
        self.sid = sid
        self.step_type = step_type
        self.meta['type'] = step_type
        self.step_size = step_size
        self.self_steps = {float(key): val for key, val in self_steps.items()}
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
        with open(config_file, "r") as jsonfile:
            config = json.load(jsonfile)

        for keys in config['connections']:
            if self._agent_name in keys:
                neighbor_name = keys[(len(self._agent_name) + 2):]
                if neighbor_name == self._agent_name:
                    continue
                self._neighbors.append(neighbor_name)

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
        next_step = None
        self._time = time
        self._max_advance = max_advance
        if self.wallclock_duration:
            sleep(self.wallclock_duration)
        if not inputs:
            inputs = self._saved_inputs
            self._saved_inputs = dict()

        for eid, attr_names_to_source_values in inputs.items():
            for attribute, sources_to_values in \
                    attr_names_to_source_values.items():
                if attribute == 'message_with_delay':
                    messages = \
                        list(sources_to_values.values())[0]
                    for msg in messages:
                        self._answer = None
                        if msg['receiver'] == self.eid:
                            if msg['output_time'] > time:
                                next_step = msg['output_time']
                                self._saved_inputs = inputs
                                print(f'Agent {self.agent_name} performs new step at time {msg["output_time"]}')
                            elif msg['output_time'] < time:
                                raise RuntimeError(
                                    f'Agent {self.agent_name} received message at time {time}, expected time is '
                                    f'{msg["output_time"]}.')
                            elif msg['output_time'] == time and msg is not None:
                                print(
                                    f'{self.agent_name} received {msg}')
                                # send reply
                                message_text = self.get_next_content(
                                    msg['message'])
                                self._answer = {
                                    'type': CosimaMsgGroup.CosimaMsg.MsgType.INFO,
                                    'sender': self._agent_name,
                                    'receiver': msg['sender'],
                                    'max_advance': self._max_advance,
                                    'message': message_text,
                                    'simTime': self._time,
                                    'stepSize': self.step_size}
        return next_step

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
        outputs = []
        if self._time == 0 and self._agent_name == 'client0' and \
                self._send_msg:
            self._send_msg = False
            for neighbor in self._neighbors:
                outputs.append({'type': CosimaMsgGroup.CosimaMsg.MsgType.INFO,
                                'sender': self._agent_name,
                                'receiver': neighbor,
                                'max_advance': self._max_advance,
                                'message': 'Tic Toc on the clock!',
                                'simTime': self._time,
                                'stepSize': self.step_size})
            data = {self.eid: {'message': outputs}}
        elif self._agent_name == 'client1' and self._time == 1 and \
                self._send_msg:
            self._send_msg = False
            for neighbor in self._neighbors:
                outputs.append({'type': CosimaMsgGroup.CosimaMsg.MsgType.INFO,
                                'sender': self._agent_name,
                                'receiver': neighbor,
                                'max_advance': self._max_advance,
                                'message': 'Sending other side of parallel message',
                                'simTime': self._time,
                                'stepSize': self.step_size})
            data = {self.eid: {'message': outputs}}

        elif self._answer is not None:
            data = {self.eid: {'message': self._answer}}
            self._answer = None
        else:
            data = {}

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
