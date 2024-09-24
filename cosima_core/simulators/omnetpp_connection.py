import os
import socket
import socketserver
import threading
import time

import cosima_core.util.general_config as cfg
from cosima_core.messages.message_pb2 import CosimaMsgGroup
from cosima_core.util.util_functions import log


class OmnetppConnection:

    def __init__(self, observer_port):
        self.observer_port = observer_port
        self._servername = "127.0.0.1"
        self._listener_port = 4243
        self.stored_messages = []
        self.expected_messages = 0
        self._wait_for_messages = True
        self.server_thread = None
        self._done_receiving = True
        self.current_received_msgs_groups = 0
        self.server = None

    def start_connection(self):
        # Create a new thread to run the server
        while True:
            try:
                self.server = socketserver.TCPServer((self._servername, self._listener_port),
                                                     self.handle_server_connection())
                self.server_thread = threading.Thread(target=self.server.serve_forever)
                self.server_thread.daemon = True
                # Start the server thread
                self.server_thread.start()
                return
            except OSError as e:
                log(f'Retry connecting because of error{e}')
                os.system('fuser -k 4243/tcp')
                time.sleep(5)

    def close_connection(self):
        log('close connection called')
        self._wait_for_messages = False
        try:
            self.server.server_close()
            self.server.shutdown()
        except Exception as e:
            print(f'Exception: {e}')
            pass
        try:
            self.server_thread.join()
        except Exception as e:
            print(f'Exception: {e}')
            pass
        os.system('killall -9 cosima_omnetpp_project')
        os.system('fuser -k 4243/tcp')

    def send_messages(self, messages):
        sender_threads = list()
        for message in messages:
            sender_thread = threading.Thread(target=self.sender, args=(message,), daemon=True)
            sender_threads.append(sender_thread)
            sender_thread.start()

        for index, thread in enumerate(sender_threads):
            thread.join()

    def sender(self, message):
        sender_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
        time.sleep(1)
        for i in range(100):
            if not self._wait_for_messages:
                break
            try:
                sender_socket.connect((self._servername, self.observer_port))
                sender_socket.send(message)
                sender_socket.close()
                break
            except Exception:
                time.sleep(1)


    def return_messages(self):
        time.sleep(0.1)
        if not self._done_receiving:
            return []
        if self.expected_messages > len(self.stored_messages):
            return []
        msgs = self.stored_messages
        self.stored_messages = []
        return msgs

    def process_data(self, data):
        """
        Process received bytes from OMNeT++. Create msgs groups and store data.
        """
        self._done_receiving = False
        if data.count(b'END') == 0:
            raise ValueError('No separation tag in message!')
        data_split = data.split(b'END')
        data = data_split[0]
        try:
            msg_group = CosimaMsgGroup()
            msg_group.ParseFromString(data)
            number_of_msg_groups = msg_group.number_of_message_groups
            self.expected_messages = msg_group.number_of_messages

            self.stored_messages.extend(msg_group.info_messages)
            self.stored_messages.extend(msg_group.infrastructure_messages)
            self.stored_messages.extend(msg_group.synchronisation_messages)

            self.current_received_msgs_groups += 1
            if self.current_received_msgs_groups == number_of_msg_groups \
                    and self.expected_messages == len(self.stored_messages):
                self._done_receiving = True
                self.current_received_msgs_groups = 0
            elif self.current_received_msgs_groups == number_of_msg_groups \
                    and self.expected_messages < len(self.stored_messages):
                print(f'Expected {self.expected_messages}, but received {len(self.stored_messages)}.')
                print('step: ', msg_group.current_mosaik_step)
                self._done_receiving = True
                self.current_received_msgs_groups = 0

        except Exception as other_exception:
            log(other_exception)
            self.close_connection()
            log("Connection to OMNeT++ closed.")

    def handle_server_connection(self):
        outer_class_self = self

        class CosimaTCPHandler(socketserver.BaseRequestHandler):
            """
            The request handler class for our server.
            It is instantiated once per connection to the server, and must
            override the handle() method to implement communication to the
            client.
            """

            def handle(self):
                outer_class_self._done_receiving = False
                prev_data = None
                outer_class_self.current_received_msgs_groups = 0

                while not outer_class_self._done_receiving:
                    # self.request is the TCP socket connected to the client
                    data = self.request.recv(cfg.MAX_BYTE_SIZE_PER_MSG_GROUP)
                    if len(data) == 0:
                        continue
                    else:
                        thread = threading.Thread(target=outer_class_self.process_data(data),
                                                  args=(), daemon=True)
                        thread.start()
                        thread.join()

        return CosimaTCPHandler
