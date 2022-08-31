import os
import signal
import socket
import subprocess
import time
from datetime import datetime

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


def get_agent_names(num_agents=cfg.NUMBER_OF_AGENTS):
    agent_names = []
    for agent_index in range(num_agents):
        agent_names.append(f'client{agent_index}')
    return agent_names


def log(info):
    print(f'mosaik:  {datetime.now().strftime("%H:%M:%S:%f")} {info}')
