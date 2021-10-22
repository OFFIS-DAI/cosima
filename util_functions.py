import subprocess
import time
import socket


def start_omnet(start_mode, network):
    command = f"./mosaik_omnetpp_observer -n ../inet/src/inet -f mosaik.ini -c {network}"
    if start_mode == 'cmd':
        command = command + " -u Cmdenv"
        subprocess.Popen(command, shell=True, cwd='mosaik_omnetpp_observer/')
    elif start_mode == 'qtenv':
        subprocess.Popen(command, shell=True, cwd='mosaik_omnetpp_observer/')
    elif start_mode == 'ide':
        pass


def check_omnet_connection(config):
    servername = "127.0.0.1"
    observer_port = config['observer_port']
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection_possible = False
    while not connection_possible:
        try:
            client_socket.connect((servername, observer_port))
            # no ConnectionRefusedError
            connection_possible = True
            print('Connection to OMNeT++ possible: ', connection_possible)
            # shutdown connection to not keep it open
            client_socket.shutdown(socket.SHUT_RDWR)
            client_socket.close()
        except ConnectionRefusedError:
            print('Connection to OMNeT++ possible: ', connection_possible)
            time.sleep(1)
            continue
