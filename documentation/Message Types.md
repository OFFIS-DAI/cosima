# Message Types
There are different message types used in order to exchange information in this project. \
In mosaik normal python-dictionaries are used for message exchange between the simulators. \
For information exchange between mosaik and OMNeT++ Protocol buffers (in the following called Protobuf) are used and described in detail below. \
For information exchange in OMNeT++ .msg-files are used and also described below.

## Protocol buffers 
[Protocol buffers](https://developers.google.com/protocol-buffers/docs/overview) provide an easy way of serializing 
data. Here, a [Proto file](../cosima_core/messages/message.proto) is used in order to define the different Protocol buffers. \
A Protobuf message contains a `CosimaMsgGroup`, which is a list of `CosimaMsg` objects, in order to send multiple messages at once / in one time step. A `CosimaMsgGroup` also contains 
information about the current time step in mosaik. \
A `CosimaMsg` has a type, which is an enum. \
A message of the type `CMD` is a control message. It is used for the initial message from mosaik to OMNeT++, but may 
be used for any control information. \
A message of the type `INFO` contains the information about a message, i.e., the actual content of the mesage, sent between simulators in mosaik.\
A message of the type `MAX_ADVANCE` is sent from OMNeT++ to mosaik in order to confirm the [CommSim](CommSim.md) about 
a received max-advance-event. That means, that mosaik now may send new messages to OMNeT++. For further details on 
synchronisation see [Synchronisation](Synchronization.md). \
A message of the type `WAITING` is sent from mosaik to OMNeT++ to inform OMNeT++ that mosaik is not proceeding further in the simulation, as it is waiting for messages from OMNeT++. \
A message of the type `TRANSMISSION_ERROR` is sent from OMNeT++ to mosaik whenever there was a transmission error in the 
network when transmitting a message between the simulator-modules. This can happen, if a module has been disconnected 
before. \
For disconnects and reconnects the message types `DISCONNECT` and `RECONNECT`are used. \
In addition to the message type, a `CosimaMsg` also contains other attributes. 
The use of these attributes depends on the type of the message and therefore, some attributes may not be assigned in each case. An `INFO` message for example
should contain information about the sender and the receiver of the message. If it's sent from mosaik to OMNeT++ it 
should also contain the current max advance info, because that information is used in OMNeT++. \
A brief explanation of the attributes:
- `string sender` : the sender of an `INFO` message, should be the name of the module in OMNeT++ 
- `string receiver` : the receiver of an `INFO` message, should be the name of the module in OMNeT++  
- `int32 max_advance` : the current value of the mosaik-internal max advance value
- `int32 size`: the size of the (`INFO`) message in bytes (not containing the fields just used for synchronisation, e.g., `simTime`, `stepSize`...)
- `string content` : the content of the (`INFO`) message
- `double delay` : the simulated end-to-end delay in the OMNeT++ network
- `int32 simTime`: the simulation time in OMNeT++
- `int32 stepSize` : the step size used in the simulation, based on the step-size definition in mosaik (e.g., 1 mosaik step is equivalent to 1 millisecond) 
- `string msgId` : a msg id identifies the message
- `int32 until` : the time of the simulation end in mosaik
- `int32 creation_time` : the time of the creation of the message
- `string change_module` : used for messages of the type `DISCONNECT` and `RECONNECT`, identifies the module that has to be changed
- `bool connection_change_successful` : indicates if a connection change in OMNeT++ has been successful 

## OMNeT++ messages 
In OMNeT++ there are a number of defined messages or events, which are used for internal information exchange between modules and the scheduler in OMNeT++. 

### MosaikSchedulerMessage
A [MosaikSchedulerMessage](../cosima_omnetpp_project/messages/MosaikSchedulerMessage.msg) is used for message exchange
between the [MosaikScheduler](../cosima_omnetpp_project/modules/MosaikScheduler.h) and the modules of the network. \
If, for example, a message between client0 and client1 has to be sent, the MosaikScheduler sends a MosaikSchedulerMessage
to client0 which then sends an inet packet to client1 over the modelled network. 

### MosaikApplicationChunk
The [MosaikApplicationChunk](../cosima_omnetpp_project/messages/MosaikApplicationChunk.msg) is used as a payload in an 
inet packet which is sent between two modules over the network in OMNeT++. It contains message specific information such 
as the content of the message, its sender and receiver and the message id. 

### MosaikCtrlEvent
The [MosaikCtrlEvent](../cosima_omnetpp_project/messages/MosaikCtrlEvent.msg) is used for control tasks such as max 
advance or until events. A MosaikCtrlEvent contains an enum 
[ControlType](../cosima_omnetpp_project/messages/ControlType.msg) what defines the type of control task. 

### Timer 
The [Timer message](../cosima_omnetpp_project/messages/Timer.msg) is used in the implementation of the 
[SocketAgentAppTcp](../cosima_omnetpp_project/modules/AgentAppTcp.h) in order to handle the TCP connections in the 
OMNeT++ network. 
