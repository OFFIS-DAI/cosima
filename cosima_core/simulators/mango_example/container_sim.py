import asyncio
from abc import ABC, abstractmethod
from copy import deepcopy

import mosaik_api
import math

from cosima_core.simulators.mango_example.reply_agent import ReplyAgent
from mango_library.negotiation.cohda.cohda import COHDARole, CohdaNegotiationStarterRole

from mango.role.core import RoleAgent

from cosima_core.util.general_config import CONNECT_ATTR
from cosima_core.util.util_functions import log
from mango.core.container import Container
from mango_library.coalition.core import CoalitionParticipantRole, CoalitionInitiatorRole

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
        super().__init__(META)
        self._sid = None
        self._container = None
        self._agent = None
        self._client_agent_mapping = None

        self._output = []

        self._host = 'localhost'
        self._port = 0

        self._loop = None
        self._current_container_time = 0
        self._outputs = {}
        self._buffered_mango_inputs = []
        self._conversion_factor = 1
        self._message_counter = 0

    def init(self, sid, time_resolution=1., **sim_params):
        if 'client_name' in sim_params.keys():
            self.meta['models']['ContainerModel']['attrs'].append(f'{CONNECT_ATTR}{sim_params["client_name"]}')
            self._sid = sim_params['client_name']
        if 'port' in sim_params.keys():
            self._port = sim_params['port']
        self._loop = asyncio.get_event_loop()
        if 'client_agent_mapping' in sim_params.keys() and 'codec' in sim_params.keys():
            self._client_agent_mapping = sim_params['client_agent_mapping']
            self._loop.run_until_complete(self.create_container_and_agent_model(sim_params['codec']))
        if 'conversion_factor' in sim_params.keys():
            self._conversion_factor = sim_params['conversion_factor']

        return META

    async def create_agent_model(self, neighbors, agent_id):
        pass

    async def create_container_and_agent_model(self, codec):
        # container for agent
        self._container = await Container.factory(addr=self._sid, connection_type='mosaik', codec=codec)
        agent_id = self._client_agent_mapping[self._sid]
        neighbors = deepcopy(self._client_agent_mapping)
        del neighbors[self._sid]
        neighbors = [(key, value) for key, value in neighbors.items()]

        await self.create_agent_model(neighbors=neighbors, agent_id=agent_id)

    @property
    def agent(self):
        return self._agent

    def create(self, num, model, **model_conf):
        return [{'eid': self._sid, 'type': model}]

    def step(self, time, inputs, max_advance):
        log(f'Container Sim {self._sid} received inputs in time {time}')
        mango_inputs = []
        for eid, attr_names in inputs.items():
            for attribute, sources_to_values in attr_names.items():
                for values in sources_to_values.values():
                    if type(values) is list:
                        for value in values:
                            value_in_bytes = str.encode(value['content'])
                            mango_inputs.append(value_in_bytes)

        self._buffered_mango_inputs.extend(mango_inputs)

        output = self._loop.run_until_complete(
            self._container.step(time / self._conversion_factor, self._buffered_mango_inputs))
        self._buffered_mango_inputs = []

        if output.next_activity is None and (not output.messages and output.duration * self._conversion_factor <= 1):
            log(f'Container {self._sid} returns None as next step in step {time}. ')
            return None

        duration = math.ceil(output.duration * self._conversion_factor)
        self._current_container_time = time + duration

        # log(f' output from container step: {output}')

        for message in output.messages:
            msg_output_time = math.ceil(message.time * self._conversion_factor) + 1

            output_msg = {'msg_id': f'Message_{self._sid}_{self._message_counter}',
                          'max_advance': max_advance,
                          'sim_time': msg_output_time,
                          'sender': self._sid,
                          'receiver': message.receiver,
                          'content': message.message.decode(),
                          'creation_time': msg_output_time,
                          }
            self._message_counter += 1
            if msg_output_time not in self._outputs:
                self._outputs[msg_output_time] = []
            self._outputs[msg_output_time].append(output_msg)

        log(f' {self._sid} Output times: {self._outputs.keys()}')

        log(f'{self._sid}: Next activity: {output.next_activity}')

        if output.next_activity is None:
            # next_step = self._current_container_time
            next_step = None
            log(f'Set next_step to container time {next_step}')
        else:
            next_step = math.ceil(output.next_activity * self._conversion_factor)  #
            log(f'Set next_step to next activity {next_step}')
            if next_step == time:
                log(f'next_step == time, therefore set next_step to next activity {next_step} + 1')
                next_step += 1

        if len(self._outputs) > 1:
            output_times_sorted = list(self._outputs.keys())
            output_times_sorted.sort()
            next_step = output_times_sorted[1]
        log(f'Container {self._sid} returns time {next_step} in step {time}. ', 'info')
        return next_step

    def get_data(self, outputs):
        data = {}
        if self._outputs:
            minimal_output_time = min(list(self._outputs.keys()))
            output_object = self._outputs[minimal_output_time]
            data = {self._sid: {f'message': output_object}, 'time': minimal_output_time}
            del self._outputs[minimal_output_time]
            log(f'Container Sim returns data for time {data["time"]}')
        return data

    def finalize(self):
        log('finalized called.')
        self._loop.run_until_complete(self._shutdown(self._container))

    @staticmethod
    async def _shutdown(*args):
        futs = []
        for arg in args:
            futs.append(arg.shutdown())
        log('Going to shutdown agents and container... ')
        await asyncio.gather(*futs)
        log('done.')


class COHDAContainerSimulator(ContainerSimulator):

    def __init__(self):
        super().__init__()
        self._schedules_for_agents = []
        self._is_controller_agent = False
        self._target = None

    def init(self, sid, time_resolution=1., **sim_params):
        if 'schedules_for_agents' in sim_params.keys():
            self._schedules_for_agents = sim_params['schedules_for_agents']
        if 'is_controller_agent' in sim_params.keys():
            self._is_controller_agent = sim_params['is_controller_agent']
            if 'target' in sim_params.keys():
                self._target = sim_params['target']

        return super().init(sid=sid, time_resolution=time_resolution, **sim_params)

    async def create_agent_model(self, neighbors, agent_id):
        self._agent = RoleAgent(self._container, suggested_aid=agent_id)
        cohda_role = COHDARole(schedules_provider=lambda: self._schedules_for_agents[0],
                               local_acceptable_func=lambda s: True)
        self._agent.add_role(cohda_role)
        self._agent.add_role(CoalitionParticipantRole())

    def setup_done(self):
        if self._is_controller_agent:
            addrs = [(key, value) for key, value in self._client_agent_mapping.items()]
            self._agent.add_role(CoalitionInitiatorRole(addrs, 'cohda', 'cohda-negotiation'))
            self._agent.add_role(CohdaNegotiationStarterRole(target_params=self._target))


class SimpleContainerSimulator(ContainerSimulator):

    def __init__(self):
        super().__init__()
        self._agent_type = ReplyAgent

    def init(self, sid, time_resolution=1., **sim_params):
        if 'agent_type' in sim_params.keys():
            self._agent_type = sim_params['agent_type']
        return super().init(sid=sid, time_resolution=time_resolution, **sim_params)

    async def create_agent_model(self, neighbors, agent_id):
        self._agent = self._agent_type(container=self._container, neighbors=neighbors,
                                       aid=agent_id)

    def setup_done(self):
        self._agent.start()
