from typing import Dict, Any
import pandas as pd

from mango import Agent

from cosima_core.util.util_functions import log
from cosima_core.util.general_config import ROOT_PATH

CONTENT_PATH = ROOT_PATH / 'simulators' / 'tic_toc_example' / 'content.csv'


class ActiveAgent(Agent):

    def __init__(self, container, neighbors, aid):
        super().__init__(container, suggested_aid=aid)
        self._neighbors = neighbors
        self._content_to_send = None
        self._content_to_send = pd.read_csv(CONTENT_PATH, delimiter=';', encoding="utf-8-sig")["content"].to_list()

    def start(self):
        log('Starting ActiveAgent.')
        self.schedule_instant_task(self.greet_all_neighbors())

    async def greet_all_neighbors(self):
        log('Greet all neighbors. ')
        for neighbor in self._neighbors:
            log(f'ActiveAgent sends message to {neighbor}')
            await self.context.send_message(receiver_addr=neighbor[0], receiver_id=neighbor[1],
                                               acl_metadata={'sender_id': self.aid},
                                               create_acl=True, content='Lets go through the alphabet!',
                                               )

    def handle_msg(self, content, meta: Dict[str, Any]):
        log(f'ActiveAgent received message with content "{content}"')
        self.schedule_instant_task(
            self.reply_to_msg(receiver_id=meta['sender_id'], receiver_addr=meta['sender_addr'], last_content=content))

    async def reply_to_msg(self, receiver_id, receiver_addr, last_content):
        log(f'ActiveAgent {self.aid} replies to message. ')
        await self.context.send_message(receiver_addr=receiver_addr, receiver_id=receiver_id,
                                           acl_metadata={'sender_id': self.aid},
                                           create_acl=True, content=self.get_next_content(last_content))

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
