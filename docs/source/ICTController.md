# ICTController

The [ICT Controller](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_core/simulators/ict_controller_simulator.py)
is a mosaik simulator which is responsible for infrastructure changes of the modelled communication topology in OMNeT++.
Here, a very simple version of such a controller is implemented, but it may be extended easily.

## Configuration of the infrastructure changes

The infrastructure changes (disconnecting devices and reconnecting devices) may be configured in the
[scenario configuration file](https://github.com/OFFIS-cosima/cosima/blob/master/scenario_config.py). \
Under the attribute `INFRASTRUCTURE_CHANGES` a list of possible disconnects or/and reconnects can be defined. \
For example, if the module of agent client1 should be disconnected at time 2, it would look like that:

```
INFRASTRUCTURE_CHANGES = [
         {'type': 'Disconnect',
          'start': 2,
          'module': 'client1'}
]
```

The `'type'` of an infrastructure change might be `'Disconnect'` or `'Connect'`. \
The `'start` attribute may take any integer value larger than zero. \
The `'module` identifies the cModule in the OMNeT++ network, that is to be dis- or reconnected. That might be an end
device, a router or a switch. In the dictionary, the module name of the module in the network should be used.\
The `ÌNFRASTRUCTURE_CHANGES`are a list, therefore multiple dictionaries may also be added.

## Configuration of cyberattacks

Modeling cyberattacks in energy system simulations is vital to assess vulnerabilities, strengthen security measures, and
ensure the reliable and resilient operation of critical energy infrastructure in the face of emerging cyber threats.
Therefore, cosima offers the possibility to easily add cyberattacks to the scenario. The cyberattacks may also be
configured in the
[scenario configuration file](https://github.com/OFFIS-cosima/cosima/blob/master/scenario_config.py). \
Under the attribute `ATTACK_CONFIGURATION` a list of possible attacks. There are three types of attacks that can be
modeled out-of-the-box using cosima:

- **PacketDrop** packets are dropped and will not be received by the receiving module with a given probability
- **PacketFalsification** packets are marked as falsified with a given probability
- **PacketDelay** packets are delayed with a given probability For example, if the packets at client1 should be delayed
  with a probability of 0.5 between time 2 and 10, it would look like that:

```
ATTACK_CONFIGURATION = [
         {'type': 'PacketDelay',
          'start': 2,
          'stop': 10,
          'attacked_module': 'client1',
          'probability': 0.5}
]
```

As the modules on OMNeT++-side need a specific configuration in order to realise attacks, you need to use special
network with modules of the type `Attacked_Ue`. An example may be found in
the [network folder](https://gitlab.com/mosaik/examples/cosima/-/tree/master/cosima_omnetpp_project/networks).

## Configuration of increased traffic

Modeling increased network traffic in communication simulation for energy systems is essential because it enables us to
understand and optimize the dynamic interactions between various components, uncover potential bottlenecks, and devise
robust strategies to ensure seamless communication and efficient energy management. Therefore, cosima also enables the
integration of increased network traffic in user-defined scenarios. The integration can also be done using the
[scenario configuration file](https://github.com/OFFIS-cosima/cosima/blob/master/scenario_config.py). An attack
configuration may look like this:

```
TRAFFIC_CONFIGURATION = [
   {
         'source': 'traffic_device_0',
         'destination': 'client1',
         'start': 1,
         'stop': 100,
         'interval': 10,
         'packet_length': 1000
     }
 ]
```
In this example, the end device called traffic_device_0 sends additional traffic to client 1 from time 1 to time 100 in
an interval of 10ms and with packets of length 1000 Bytes. 

## Operating Principle

Whenever the ICTController is being initialized as part of a mosaik scenario, it receives the lists of the configured
`ÌNFRASTRUCTURE_CHANGES`, `TRAFFIC_CONFIGURATION` and `ATTACK_CONFIGURATION`. \
At every call of the `step()`-method, the simulator iters over its list of events and defines the `next_step` - value as
the time of the next event. \
In the `get_data()`-method of the simulator the corresponding event type is extracted for the given time. The `output`
then contains a CosimaMsgGroup (for further details on message types see
[Message types](MessageTypes.md)) with the information from the event-list for the given time. \
This output is then given to the CommunicationSimulator (see also [CommunicationSimulator](CommunicationSimulator.md))
where its being sent to OMNeT++. \
In OMNeT++
the [MosaikScenarioManager](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/modules/MosaikScenarioManager.h)
handles the change of the modelled network. 
