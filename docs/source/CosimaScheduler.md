# CosimaScheduler
The default scheduler cSequentialScheduler in OMNeT++ can be replaced by a custom scheduler implementation (such as a 
real-time scheduler). The custom scheduler class must implement the abstract cScheduler class. For further details on 
schedulers in OMNeT++ see the [Simulation Manual of OMNeT++](https://doc.omnetpp.org/omnetpp/manual/).  
The CosimaScheduler is the interface to mosaik and therefore, holds the TCP socket connection to python and receives / 
sends messages from/ to the CommunicationSimulator (see [CommunicationSimulator](CommunicationSimulator.md)) in mosaik.

## Using the CosimaScheduler
If the CosimaScheduler is used as the event scheduler during the simulation, its method ``startRun()`` is called in the
beginning of the simulation in OMNeT++. By that, the listener TCP socket is initialized in order to wait for incoming 
connections from the OmnetppConnection (see [OmnetppConnection](../../cosima_core/simulators/omnetpp_connection.py)) on mosaik side.

## Operating Principle
As soon as the connection to mosaik is established, messages between OMNeT++ and mosaik can be sent and received. 
The CosimaScheduler must forward messages from mosaik to the correct modules in the OMNeT++ network. 
For this the scheduler needs knowledge about the modules. For this reason, the end devices register at the 
CosimaScheduler at the beginning of the simulation by calling the method `setInterfaceModule()`. That way the scheduler
can keep a list of the registered modules during runtime. \
Whenever the CosimaScheduler receives a message from mosaik (that has to be sent from client0 to client1 for example), 
it must only search for the corresponding module of client0 in its list of the registered modules. \
The representative module of agent client1 then sends the message over the modelled network in OMNeT++. \
As soon as client1 receives the message, it forwards that information back to the CosimaScheduler, which then sends 
a message back to the CommunicationSimulator in mosaik. \
In addition to normal messages, control messages can also be sent by mosaik. The different message types are explained 
in [Message Types](MessageTypes.md). \
If the CosimaScheduler receives a `SynchronisationMessage` of the type `WAITING`, 
that means that the coupled simulation (mosaik or mango) is waiting for messages in OMNeT++ to be 
simulated and the simulation is being continued. \
If the CosimaScheduler receives a message from the ICT-Controller the infrastructure of the simulation network has 
to be changed. These infrastructure changes (e.g., disconnect and reconnect of router, switches clients) are then 
conducted by the [CosimaScenarioManager](../../cosima_omnetpp_project/modules/CosimaScenarioManager.h). \
Until and maxAdvance information (for further details see [Synchronisation](Synchronization.md)) is handled by the 
[CosimaSchedulerModule](../../cosima_omnetpp_project/modules/CosimaSchedulerModule.h) (for further details see below). 

## The CosimaSchedulerModule
The [CosimaSchedulerModule](../../cosima_omnetpp_project/modules/CosimaSchedulerModule.h) is a cModule that works as a helper module for the CosimaScheduler, because a cModule is needed in OMNeT++ for event scheduling.
Whenever the CosimaScheduler receives information about maxAdvance or until values from mosaik, that information is 
transferred into events at the given time. These events are then scheduled for the CosimaSchedulerModule. 

## Scheduling tasks
As the used scheduler in the simulation, the CosimaScheduler also has to take over the scheduling tasks of the default 
event scheduler. \
Therefore, the method `takeNextEvent()`is implemented. This method is by default called by the simulation kernel in 
OMNeT++ whenever the next event from the Future Event Set (FES) (for further information on the concept of the FES in OMNeT++ see [manual](https://doc.omnetpp.org/omnetpp/manual/#sec:simple-modules:event-loop)) might be taken for execution. As soon as the initial 
message from mosaik has arrived at the CosimaScheduler, the scheduler takes the events in the scheduled order from the FES. Additional to that the method 
`receiveUntil()` is called here. In `receiveUntil()` the CosimaScheduler waits for messages from mosaik and inserts the received messages as events into the FES. 

 
