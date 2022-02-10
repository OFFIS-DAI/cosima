import comm_scenario
import time

from util_functions import log

number_of_agents = [2, 3]
omnet_networks = ["EventBasedWinzentTCP", "EventBasedWinzentUDP",
                   "EventBasedUDP_SimulaneousMessageReceiving",
                   "EventBasedWinzentTCP_disconnected_client2",
                   "EventBasedWinzentTCP_disconnected_client1",
                   "EventBasedWinzentUDP_disconnected_client2",
                   "EventBasedWinzentUDP_disconnected_client1"]
parallel = [False, True]
sim_end = 1000

for a in number_of_agents:
    for p in parallel:
        for n in omnet_networks:
            log(f'Simulate experiment with {a} agents, omnet network {n} and parallel= {p}')
            comm_scenario.main(a, n, p, sim_end)
            time.sleep(5)
            print("\n \n \n \n \n")
