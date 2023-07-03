import os
import signal
import socket
import subprocess
import time
from datetime import datetime
import google.protobuf.json_format
from simpy.io.codec import JSON
from termcolor import colored

from cosima_core.messages.message_pb2 import CosimaMsgGroup, InitialMessage, InfoMessage, SynchronisationMessage, \
    InfrastructureMessage, TrafficMessage, AttackMessage
import cosima_core.util.general_config as cfg
import scenario_config


def start_omnet(start_mode, network):
    command = f"./cosima_omnetpp_project -n ../inet4/src/inet -f " \
              f"mosaik.ini -c {network}"
    cwd = str(cfg.ROOT_PATH.parent) + '/cosima_omnetpp_project/'
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


def get_host_names(first_index=0, num_hosts=scenario_config.NUMBER_OF_AGENTS):
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


def make_protobuf_message_for_type(msg_group, message_type, message_dict, message_count):
    if message_type == InitialMessage:
        protobuf_msg = msg_group.initial_messages.add()
    elif message_type == InfoMessage:
        protobuf_msg = msg_group.info_messages.add()
        if 'content' in message_dict.keys():
            message_dict['size'] = len(JSON().encode(message_dict['content']))
        elif 'content_bytes' in message_dict.keys():
            message_dict['size'] = len(message_dict['content_bytes'])
        else:
            raise ValueError("Message has no content!")
        message_count += 1
    elif message_type == InfrastructureMessage:
        protobuf_msg = msg_group.infrastructure_messages.add()
    elif message_type == SynchronisationMessage:
        protobuf_msg = msg_group.synchronisation_messages.add()
    elif message_type == TrafficMessage:
        protobuf_msg = msg_group.traffic_messages.add()
    elif message_type == AttackMessage:
        protobuf_msg = msg_group.attack_messages.add()
    else:
        raise RuntimeError("unknown message type")
    get_protobuf_message_from_dict(message_dict, protobuf_msg)
    return msg_group, message_count


def create_protobuf_messages(messages, current_step):
    """creates protobuf message from given dictionary"""
    msg_groups = list()
    msg_ids = list()
    msg_group = CosimaMsgGroup()
    message_count = 0
    for index, (message_dict, message_type) in enumerate(messages):
        msg_ids.append(message_dict['msg_id'])
        # fill protobuf message group msg_group
        msg_group, message_count = make_protobuf_message_for_type(msg_group, message_type, message_dict, message_count)
        byte_size = msg_group.ByteSize()
        # if size of msg_group exceeds max -> make new msg group, add protobuf message to new message group
        # if size of msg_group is within boundaries -> keep msg_group, add current to list
        if byte_size <= cfg.MAX_BYTE_SIZE_PER_MSG_GROUP - 500:
            if msg_groups:
                msg_groups.pop()
        else:
            msg_group.Clear()
            msg_group, message_count = make_protobuf_message_for_type(msg_group, message_type, message_dict,
                                                                      message_count)
        msg_group_copy = CosimaMsgGroup()
        msg_group.current_mosaik_step = int(current_step)
        msg_group_copy.CopyFrom(msg_group)
        msg_groups.append(msg_group_copy)

    for msg_group in msg_groups:
        msg_group.number_of_message_groups = len(msg_groups)
        if msg_group.ByteSize() > cfg.MAX_BYTE_SIZE_PER_MSG_GROUP:
            raise ValueError(f'Max byte size exceeded. Consider increasing the MAX_BYTE_SIZE_PER_MSG_GROUP.')

    return msg_groups, message_count, msg_ids


def set_up_file_logging():
    f = open(f'{cfg.ROOT_PATH.parent}/log.txt', 'w')
    f.close()


def log(text, log_type='debug'):
    if log_type == 'warning':
        print(datetime.now(), end='')
        print(colored('  | WARNING | mosaik: ', 'red'), end='')
        print(text)
    elif log_type == 'info' and scenario_config.LOGGING_LEVEL == 'info':
        print(datetime.now(), end='')
        print(colored('  | INFO    | mosaik: ', 'blue'), end='')
        print(text)
    elif scenario_config.LOGGING_LEVEL == 'debug':
        print(f'{datetime.now()} | DEBUG   | mosaik: {text}')
    if log_type == 'debug':
        return
    written = False
    while not written:
        try:
            f = open(f'{cfg.ROOT_PATH.parent}/log.txt', 'a')
            f.write(f'{datetime.now()} mosaik: ' + text + '\n')
            written = True
            f.close()
        except:
            pass

