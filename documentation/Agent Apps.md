# Agent Apps
The *[AgentAppUdp](../cosima_omnetpp_project/modules/AgentAppUdp.h)* and *[AgentAppTcp](../cosima_omnetpp_project/modules/AgentAppTcp.h)* represent the implementation of the application layer
(and transport layer) of the end devices, which represent the agents from mosaik on the OMNeT++ side. \
The *AgentAppUdp* is used for communication via UDP and the *AgentAppTcp* is used for communication via TCP. The protocol (UDP or TCP) used by the clients can be configured by choosing the corresponding *AgentApp(TCP/UDP)* in the .ini-file of the project. 

## *[AgentApp](../cosima_omnetpp_project/modules/AgentAppUdp.h)*
When the one instance of the *AgentAppUdp* class is initialised, the corresponding module is registered with the scheduler. This way, the scheduler has knowledge about the modules in the network and can forward messages from mosaik accordingly. \
The *AgentAppUdp* inherits from the class cSimpleModule and thus receives messages via the method handleMessage().  A received message can either have been received via the modelled network or given to the *AgentAppUdp* by the scheduler. \
If the message has been placed by the scheduler, a packet with the content of the message is generated and sent over the network. For this purpose, the *AgentAppUdp* holds a UDP socket of the library inet. \
If the message has been received over the network, the information is returned to the scheduler. 

## *[AgentAppTcp](../cosima_omnetpp_project/modules/AgentAppTcp.h)*
When the one instance of the *AgentAppTcp* class is initialised, the corresponding module is also registered with the scheduler. The other concepts are also very similar to the implementation of the AgentApp. \
The main difference lies in the protocol used. Due to the more complex functioning of TCP compared to UDP, the concept of timer messages has been used here. For further details on messages see [Message Types](Message Types.md). \
Timer messages assist in establishing connections and sending messages over the network. Depending on the type of timer, these trigger the connection to another socket, the sending of messages via the network, the closing of a connection or the renewal of a socket.\
The *AgentAppTcp* holds a map of timers for different modules. That way, a list of Timer messages for each established connection exists.\
The *AgentAppTcp* has a server TCP socket for receiving messages and a map of client TCP sockets for sending messages.  
