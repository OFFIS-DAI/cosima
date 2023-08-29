# Message Types
There are different message types used in order to exchange information in this project. \
In mosaik normal python-dictionaries are used for message exchange between the simulators. \
For information exchange between mosaik and OMNeT++ Protocol buffers (in the following called Protobuf) are used and described in detail below. \
For information exchange in OMNeT++ .msg-files are used and also described below.

## Protocol buffers 
[Protocol buffers](https://developers.google.com/protocol-buffers/docs/overview) provide an easy way of serializing 
data. Here, a [Proto file](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_core/messages/message.proto) is used in order to define the different Protocol buffers. \
A Protobuf message contains a `CosimaMsgGroup`, which is a list of individual message objects, in order to send multiple 
messages at once / in one time step. 
There are different types of protobuf messages with different values: 
* `InitialMessage`: One message of this type is sent to OMNeT++ in the beginning of the simulation in order to inform OMNeT++ about important parameters in the simulation on mosaik side.
  * `string msg_id` : a msg id identifies the message
  * `max_advance` : the current value of the mosaik-internal max advance value
  * `int32 until` : the time of the simulation end in mosaik
  * `bool is_time_based`: indicates whether the simulation in mosaik is time-based or not
  * `int32 step_size` : the step size used in the simulation, based on the step-size definition in mosaik (e.g., 1 mosaik step is equivalent to 1 millisecond)
* `InfoMessage`
  * `string msg_id` : a msg id identifies the message
  * `max_advance` : the current value of the mosaik-internal max advance value
  * `int32 sim_time`: the simulation time in OMNeT++
  * `string sender` : the sender of a message, should be the name of the module in OMNeT++
  * `string receiver` : the receiver of a message, should be the name of the module in OMNeT++
  * `int32 size`: the size of the message in bytes (not containing the fields just used for synchronisation, e.g., `simTime`, `stepSize`...)
  * `string content` : the content of the message
  * `int32 creation_time` : the time of the creation of the message
  * `bool is_valid`: indicated whether the message was transmitted in a valid format
* `SynchronizationMessage`: messages of this type are sent between mosaik and OMNeT++ in order to synchronize the clocks
  * `enum MsgType {
      MAX_ADVANCE = 0;
      WAITING = 1;
      TRANSMISSION_ERROR = 2;
    }`
    `MsgType msg_type`: the type/ cause of the message. 
  * `string msg_id` : a msg id identifies the message
  * `int32 sim_time`: the simulation time in OMNeT++
* `InfrastructureMessage`: these messages are used for information about infrastructure changes in OMNeT++ such as disconnects or reconnects of routers etc.
  * `enum MsgType {
      DISCONNECT = 0;
      RECONNECT = 1;
    }
    MsgType msg_type` : indicates whether the message is sent as disconnect or reconnect info
  * `string msg_id` : a msg id identifies the message
  * `int32 sim_time`: the simulation time in OMNeT++
  * `string change_module` : identifies the module that has to be changed
  * `bool connection_change_successful` : indicates if a connection change in OMNeT++ has been successful
A `CosimaMsgGroup` also contains 
information about the current time step in mosaik. \

## OMNeT++ messages 
In OMNeT++ there are a number of defined messages or events, which are used for internal information exchange between modules and the scheduler in OMNeT++. 

### MosaikSchedulerMessage
A [MosaikSchedulerMessage](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/messages/MosaikSchedulerMessage.msg) is used for message exchange
between the [MosaikScheduler](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/modules/MosaikScheduler.h) and the modules of the network. \
If, for example, a message between client0 and client1 has to be sent, the MosaikScheduler sends a MosaikSchedulerMessage
to client0 which then sends an inet packet to client1 over the modelled network. 

### MosaikApplicationChunk
The [MosaikApplicationChunk](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/messages/MosaikApplicationChunk.msg) is used as a payload in an 
inet packet which is sent between two modules over the network in OMNeT++. It contains message specific information such 
as the content of the message, its sender and receiver and the message id. 

### MosaikCtrlEvent
The [MosaikCtrlEvent](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/messages/MosaikCtrlEvent.msg) is used for control tasks such as max 
advance or until events. A MosaikCtrlEvent contains an enum 
[ControlType](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/messages/ControlType.msg) what defines the type of control task. 

### Timer 
The [Timer message](.https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/messages/Timer.msg) is used in the implementation of the 
[SocketAgentAppTcp](https://github.com/OFFIS-cosima/cosima/blob/master/cosima_omnetpp_project/modules/AgentAppTcp.h) in order to handle the TCP connections in the 
OMNeT++ network. 
