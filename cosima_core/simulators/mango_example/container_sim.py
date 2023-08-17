import asyncio
import math
from copy import deepcopy

import mosaik_api

from mango import create_container
from mango.agent.role import RoleAgent

from cosima_core.util.general_config import CONNECT_ATTR
from cosima_core.util.util_functions import log

# The simulator meta data that we return in "init()":
META = {
    'api_version': '3.0',
    'type': 'event-based',
    'models': {
        'ContainerModel': {
            'public': True,
            'params': [],
            'attrs': ['message'],
        },
    },
}


class ContainerSimulator(mosaik_api.Simulator):

    def __init__(self):
        super().__init__(deepcopy(META))
        self._sid = None
        self._container = None
        self._agent = None
        self._client_agent_mapping = None

        self._output = []

        self._host = 'localhost'
        self._port = 0

        self._loop = None
        self._current_container_time = 0
        self._current_simulator_time = 0
        self._outputs = {}
        self._buffered_mango_inputs = []
        self._conversion_factor = 1
        self._message_counter = 0

        self._agent_roles = []

    def init(self, sid, time_resolution=1., **sim_params):
        if 'client_name' in sim_params.keys():
            self.meta['models']['ContainerModel']['attrs'].append(f'{CONNECT_ATTR}{sim_params["client_name"]}')
            self._sid = sim_params['client_name']
        if 'port' in sim_params.keys():
            self._port = sim_params['port']
        self._loop = asyncio.get_event_loop()
        if 'conversion_factor' in sim_params.keys():
            self._conversion_factor = sim_params['conversion_factor']
        if 'agent_roles' in sim_params.keys():
            self._agent_roles = sim_params['agent_roles']
        if 'client_agent_mapping' in sim_params.keys() and 'codec' in sim_params.keys():
            self._client_agent_mapping = sim_params['client_agent_mapping']
            self._loop.run_until_complete(self.create_container_and_agent(sim_params['codec']))
        if 'connect_attributes' in sim_params:
            self.meta['models']['ContainerModel']['attrs'].extend(sim_params['connect_attributes'])

        return self.meta

    async def create_container_and_agent(self, codec):
        # container for agent
        self._container = await create_container(addr=self._sid, connection_type='mosaik', codec=codec)
        agent_id = self._client_agent_mapping[self._sid]

        self._agent = RoleAgent(self._container, suggested_aid=agent_id)
        for role in self._agent_roles:
            self._agent.add_role(role)

    @property
    def agent(self):
        return self._agent

    def create(self, num, model, **model_conf):
        return [{'eid': self._sid, 'type': model}]

    def step(self, time, inputs, max_advance):
        log(f'Container Sim {self._sid} received inputs in time {time}', 'info')
        self._current_simulator_time = time
        mango_inputs = []
        for eid, attr_names in inputs.items():
            for attribute, sources_to_values in attr_names.items():
                for values in sources_to_values.values():
                    if type(values) is list:
                        for value in values:
                            value_in_bytes = str.encode(value['content'])
                            mango_inputs.append(value_in_bytes)
                    else:
                        for role in self._agent_roles:
                            if hasattr(role, 'handle_simulator_input'):
                                role.handle_simulator_input(sources_to_values)

        self._buffered_mango_inputs.extend(mango_inputs)
        output = self._loop.run_until_complete(
            self._container.step(time / self._conversion_factor, self._buffered_mango_inputs))
        self._buffered_mango_inputs = []

        duration = math.ceil(output.duration * self._conversion_factor)
        self._current_container_time = time + duration

        # log(f' output from container step: {output}')

        for message in output.messages:
            msg_output_time = math.ceil(message.time * self._conversion_factor)

            output_msg = {'msg_id': f'Message_{self._sid}_{self._message_counter}',
                          'max_advance': max_advance,
                          'sim_time': msg_output_time + 1,
                          'sender': self._sid,
                          'receiver': message.receiver,
                          'content': message.message.decode(),
                          'creation_time': msg_output_time + 1,
                          }
            self._message_counter += 1
            if msg_output_time not in self._outputs:
                self._outputs[msg_output_time] = []
            self._outputs[msg_output_time].append(output_msg)

        if output.next_activity is None and (not self._outputs and output.duration * self._conversion_factor <= 1):
            log(f'Container {self._sid} returns None as next step in step {time}. ', 'info')
            return None

        log(f'{self._sid}: Next activity: {output.next_activity}', 'info')

        if output.next_activity is None:
            # next_step = self._current_container_time
            next_step = None
            log(f'Set next_step to container time {next_step}', 'info')
        else:
            next_step = math.ceil(output.next_activity * self._conversion_factor)  #
            log(f'Set next_step to next activity {next_step}')
            if next_step == time:
                log(f'next_step == time, therefore set next_step to next activity {next_step} + 1', 'info')
                next_step += 1

        if len(self._outputs) > 1:
            output_times_sorted = list(self._outputs.keys())
            output_times_sorted.sort()
            next_step = output_times_sorted[1]
        log(f'Container {self._sid} returns time as next step {next_step} in step {time}. ', 'info')
        return next_step

    def get_data(self, outputs):
        data = {}
        if self._outputs:
            minimal_output_time = min(list(self._outputs.keys()))
            output_object = self._outputs[minimal_output_time]
            data = {self._sid: {f'message': output_object}, 'time': minimal_output_time + 1}
            if self._current_simulator_time == minimal_output_time + 1:
                raise ValueError(f"Output time ({minimal_output_time + 1})is the same as current simulator time ({self._current_simulator_time}).")
            del self._outputs[minimal_output_time]
            log(f'Container Sim {self._sid} returns data for time {data["time"]}', 'info')
        for role in self._agent_roles:
            if hasattr(role, 'get_data'):
                agent_data = role.get_data(outputs=outputs)

                for key, value in agent_data.items():
                    if key in data.keys():
                        data[key].update(value)
                    else:
                        data[key] = agent_data[key]
        return data

    def finalize(self):
        log('finalized called.')
        self._loop.run_until_complete(self._shutdown(self.agent, self._container))

    @staticmethod
    async def _shutdown(*args):
        futs = []
        for arg in args:
            futs.append(arg.shutdown())
        log('Going to shutdown agents and container... ')
        await asyncio.gather(*futs)
        log('done.')

