from abc import ABC
from typing import Dict, Any
from mango import Agent
from json_tricks import dumps, loads

from cosima_core.util.util_functions import log


class UnitAgent(Agent, ABC):

    def __init__(self, container, neighbors, aid):
        super().__init__(container, suggested_aid=aid)
        self.aid = aid
        self._neighbors = neighbors
        self.unit_values = dict()

    def handle_message(self, content, meta: Dict[str, Any]):
        log(f'UnitAgent {self.aid} received: {loads(content)}')

    async def send_values(self):
        for neighbor in self._neighbors:
            log(f'UnitAgent sends its power values to {neighbor}', 'info')
            values = dumps({self.aid: self.unit_values})
            await self.send_message(receiver_addr=neighbor[0], receiver_id=neighbor[1],
                                    acl_metadata={'sender_id': self.aid},
                                    create_acl=True, content=values,
                                    )

    def set_inputs(self, inputs):
        for client_name, input in inputs.items():
            for attr, value_dict in input.items():
                for simulator_attr, value in value_dict.items():
                    self.unit_values[simulator_attr] = value
        log(f'Agent {self.aid} has new values: {self.unit_values}', 'info')
        self.schedule_instant_task(self.send_values())

    def get_data(self, outputs):
        data = {}
        for sid, attrs in outputs.items():
            for attr in attrs:
                if attr == 'p_set_point':
                 data[sid] = {attr: list(self.unit_values.values())[0]}
        return data
