# CommunicationSimulator
To understand the role of the [CommunicationSimulator](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_core/simulators/communication_simulator.py), the meaning of the max advance
value and the concept of the synchronization between OMNeT++ and mosaik is
useful. Explanations of these can be found in ([Synchronization](Synchronization.md)).


## Concept
The CommunicationSimulator is the interface to OMNeT++ for simulators from python and 
therefore the connection between mosaik and OMNeT++. It takes messages from
mosaik simulators, sends them to OMNeT++ and receives messages from OMNeT++
which it then forwards to the corresponding mosaik simulators.
The CommunicationSimulator saves an instance of the class ([OmnetppConnection](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_core/simulators/omnetpp_connection.py)),
which opens up TCP-Connection to OMNeT++ and is used to send and receive messages to and from OMNeT++.
This instance keeps an active connection to OMNeT++ and waits for messages independently of the step logic of mosaik.
This way, messages that arrive while the CommunicationSimulator is not in its step function (so while another simulator is in step function) will still be considered.
Whenever a message is received from OMNeT++, the class OmnetppConnection stores the message in a list which
is accessible by the CommunicationSimulator.


## Operating Principle
If a message should be sent to OMNeT++, the CommunicationSimulator receives this message from other
simulators by mosaik in the step-function as input. The message is then send by the
instance of the OmnetppConnection, which sends the message via the opened
TCP connection.
All simulators connected to the CommunicationSimulator always step first and send their messages.
The CommunicationSimulator receives these messages from other simulators one mosaik step 
later and therefore after all the steps of other simulators.
Thus, it is ensured that the CommunicationSimulator considers all messages from other simulators in its step.

When the CommunicationSimulator steps, it also checks whether there are messages in the list
in the OmnetppConnection, so if there are some messages from OMNeT++ which
should be forwarded to mosaik simulators. If this is the case, the CommunicationSimulator 
takes the messages and sends them to the corresponding receivers of the 
messages through mosaik. This is done by sending the message via the 
get_data-method from mosaik.
The CommunicationSimulator is furthermore responsible for the synchronization to OMNeT++. 
It therefore checks in its mosaik-step after sending the received messages to 
OMNeT++, whether it should wait for further messages from OMNeT++ or not. It
always counts the number of agent messages sent to OMNeT++ and received from
OMNeT++. Therefore, it can determine, whether there are currently still 
messages simulated in OMNeT++. This means that there are still relevant messages for
the agent simulators. 
When the simulated messages are then being sent back to mosaik, this might
cause further events (messages) from the receiving agent of the simulated message.


## Step function
An example simulation step of the CommunicationSimulator could look like this.<br />
![image](./images/commsim_step.png)


* If the CommunicationSimulator has messages in its inputs in the step function from mosaik, 
it sends them to OMNeT++ via the class OmnetppConnection.
* If the CommunicationSimulator received a message for any of the agents, it directly ends
its step and sends the message to the receiving agent by its get-data-method.
* If there are no further messages being simulated in OMNeT++, the CommunicationSimulator does
not wait for any messages from OMNeT++ and it ends its step.
* If there are still messages being simulated in OMNeT++, the CommunicationSimulator waits for 
these messages, it checks whether the next step is
determined by the agents. If this is the case, the CommunicationSimulator directly ends its
step to make sure these actions of the actions are simulated.
* If the next step is not determined by the agents, the CommunicationSimulator sends a 
*WaitingMessage* (see [Message Types](MessageTypes.md)) to OMNeT++, 
to point out to OMNeT++ that the mosaik-side is still waiting for messages. 
After that, the CommunicationSimulator is actively waiting until a message was received from
OMNeT++ (either an AgentMessage, a message to point out a synchronization point
is reached from OMNeT++ (see [Synchronization](Synchronization.md)) or an 
error message). If a message from OMNeT++ which is pointing out that max 
advance is reached was received, the CommunicationSimulator ends its step and tells mosaik that 
its next step will be one step after the time for max advance (therefore max advance + 1). Therefore, it is 
ensured that the event in mosaik which influenced the max advance value has 
taken place. The CommunicationSimulator continues to wait for messages from OMNeT++ afterwards.
