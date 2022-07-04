import os
import signal
import socket
import subprocess
import time
from datetime import datetime
import google.protobuf.json_format
from simpy.io.codec import JSON

from cosima_core.messages.message_pb2 import CosimaMsgGroup, InitialMessage, InfoMessage, SynchronisationMessage, \
    InfrastructureMessage
import cosima_core.config as cfg


def start_omnet(start_mode, network):
    command = f"./cosima_omnetpp_project -n ../inet/src/inet -f " \
              f"mosaik.ini -c {network}"
    cwd = '../cosima_omnetpp_project/'
    omnet_process = None
    if start_mode == 'cmd':
        command = command + " -u Cmdenv"

    if start_mode != 'ide':
        if cfg.VERBOSE:
            omnet_process = subprocess.Popen(command,
                                             preexec_fn=os.setsid, shell=True,
                                             cwd=cwd)
        else:
            omnet_process = subprocess.Popen("exec " + command,
                                             stdout=subprocess.PIPE,
                                             preexec_fn=os.setsid, shell=True,
                                             cwd=cwd)
    return omnet_process


def stop_omnet(omnet_process):
    log("stop omnet process")
    if omnet_process:
        # omnet_process.kill()
        os.killpg(os.getpgid(omnet_process.pid), signal.SIGTERM)


def check_omnet_connection(port):
    servername = "127.0.0.1"
    observer_port = port
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection_possible = False
    while not connection_possible:
        try:
            client_socket.connect((servername, observer_port))
            # no ConnectionRefusedError
            connection_possible = True
            log(f'Connection to OMNeT++ possible: {connection_possible}')
            # shutdown connection to not keep it open
            client_socket.shutdown(socket.SHUT_RDWR)
            client_socket.close()
        except ConnectionRefusedError:
            log(f'Connection to OMNeT++ possible: {connection_possible}')
            time.sleep(1)
            continue


def get_host_names(first_index=0, num_hosts=cfg.NUMBER_OF_AGENTS):
    host_names = []
    for index in range(first_index, num_hosts):
        host_names.append(f'client{index}')
    return host_names


def get_protobuf_message_from_dict(dictionary, message):
    protobuf = google.protobuf.json_format.ParseDict(dictionary, message, ignore_unknown_fields=False,
                                                     descriptor_pool=None)
    return protobuf


def get_dict_from_protobuf_message(protobuf_message):
    return google.protobuf.json_format.MessageToDict(protobuf_message, preserving_proto_field_name=True)


def create_protobuf_msg(messages, current_step):
    """creates protobuf message from given dictionary"""
    msg_group = CosimaMsgGroup()
    msg_group.current_mosaik_step = current_step
    message_count = 0
    for message_dict, message_type in messages:
        if message_type == InitialMessage:
            protobuf_msg = msg_group.initial_messages.add()
        elif message_type == InfoMessage:
            protobuf_msg = msg_group.info_messages.add()
            message_dict['size'] = len(JSON().encode(message_dict))
            message_count += 1
        elif message_type == InfrastructureMessage:
            protobuf_msg = msg_group.infrastructure_messages.add()
        elif message_type == SynchronisationMessage:
            protobuf_msg = msg_group.synchronisation_messages.add()
        else:
            raise RuntimeError("unknown message type")
        get_protobuf_message_from_dict(message_dict, protobuf_msg)
    return msg_group, message_count


def log(info):
    print(f'mosaik:  {datetime.now().strftime("%H:%M:%S:%f")} {info}')
