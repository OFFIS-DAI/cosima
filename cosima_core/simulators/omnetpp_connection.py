import sys
import time

from socket import SHUT_RDWR
import google.protobuf.message

from cosima_core.messages.message_pb2 import CosimaMsgGroup
import threading

from cosima_core.util.util_functions import log


class OmnetppConnection:

    def __init__(self, observer_port, client_socket):
        self.observer_port = observer_port
        self.client_socket = client_socket
        self._servername = "127.0.0.1"
        self.stored_messages = []
        self._wait_for_messages = True
        self._closed_due_to_timeout = False
        self.omnet_thread = None

    def start_connection(self):
        self.client_socket.connect((self._servername, self.observer_port))
        self.omnet_thread = threading.Thread(target=self.wait_for_msgs)
        self.omnet_thread.daemon = False
        self.omnet_thread.start()

    @property
    def is_still_connected(self):
        return self._wait_for_messages and not self._closed_due_to_timeout

    def close_connection(self):
        log('close connection called')
        self._wait_for_messages = False
        try:
            self.client_socket.shutdown(SHUT_RDWR)
            self.client_socket.close()
        except Exception as e:
            print(f'Exception: {e}')
            pass
        try:
            self.omnet_thread.join()
        except Exception as e:
            print(f'Exception: {e}')
            pass

    def wait_for_msgs(self):
        while self._wait_for_messages:
            time.sleep(1)
            try:
                data = self.client_socket.recv(4096)
            except OSError:
                data = ''
            while data != '':
                try:
                    msg_group = CosimaMsgGroup()
                    msg_group.ParseFromString(data)
                    data = ''
                    for msg in msg_group.info_messages:
                        self.stored_messages.append(msg)
                    for msg in msg_group.infrastructure_messages:
                        self.stored_messages.append(msg)
                    for msg in msg_group.synchronisation_messages:
                        self.stored_messages.append(msg)
                except google.protobuf.message.DecodeError:
                    data += self.client_socket.recv(4096)
                except Exception as other_exception:
                    log(other_exception)
                    self._closed_due_to_timeout = False
                    log("Connection to OMNeT++ closed.")
                    self.close_connection()

    def send_message(self, msg):
        try:
            self.client_socket.connect((self._servername, self.observer_port))
        except Exception:
            pass

        self.client_socket.send(msg)

    def return_messages(self):
        msgs = self.stored_messages
        self.stored_messages = []
        return msgs
