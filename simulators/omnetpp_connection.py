import time

from message_pb2 import CosimaMsgGroup
import asyncio
import threading


class OmnetppConnection:

    def __init__(self, observer_port, client_socket, connection_eids,
                 connections):
        self.observer_port = observer_port
        self.client_socket = client_socket
        self._servername = "127.0.0.1"
        self.connections = connections
        self.connection_eids = connection_eids
        self.stored_messages = []
        self._wait_for_messages = True
        self._closed_due_to_timeout = False
        self.client_socket.connect((self._servername, self.observer_port))
        self.t = threading.Thread(target=self.wait_for_msgs)
        self.t.daemon = False
        self.t.start()

    @property
    def is_still_connected(self):
        return self._wait_for_messages and not self._closed_due_to_timeout

    def close_connection(self):
        self._wait_for_messages = False

    def wait_for_msgs(self):
        while self._wait_for_messages:
            time.sleep(1)
            try:
                data = self.client_socket.recv(4096)
                msg_group = CosimaMsgGroup()
                msg_group.ParseFromString(data)
                for msg in msg_group.msg:
                    self.stored_messages.append(msg)
            except Exception as current_exception:
                self._closed_due_to_timeout = False
                print(current_exception)
                print("Connection to OMNeT++ closed.")
                self.close_connection()

    def send_message(self, msg):
        self._wait_for_messages = False
        try:
            self.client_socket.connect((self._servername, self.observer_port))
        except:
            pass

        self.client_socket.send(msg)
        self._wait_for_messages = True

    def return_messages(self):
        msgs = self.stored_messages
        self.stored_messages = []
        return msgs
