import asyncio
import datetime
import math
from enum import Enum

from mango.container.external_coupling import ExternalSchedulingContainer
from typing import Dict

import scenario_config
from cosima_core.messages.message_pb2 import InfoMessage, InitialMessage, SynchronisationMessage
from cosima_core.simulators.omnetpp_connection import OmnetppConnection
from cosima_core.util.general_config import MAX_BYTE_SIZE_PER_MSG_GROUP, MANGO_CONVERSION_FACTOR
from cosima_core.util.util_functions import create_protobuf_messages, check_omnet_connection, start_omnet, \
    get_dict_from_protobuf_message

logging_level = 'info'


class MangoCommunicationNetwork:
    """
       Represents a communication network based on a simulated network in OMNeT++ using the agent framework mango.

       This class sets up the communication network, manages message passing between components,
       and interacts with an OMNeT++ simulation.

       Args:
           client_container_mapping (Dict[str, MosaikContainer]): A mapping of client names (in OMNeT++) to
           MosaikContainer instances (mango).
           port (int): The port to establish socket communication with OMNeT++.

       Attributes:
           _client_container_mapping (Dict[str, MosaikContainer]): A mapping of client names (in OMNeT++) to
           MosaikContainer instances (mango).
           _next_activities (list): List of next activity timestamps from agents in mango containers.
           _current_time (int): Current simulation time in milliseconds.
           _loop (asyncio.AbstractEventLoop): The asyncio event loop.
           _message_buffer (list): Buffer to store messages to be sent.
           _msg_counter (int): Counter for message IDs.
           _waiting_msgs_counter (int): Counter for waiting message IDs.
           omnetpp_connection (OmnetppConnection): Connection to the OMNeT++ simulation.
           omnet_process (subprocess.Popen): Process running the OMNeT++ simulation.
           _sent_msgs_ids (list): List of sent message IDs.
           _number_of_messages_sent (int): Count of sent messages.
           _number_of_messages_received (int): Count of received messages.
       """

    class ClientStatus(Enum):
        CONNECTED = 0
        DISCONNECTED = 1

    def __init__(self, client_container_mapping: Dict[str, ExternalSchedulingContainer], port: int,
                 duration_s: int, start_timestamp=0.0, start_mode='cmd', network='SimbenchNetwork',
                 results_recorder=None, traffic_configuration=None,
                 infrastructure_changes=None):
        """
            Initialize the MangoCommunicationNetwork instance.

            This method sets up the necessary attributes.

            Args:
                client_container_mapping (Dict[str, MosaikContainer]): A mapping of client names (in OMNeT++) to
                MosaikContainer instances (mango).
                port (int): The port to establish socket communication with OMNeT++.
        """
        if traffic_configuration is None:
            traffic_configuration = []
        if infrastructure_changes is None:
            infrastructure_changes = []
        self._start_time = start_timestamp
        print('duration in seconds: ', duration_s)
        self._simulation_end_time = start_timestamp + duration_s
        print('Simulation start time: ', datetime.datetime.fromtimestamp(self._start_time))
        print('Simulation end time: ', datetime.datetime.fromtimestamp(self._simulation_end_time))
        self._client_container_mapping = client_container_mapping
        self._next_activities = list()
        self._current_time = 0
        self._loop = asyncio.get_running_loop()
        self._loop.create_task(self._start_scenario())
        self._message_buffer = list()
        self._msg_counter = 0
        self._waiting_msgs_counter = 0
        self.omnetpp_connection = OmnetppConnection(observer_port=port)
        self.omnet_process = start_omnet(start_mode, network)
        check_omnet_connection(port)
        self.omnetpp_connection.start_connection()
        self._sent_msgs_ids = list()
        self._number_of_messages_sent = 0
        self._number_of_messages_received = 0
        self._traffic_configurations = traffic_configuration
        self._infrastructure_changes = infrastructure_changes
        self.scenario_finished = asyncio.Future()

        self._client_status = {client_name: self.ClientStatus.CONNECTED
                               for client_name in client_container_mapping.keys()}
        self.update_client_status()

        self.results_recorder = results_recorder

    async def _start_scenario(self):
        """
            Start the communication network in the agent-system scenario.

            This method initializes the scenario by sending an initial message to OMNeT++,
            performing the initial steps, and running the scenario loop.
        """
        initial_msg = {
            'msg_id': 'InitialMessage',
            'max_advance': self._simulation_end_time - self._start_time,
            'until': self._simulation_end_time - self._start_time,
            'stepSize': 1000,
            'logging_level': logging_level,
            'max_byte_size_per_msg_group': MAX_BYTE_SIZE_PER_MSG_GROUP
        }
        scenario_config.LOGGING_LEVEL = logging_level
        self._message_buffer.append((initial_msg, InitialMessage))
        for traffic_config in self._traffic_configurations:
            msg_dispatch_time = 0 if 'start' not in traffic_config else math.ceil(traffic_config['start'])
            msg_end_time = int(self._simulation_end_time - self._start_time) if 'stop' not in traffic_config else traffic_config['stop']
            source = traffic_config['source']
            destination = traffic_config['destination']
            while msg_dispatch_time < msg_end_time:
                msg_id = f'TrafficMessage_{source}_{self._msg_counter}'
                message_dict = {'msg_id': msg_id,
                                'max_advance': self._simulation_end_time - self._start_time,
                                'sim_time': msg_dispatch_time,
                                'sender': source,
                                'receiver': destination,
                                'content': 't' * traffic_config['packet_length_B'],
                                'creation_time': msg_dispatch_time,
                                }
                self._msg_counter += 1
                self._message_buffer.append((message_dict, InfoMessage))
                self.results_recorder.add_comm_results(msg_id=msg_id,
                                                       time_send=msg_dispatch_time,
                                                       time_receive=None,
                                                       sender=source,
                                                       receiver=destination,
                                                       content=f'traffic with {traffic_config["packet_length_B"]} Bytes')
                msg_dispatch_time += traffic_config['interval_ms']
        for container_name, container in self._client_container_mapping.items():
            output = await container.step(simulation_time=self._start_time, incoming_messages=[])
            self.process_mango_outputs(container_name, output)
        await self.send_messages_to_omnetpp()
        await self.run_scenario()

    def update_client_status(self):
        # Create a new list for infrastructure changes that have not been processed yet
        remaining_changes = []

        for change in self._infrastructure_changes:
            # Check if the event's simulation time has passed or is at the current time
            if self._current_time >= (self._start_time + change['simulation_time_ms']):
                # Get the type of change and the module to change
                module = change['change_module']
                if change['type'] == 'disconnect':
                    # Disconnect the module
                    self._client_status[module] = self.ClientStatus.DISCONNECTED
                elif change['type'] == 'connect':
                    # Reconnect the module
                    self._client_status[module] = self.ClientStatus.CONNECTED
            else:
                # If the change hasn't been processed yet, keep it in the list
                remaining_changes.append(change)

        # Update the infrastructure changes to only include unprocessed changes
        self._infrastructure_changes = remaining_changes

    async def run_scenario(self):
        """
            Run the communication network simulation scenario loop.

            This method continuously runs the scenario loop, processing received messages,
            performing container steps, handling the synchronization and sending messages to OMNeT++.
        """
        while True:
            if self._current_time > self._simulation_end_time:
                print('--Simulation end, reason: exceeded the simulation time')
                return
            while not self.waiting_for_messages_from_omnetpp() \
                    and len(self._next_activities) > 0:
                # we are not waiting for messages from OMNeT++ and have no next activities in mango
                # therefore, we advance in time in mango
                smallest_next_activity = min(self._next_activities)
                self._current_time = smallest_next_activity
                self.update_client_status()
                if self._current_time > self._simulation_end_time:
                    print('--Simulation end, reason: exceeded the simulation time')
                    return
                await self.step_mango_containers(received_messages=[])
                if len(self._message_buffer) == 0:
                    # no messages occurred in mango
                    if len(self._next_activities) == 0:
                        # no next activities
                        # therefore, simulation is finished
                        print('--Simulation end, reason: no messages in buffer and no next activities')
                        return
                    # new next activities in mango
                    # therefore, continue advancing in time in mango
                    continue
                else:
                    # messages occurred in mango
                    # therefore, send messages to OMNeT++
                    await self.send_messages_to_omnetpp()
            # receive and process messages from OMNeT++
            received_messages = self.omnetpp_connection.return_messages()
            if len(received_messages) != 0:
                received_info_msgs = [msg for msg in received_messages if type(msg) == InfoMessage]
                self._number_of_messages_received += len(received_info_msgs)
                self._current_time = received_messages[0].sim_time + self._start_time
                self.update_client_status()

                # perform container steps in mango
                await self.step_mango_containers(received_messages=received_messages)

                # handle synchronization
                await self.handle_synchronization_with_waiting_messages()

                # send messages to OMNeT++
                await self.send_messages_to_omnetpp()

    async def step_mango_containers(self, received_messages: list):
        """
            Step mango containers, process received outputs, and update next activities.

            This method iterates over the containers in the network, processes received
            messages for each container, and updates the next activities list accordingly.

            Args:
                received_messages (list): A list of received messages (from OMNeT++) to process (in mango).

            Returns:
                None
        """
        for container_name, container in self._client_container_mapping.items():
            if self._client_status.get(container_name) == self.ClientStatus.CONNECTED:
                if self.results_recorder:
                    for message in received_messages:
                        if type(message) == InfoMessage:
                            self.results_recorder.update_received_message(msg_id=message.msg_id,
                                                                          time_receive=message.sim_time)
                received_messages_for_container = [str.encode(get_dict_from_protobuf_message(message)['content'])
                                                   for message in received_messages
                                                   if type(message) == InfoMessage
                                                   and 'Traffic' not in message.msg_id
                                                   and message.receiver == container_name]
            else:
                print(container_name, ' not connected. Discard incoming messages. ')
                received_messages_for_container = []
            output = await container.step(simulation_time=self._current_time,
                                          incoming_messages=received_messages_for_container)
            self.process_mango_outputs(container_name, output)
        self._next_activities = [n_a for n_a in self._next_activities if n_a > self._current_time]

    def process_mango_outputs(self, container_name, output):
        """
            Process the mango outputs from a container.

            This method processes the output from a MosaikContainer, generates message
            dictionaries, and adds them to the message buffer.

            Args:
                container_name (str): Name of the container.
                output: Output received from the MosaikContainer's step.
        """
        if output.next_activity is None:
            next_activity = self._simulation_end_time
        else:
            next_activity = math.ceil(output.next_activity * MANGO_CONVERSION_FACTOR)
            self._next_activities.append(next_activity)
        for mango_output in output.messages:
            msg_output_time = math.ceil(mango_output.time * MANGO_CONVERSION_FACTOR)
            if self._client_status.get(container_name) == self.ClientStatus.CONNECTED:
                msg_id = f'AgentMessage_{container_name}_{self._msg_counter}'
                message_dict = {'msg_id': msg_id,
                                'max_advance': next_activity - self._start_time,
                                'sim_time': msg_output_time - self._start_time,
                                'sender': container_name,
                                'receiver': mango_output.receiver,
                                'content': mango_output.message.decode(),
                                'creation_time': msg_output_time - self._start_time,
                                }
                self.results_recorder.add_comm_results(msg_id=msg_id,
                                                       time_send=msg_output_time - self._start_time,
                                                       time_receive=None,
                                                       sender=container_name,
                                                       receiver=mango_output.receiver,
                                                       content=mango_output.message.decode())
                self._message_buffer.append((message_dict, InfoMessage))
                self._msg_counter += 1
            else:
                print(container_name, ' is not connected. Message will not be sent at time ',
                      msg_output_time - self._start_time)

    async def handle_synchronization_with_waiting_messages(self):
        """
            Handle synchronization with OMNeT++ by sending waiting message if conditions are met.

            This method checks if certain conditions are met for sending a waiting message
            to synchronize the simulation components.

            The waiting message includes synchronization information for the simulation.

            This method does not return any value.
        """
        if self.waiting_for_messages_from_omnetpp() and \
                len(self._next_activities) == 0 and len(self._message_buffer) == 0:
            # there are still events in OMNeT++, but no further events in mango
            # therefore, OMNeT++ may simulate until the next event occurs
            waiting_msg = {
                'msg_type': SynchronisationMessage.MsgType.WAITING,
                'msg_id': f'WaitingMessage_{self._waiting_msgs_counter}',
                'sim_time': self._current_time - self._start_time,
                'max_advance': self._simulation_end_time,
            }
            self._waiting_msgs_counter += 1
            self._message_buffer.append((waiting_msg, SynchronisationMessage))
        elif self.waiting_for_messages_from_omnetpp() and \
                len(self._next_activities) != 0 and len(self._message_buffer) == 0:
            # there are still events in OMNeT++ and also possible next activities of the agents in mango
            # therefore, OMNeT++ may simulate until the next event occurs in OMNeT++ or an agents has its next activity
            smallest_next_activity = min(self._next_activities)
            waiting_msg = {
                'msg_type': SynchronisationMessage.MsgType.WAITING,
                'msg_id': f'WaitingMessage_{self._waiting_msgs_counter}',
                'sim_time': self._current_time - self._start_time,
                'max_advance': smallest_next_activity - self._start_time,
            }
            self._waiting_msgs_counter += 1
            self._message_buffer.append((waiting_msg, SynchronisationMessage))

    def waiting_for_messages_from_omnetpp(self):
        """
            Check if we are waiting for messages from OMNeT++.

            This method compares the number of messages sent with the number of messages
            received to determine if there are still messages to expect from OMNeT++.

            Returns:
                bool: True if we are waiting for messages, False otherwise.
        """
        return self._number_of_messages_sent != self._number_of_messages_received

    async def send_messages_to_omnetpp(self):
        """
            Send messages from the message buffer to OMNeT++.

            This method sends messages from the message buffer to the OMNeT++ simulation.
            It also handles synchronization messages if required.
        """
        proto_messages, message_count, msg_ids = create_protobuf_messages(self._message_buffer, self._current_time)
        self._number_of_messages_sent += message_count
        equal_entries = [entry for entry in msg_ids if entry in self._sent_msgs_ids]
        if len(equal_entries) != 0:
            raise Exception(f'Message ID {equal_entries} has already been sent! Please use unique ids.')
        self._sent_msgs_ids.extend(msg_ids)
        serialized_messages = list()
        for message in proto_messages:
            serialized_messages.append(message.SerializeToString())
        self.omnetpp_connection.send_messages(serialized_messages)
        self._message_buffer.clear()
        return message_count
