from json import dumps
from marshal import loads

from mango import Role

from cosima_core.messages.mango_messages import SimulatorMessage
from cosima_core.util.util_functions import log


class UnitRole(Role):

    def __init__(self, neighbors):
        super().__init__()
        self._neighbors = neighbors
        self.unit_values = dict()

    def setup(self) -> None:
        super().setup()
        # subscribe messages
        self.context.subscribe_message(
            self, self.handle_simulator_message, lambda content, meta: isinstance(content, SimulatorMessage)
        )
        self.context.schedule_instant_task(self.send_values())

    def handle_simulator_message(self, content, meta):
        log(f'UnitRole {self.context.aid} received: {content}')
        self.context.schedule_instant_task(self.send_values())

    def handle_simulator_input(self, values):
        for simulator_attr, value in values.items():
            self.unit_values[simulator_attr] = value
        log(f'Agent {self.context.aid} has new values: {self.unit_values}', 'info')

    async def send_values(self):
        for neighbor in self._neighbors:
            log(f'UnitAgent sends its power values to {neighbor}', 'info')
            values = dumps({self.context.aid: self.unit_values})
            await self.context.send_acl_message(SimulatorMessage(values),
                                                receiver_addr=neighbor[0], receiver_id=neighbor[1],
                                                acl_metadata={'sender_id': self.context.aid},
                                                create_acl=True)

    def get_data(self, outputs):
        data = {}
        for sid, attrs in outputs.items():
            for attr in attrs:
                if attr == 'p_set_point':
                    data[sid] = {attr: list(self.unit_values.values())[0]}
        return data
