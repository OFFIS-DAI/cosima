# Scenarios 
It is possible to simulate different scenarios. In the 
following, the parameters to configure these scenarios are
described. The parameters are set in the [config file](../cosima_core/config.py).

## General Conditions
For the general conditions of the scenarios, it is possible to adapt the
**step size** of the simulation. This parameter is set in milliseconds and
determines the size of one mosaik step. For example, a step size of 10 would
mean that one mosaik step refers to 10 milliseconds. This also influences 
the time in OMNeT++, since the configuration of the step size will also be 
taken into acoount on this side.
Furthermore, it is possible to set the duration of the simulation by setting
the end of the simulation (**until** in mosaik, see: [Explanation of until](Synchronization.md)).

## Networks
To create a scenario, a network to simulate needs to be chosen. See a detailed
[description of the provided networks](Networks.md). It is possible to 
simulate UDP and TCP networks. 
The network files can be found in the 
[networks folder](../cosima_omnetpp_project/networks). 

### Dynamic configurations
It is possible to do some infrastructure changes for the simulated network.
In concrete terms that means that some elements (clients, routers and switches) 
can be disconnected and reconnected. For this, only the time and the selected element must be 
specified. It is possible to reconnect and connect the same element multiple times. The dynamic
(re)configurations can be integrated by the [ICT Controller](ICTController.md).

## Configuration of agents
To choose a configuration for the agents in the scenario, the **number of agents** to be simulated can be set.
The number of agents depends on the used network. Currently, networks for 2-20 agents are stored. It is possible 
to extend these or use your own networks (see [Networks](Networks.md)).
Thus, it is possible to have a **parallel** setting or not. If this is not the case, only one agent starts
to interact with others by sending messages to which they then respond.
With parallel setting, a second agent begins an interaction with other 
agents by sending them a message one step later than the first 
agent. Therefore, if parallel sending is simulated, two agents send messages
simultaneously and time-shifted with 1 time step difference. 

Additionally, it is possible to consider **PV plants** in the scenario. A 
simulated PV plant is given a csv-file with power values for a given time.
During the simulation, the PV plant always takes the current power value from this
list according to the current simulation time and forwards it to the agent via
mosaik. The associated agent responds with an acknowledgement that is sent
back to the plant via mosaik. At this point, the agent could make adjustments 
or take control measures for the plant. Each simulated agent can be connected to 
a PV plant.


Furthermore, some calculation times for the agents can be simulated. In that case,
the agents would not send the answer directly to other agents, but first wait 
for the given calculation time and then send their answer. This affects subsequent
responses from other agents, as they receive the message to respond at a later time.
Thus, changes in the simulation due to higher 
calculation times can be investigated. 
