# [ICTController](../cosima_core/simulators/tic_toc_project/ict_controller_simulator.py)
The ICT Controller is a mosaik simulator which is responsible for infrastructure changes  of the modelled communication 
topology in OMNeT++. 
Here, a very simple version of such a controller is implemented, but it may be extended easily. 

## Configuration of the infrastructure changes 
The infrastructure changes (disconnecting devices and reconnecting devices) may be configured in the 
[config-file](../cosima_core/config.py). \
Under the attribute `INFRASTRUCTURE_CHANGES` a list of possible disconnects or/and reconnects can be defined. \
For example, if the module of agent client1 should be disconnected at time 2, it would look like that: \
`INFRASTRUCTURE_CHANGES = [
         {'type': 'Disconnect',
          'time': 2,
          'module': 'client1'}
]`\
The `'type'` of an infrastructure change might be `'Disconnect'` or `'Connect'`. \
The `'time` attribute may take any integer value larger than zero. \
The `'module` identifies the cModule in the OMNeT++ network, that is to be dis- or reconnected. That might be an end device, a router or a switch. In the dictionary, the module name of the module in the network should be used.\
The `ÌNFRASTRUCTURE_CHANGES`are a list, therefore multiple dictionaries may also be added.

## Operating Principle
Whenever the ICTController is being initialized as part of a mosaik scenario, it receives the list of the configured 
`ÌNFRASTRUCTURE_CHANGES`. \
At every call of the `step()`-method, the simulator iters over its list and defines the `next_step` - value as the time 
of the next infrastructure change event. \
In the `get_data()`-method of the simulator the corresponding event type (disconnect/ reconnect and module) is extracted 
for the given time. The `output` then contains a CosimaMsgGroup (for further details on message types see 
[Message types](Message%20Types.md)) with the information from the `ÌNFRASTRUCTURE_CHANGES`-list for the given time. \
This output is then given to the CommunicationSimulator (see also [CommSim](CommSim.md)) where its being sent to 
OMNeT++. \
In OMNeT++ the [MosaikScenarioManager](../cosima_omnetpp_project/modules/MosaikScenarioManager.h) handles the change of 
the modelled network. 
