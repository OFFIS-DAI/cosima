# [MosaikScheduler](../cosima_omnetpp_project/modules/MosaikScheduler.h)
The default scheduler cSequentialScheduler in OMNeT++ can be replaced by a custom scheduler implementation (such as a 
real-time scheduler). The custom scheduler class must implement the abstract cScheduler class. For further details on schedulers in OMNeT++ see the [Simulation Manual of OMNeT++](https://doc.omnetpp.org/omnetpp/manual/).  
The MosaikScheduler is the interface to mosaik and therefore, holds the TCP socket connection to python and receives / 
sends messages from/ to the CommunicationSimulator (see [CommunicationSimulator](CommSim.md)) in mosaik.

## Using the *MosaikScheduler* 
If the MosaikScheduler is used as the event scheduler during the simulation, its method ``startRun()`` is called in the
beginning of the simulation in OMNeT++. By that, the listener TCP socket is initialized in order to wait for incoming 
connections from the OmnetppConnection (see [OmnetppConnection](../cosima_core/simulators/core/omnetpp_connection.py)) on mosaik side.

## Operating Principle
As soon as the connection to mosaik is established, messages between OMNeT++ and mosaik can be sent and received. 
The MosaikScheduler must forward messages from mosaik to the correct modules in the OMNeT++ network. 
For this the scheduler needs knowledge about the modules. For this reason, the end devices register at the 
MosaikScheduler at the beginning of the simulation by calling the method `setInterfaceModule()`. That way the scheduler
can keep a list of the registered modules during runtime. \
Whenever the MosaikScheduler receives a message from mosaik (that has to be sent from client0 to client1 for example), 
it must only search for the corresponding module of client0 in its list of the registered modules. \
The representative module of agent client1 then sends the message over the modelled network (e.g. ..[MosaikNetwork](/cosima_omnetpp_project/networks/MosaikNetwork.ned)) in OMNeT++. \
As soon as client1 receives the message, it forwards that information back to the MosaikScheduler, which then sends 
a message back to the CommunicationSimulator in mosaik. \
In addition to normal messages, control messages can also be sent by mosaik. The different message types are explained 
in [Message Types](Message%20Types.md). \
If the MosaikScheduler receives a Waitingmessage, that means that mosaik is waiting for messages in OMNeT++ to be 
simulated and the simulation is being continued. \
If the MosaikScheduler receives a message from the ICT-Controller-Agent the infrastructure of the simulation network has 
to be changed. These infrastructure changes (e.g., disconnect and reconnect of router, switches clients) are then conducted by the [MosaikScenarioManager](../cosima_omnetpp_project/modules/MosaikScenarioManager.h). \
Until and maxAdvance information (for further details see [Synchronisation](Synchronization.md)) is handled by the 
[MosaikSchedulerModule](../cosima_omnetpp_project/modules/MosaikSchedulerModule.h) (for further details see below). 

## The *[MosaikSchedulerModule](../cosima_omnetpp_project/modules/MosaikSchedulerModule.h)*
The MosaikSchedulerModule is a cModule that works as a helper module for the MosaikScheduler, because a cModule is needed in OMNeT++ for event scheduling.
Whenever the MosaikScheduler receives information about maxAdvance or until values from mosaik, that information is 
transferred into events at the given time. These events are then scheduled for the MosaikSchedulerModule. 

## Scheduling tasks
As the used scheduler in the simulation, the MosaikScheduler also has to take over the scheduling tasks of the default 
event scheduler. \
Therefore, the method `takeNextEvent()`is implemented. This method is by default called by the simulation kernel in 
OMNeT++ whenever the next event from the Future Event Set (FES) (for further information on the concept of the FES in OMNeT++ see [manual](https://doc.omnetpp.org/omnetpp/manual/#sec:simple-modules:event-loop)) might be taken for execution. As soon as the initial 
message from mosaik has arrived at the MosaikScheduler, the scheduler takes the events in the scheduled order from the FES. Additional to that the method 
`receiveUntil()` is called here. In `receiveUntil()` the MosaikScheduler waits for messages from mosaik and inserts the received messages as events into the FES. 

 
