/*
 * AgentAppTcp.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 *
 */

#include <cmath>
#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include <string.h>
#include <algorithm>

#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

#include "../messages/CosimaSchedulerMessage_m.h"
#include "AgentAppTcp.h"

#include "../messages/CosimaApplicationChunk_m.h"
#include "../messages/CosimaCtrlEvent_m.h"
#include "CosimaScheduler.h"


Define_Module(AgentAppTcp);

AgentAppTcp::AgentAppTcp() {
    scheduler = nullptr;
}

AgentAppTcp::~AgentAppTcp() {
    //delete and cancel all timers used
    auto iter = timer.begin();

    while (iter != timer.end()) {
        std::list<Timer * > timerForModule = iter->second;
        std::list<Timer *>::iterator it;
        for (it = timerForModule.begin(); it != timerForModule.end(); ++it) {
            scheduler->log(nameStr + ": cancel timer for client with ID " + std::to_string(iter->first), "debug");
            cancelAndDelete(*it);
        }
        ++iter;
    }
}

void AgentAppTcp::initialize(int stage) {
    TcpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        // get intern socket scheduler from simulation and cast to CosimaScheduler
        scheduler =
                check_and_cast<CosimaScheduler *>(getSimulation()->getScheduler());
        // register module at scheduler
        scheduler->setInterfaceModule(this, false);
        // get name of parent module as string for logging
        nameStr = this->getParentModule()->getName();
    }
}

void AgentAppTcp::handleMessageWhenUp(cMessage *msg)
{
    if(typeid(*msg) == typeid(Timer)) {
        // message is timer object in order to send data over the network
        handleTimer(msg);
    }
    else if (msg->getKind() == 0 and msg->getArrivalGate() != gate("socketIn")) {
        // message is an event from the scheduler
        handleSocketEvent(msg, msg->getArrivalTime().dbl());
    }
    else {
        // message is an event at the socket in the network
        TcpAppBase::socket.processMessage(msg);
    }
}

bool AgentAppTcp::handleSocketEvent(cMessage *msg, double couplingSimTime) {

    // simTime of coupled framework should have positive value
    if (couplingSimTime < 0) {
        delete msg;
        return false;
    }

    if (typeid(*msg) == typeid(CosimaCtrlEvent)) {
        // is traffic event
        CosimaCtrlEvent *msgCasted = dynamic_cast<CosimaCtrlEvent *>(msg);
        auto receiverName = msgCasted->getDestination();

        // get corresponding port for receiver name
        auto receiverPort = scheduler->getPortForModule(receiverName);

        if (receiverPort == -1) {
            delete msg;
            return false;
        }

        scheduler->log(nameStr + ": send traffic to " + receiverName + " at time " + simTime().str() + ".", "info");

        auto packet = new inet::Packet();
        const auto &payload = inet::makeShared<CosimaApplicationChunk>();
        auto msgSize = msgCasted->getPacketLength();
        payload->setChunkLength(inet::B(msgSize));
        payload->setIsTrafficMessage(true);
        packet->insertAtBack(payload);
        auto clientId = getModuleIdByName(receiverName);
        setPacketForModuleId(clientId, packet);


        auto timerMsg = getTimerForModuleId(clientId);
        // type 0 means connect
        timerMsg->setTimerType(0);
        timerMsg->setReceiverName(receiverName);
        timerMsg->setReceiverPort(receiverPort);

        // schedule timer as self message
        scheduleAt(simTime(), timerMsg);

        delete msg;
        return true;
    }

    // make packet
    if (typeid(*msg) == typeid(CosimaSchedulerMessage)) {
        // is "normal" message
        CosimaSchedulerMessage *msgCasted = dynamic_cast<CosimaSchedulerMessage *>(msg);
        auto content = msgCasted->getContent();

        // get content from message
        auto receiverName = msgCasted->getReceiver();
        auto senderName = msgCasted->getSender();
        auto msgId = msgCasted->getMsgId();
        auto msgSize = msgCasted->getSize();
        auto creation_time = msgCasted->getCreationTime();

        // get corresponding port for receiver name
        auto receiverPort = scheduler->getPortForModule(receiverName);

        if (receiverPort == -1) {
            sendTransmissionErrorNotification(receiverName, msgId, false);
            delete msg;
            return false;
        }
        scheduler->log(nameStr + ": send message " + msgId + " to " + receiverName + " with port " + std::to_string(receiverPort) + " at time "
                + std::to_string(couplingSimTime));
        std::string contentStr = content;
        // scheduler->log("content is: " + contentStr);

        // make packet
        auto packet = new inet::Packet();
        const auto &payload = inet::makeShared<CosimaApplicationChunk>();
        payload->setContent(content);
        payload->setReceiver(receiverName);
        payload->setSender(senderName);
        payload->setChunkLength(inet::B(msgSize));
        payload->setCreationTimeOmnetpp(simTime());
        payload->setMsgId(msgId);
        payload->setCreationTimeCoupling(creation_time);
        payload->setIsTrafficMessage(false);
        packet->insertAtBack(payload);
        packet->setTimestamp(couplingSimTime);

        auto clientId = getModuleIdByName(receiverName);
        setPacketForModuleId(clientId, packet);


        auto timerMsg = getTimerForModuleId(clientId);
        // type 0 means connect
        timerMsg->setTimerType(0);
        timerMsg->setReceiverName(receiverName);
        timerMsg->setReceiverPort(receiverPort);
        timerMsg->setMessageId(msgId);

        // schedule timer as self message
        scheduleAt(simTime(), timerMsg);

        delete msg;
        return true;
    }
    delete msg;
    return false;
}

Timer *AgentAppTcp::getTimerForModuleId(int clientId) {
    auto iter = timer.begin();

    // if timer is already created for client id, return it
    while (iter != timer.end()) {
        if (iter->first == clientId) {
            std::list<Timer * > timerForModule = iter->second;
            std::list<Timer *>::iterator it;
            for (it = timerForModule.begin(); it != timerForModule.end(); ++it) {
                Timer *matchedTimer = *it;
                if(not matchedTimer->isScheduled()) {
                    this->take(matchedTimer);
                    return *it;
                }
            }
            Timer *newTimer = new Timer();
            timer[clientId].push_back(newTimer);
            this->take(newTimer);
            return newTimer;
        }
        ++iter;
    }

    // else create new timer and add it to the timer map
    Timer *newTimer = new Timer();
    std::list<Timer *> timerForModule;
    timerForModule.push_back(newTimer);
    timer[clientId] = timerForModule;
    this->take(newTimer);
    return newTimer;
}

void AgentAppTcp::setPacketForModuleId(int clientId, inet::Packet *packet) {
    packetToClient[clientId].push_back(packet);
}

inet::Packet *AgentAppTcp::getPacketForModuleId(int clientId) {
    auto iter = packetToClient.begin();

    if (packetToClient.count(clientId) and packetToClient[clientId].size() > 0) {
        auto packet = packetToClient[clientId].front();
        packetToClient[clientId].pop_front();
        return packet;
    }
    return nullptr;
}

int AgentAppTcp::getModuleIdByName(const char *module_name)
{
    if(module_name != nullptr and strlen(module_name) != 0) {
        auto moduleObject = scheduler->getReceiverModule(module_name);
        if (moduleObject == nullptr) {
            return -1;
        }
        return moduleObject->getId();
    }
    return -1;
}


void AgentAppTcp::handleTimer(cMessage *msg) {
    if(msg != nullptr and typeid(*msg) == typeid(Timer)) {
        Timer *recvtimer = dynamic_cast<Timer *>(msg);
        switch (recvtimer->getTimerType()) {
        case -1:
            break;

        case 0:
            connect(recvtimer->getReceiverName(), recvtimer->getReceiverPort(), recvtimer->getMessageId());
            break;

        case 1:
            waitingForPortsToConnectTo.erase(recvtimer->getReceiverPort());
            sendData(recvtimer->getReceiverName(), recvtimer->getMessageId());
            break;

        case 2:
            close();
            break;

        case 3:
            renew(recvtimer->getReceiverName());
            break;

        case 4:
            connectTimeout(recvtimer->getReceiverName(), recvtimer->getMessageId(), recvtimer->getReceiverPort());
            break;

        }
    } else {
        throw cRuntimeError("AgentAppTcp: called handleTimer with non-Timer object.");
    }
}

void AgentAppTcp::sendTransmissionErrorNotification(const char *receiverName, const char *msgId, bool timeout=false) {
    CosimaSchedulerMessage *notification_message = new CosimaSchedulerMessage();
    notification_message->setTransmission_error(true);
    notification_message->setSender(this->getParentModule()->getName());
    notification_message->setReceiver(receiverName);
    notification_message->setTimeout(timeout);
    notification_message->setMsgId(msgId);
    scheduler->sendToCoupledSimulation(notification_message);
}

void AgentAppTcp::connectTimeout(const char *receiverName, const char *msgId, int receiverPort) {
    if (std::find(waitingForPortsToConnectTo.begin(), waitingForPortsToConnectTo.end(), receiverPort) != waitingForPortsToConnectTo.end()) {
        scheduler->log(nameStr + " received connect timeout for " + receiverName + " with message ID " + msgId, "debug");

        // case 1: called connect because received connect timeout and AgentApp is still waiting for connect
        // -> send transmission error to coupled simulation and don't wait any longer
        scheduler->log(nameStr + ": received connect timeout information for " + receiverName + " at time " + simTime().str(), "debug");
        sendTransmissionErrorNotification(receiverName, msgId, true);
        waitingForPortsToConnectTo.erase(receiverPort);
    }
}


void AgentAppTcp::connect(const char *receiverName, int receiverPort, const char *messageId)
{
    // case: received "normal" connect timer
    // -> connect to given receiver
    inet::TcpSocket clientSocket;
    int clientId = getModuleIdByName(receiverName);
    if(clientSockets.find(clientId) != clientSockets.end()) {
        clientSocket = clientSockets[clientId];
    } else {
        const char *localAddress = par("localAddress");
        int localPort = par("localPort");

        clientSocket.setOutputGate(gate("socketOut"));
        clientSocket.setCallback(this);
        clientSocket.bind(
                localAddress[0] ? inet::L3Address(localAddress) : inet::L3Address(),
                        localPort);
        auto clientId = getModuleIdByName(receiverName);
        clientSockets[clientId] = clientSocket;
    }
    // we need a new connId if this is not the first connection
    clientSocket.renewSocket();

    int timeToLive = par("timeToLive");
    if (timeToLive != -1)
        clientSocket.setTimeToLive(timeToLive);

    int dscp = par("dscp");
    if (dscp != -1)
        clientSocket.setDscp(dscp);

    int tos = par("tos");
    if (tos != -1)
        clientSocket.setTos(tos);


    inet::L3Address destination;
    try {
        inet::L3AddressResolver().tryResolve(receiverName, destination);
        if (destination.isUnspecified()) {
            scheduler->log(nameStr + ": connecting to " + receiverName + " and port " + std::to_string(receiverPort) +
                    ": cannot resolve destination address", "warning");
        }
        else {
            std::string receiverNameStr = receiverName;
            std::string messageIdStr = messageId;
            scheduler->log(nameStr + ": connecting to " + receiverNameStr + "(" + destination.str() + ") and port " + std::to_string(receiverPort), "debug");

            clientSocket.connect(destination, receiverPort);
            waitingForPortsToConnectTo.insert(receiverPort);

            numSessions++;
            emit(connectSignal, 1L);


            auto timerMsg = getTimerForModuleId(clientId);
            // type 4 means connectTimeout
            timerMsg->setTimerType(4);
            timerMsg->setReceiverName(receiverNameStr.c_str());
            timerMsg->setReceiverPort(receiverPort);
            timerMsg->setMessageId(messageIdStr.c_str());

        }
        clientSockets[clientId] = clientSocket;
    } catch(...) {
        scheduler->log(nameStr + ": Error when trying to resolve L3 address", "warning");
        sendTransmissionErrorNotification(receiverName, messageId);
        waitingForPortsToConnectTo.erase(receiverPort);
    }
}

void AgentAppTcp::renew(const char *receiver_name) {
    auto clientId = getModuleIdByName(receiver_name);
    inet::TcpSocket clientSocket = clientSockets[clientId];
    clientSocket.renewSocket();
    clientSocket.setOutputGate(gate("socketOut"));
    clientSocket.setCallback(this);
    const char *localAddress = par("localAddress");
    int localPort = par("localPort");

    clientSocket.bind(
            localAddress[0] ? inet::L3Address(localAddress) : inet::L3Address(),
                    localPort);
    clientSocket.listen();
}

void AgentAppTcp::socketEstablished(inet::TcpSocket *socket) {
    int port = socket->getRemotePort();
    if (std::find(waitingForPortsToConnectTo.begin(), waitingForPortsToConnectTo.end(), port) == waitingForPortsToConnectTo.end()) {
    } else {
        auto moduleName = scheduler->getModuleNameFromPort(socket->getRemotePort());
        if(moduleName != "") {
            auto timerMsg = getTimerForModuleId(getModuleIdByName(moduleName.c_str()));
            // type 1 means send
            timerMsg->setTimerType(1);
            timerMsg->setReceiverName(moduleName.c_str());
            timerMsg->setReceiverPort(socket->getRemotePort());
            scheduleAt(simTime(), timerMsg);
        }
    }

}

void AgentAppTcp::socketDataArrived(inet::TcpSocket *socket,
        inet::Packet *msg, bool urgent) {
    scheduler->log(nameStr + ": socket data arrived.", "debug");
    auto copy = msg->dup();
    simtime_t delay;
    auto delay_d = 0.0;
    auto replyReceiver = "";

    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    emit(inet::packetReceivedSignal, msg);

    if (msg->getArrivalGate() == gate("socketIn")) {

        inet::Packet *recvPacket = dynamic_cast<inet::Packet *>(copy);
        if (recvPacket != nullptr) {
            auto replyContent = "";
            CosimaSchedulerMessage *answer = new CosimaSchedulerMessage();
            answer->setTransmission_error(false);

            inet::b offset = inet::b(0);  // start from the beginning
            auto foundApplicationChunk = false;

            while (auto chunk = recvPacket->peekAt(offset)->dupShared()) {  // for each chunk
                if (foundApplicationChunk) {
                    delete recvPacket;
                    break;
                }
                auto length = chunk->getChunkLength();

                if (chunk->getClassName() == std::string("inet::SliceChunk")) {
                    auto newPacket = recvPacket->peekData<inet::SliceChunk>();
                    auto encapsulatedChunk = newPacket->getChunk();
                    if (encapsulatedChunk->getClassName() != std::string("CosimaApplicationChunk")) {
                        if (encapsulatedChunk->getClassName() == std::string("inet::SequenceChunk")) {
                            inet::SequenceChunk *encapsulatedSequenceChunk = dynamic_cast<inet::SequenceChunk *>(encapsulatedChunk.get());
                            auto queue = encapsulatedSequenceChunk->getChunks();
                            for (int i=0; i < queue.size(); i++) {
                                auto item = queue[i];
                                auto itemget = item.get();
                                if (itemget->getClassName() == std::string("inet::SliceChunk")) {
                                    const inet::SliceChunk *encapsulatedSliceChunk = dynamic_cast<const inet::SliceChunk *>(itemget);
                                    auto appChunkInSliceChunk = encapsulatedSliceChunk->getChunk();
                                    auto appChunk = appChunkInSliceChunk->peek<CosimaApplicationChunk>(inet::b(0), appChunkInSliceChunk->getChunkLength());
                                    if (appChunk->isTrafficMessage()) {
                                        scheduler->log(nameStr + " received traffic message at time "+ simTime().str(), "info");
                                        return;
                                    } else {
                                        bool alreadyReceived = (std::find(receivedMsgIds.begin(), receivedMsgIds.end(), appChunk->getMsgId()) != receivedMsgIds.end());
                                        if (alreadyReceived) {
                                            return;
                                        }
                                        answer->setContent(appChunk->getContent());
                                        answer->setReceiver(appChunk->getReceiver());
                                        answer->setFalsified(appChunk->isFalsified());
                                        simtime_t delay =
                                                simTime() - appChunk->getCreationTimeOmnetpp();
                                        auto delay_i = 0U;
                                        delay_i = ceil(delay.dbl()*1000);
                                        answer->setDelay(delay_i);
                                        answer->setSender(appChunk->getSender());
                                        answer->setMsgId(appChunk->getMsgId());
                                        answer->setCreationTime(appChunk->getCreationTimeCoupling());
                                        foundApplicationChunk = true;
                                        receivedMsgIds.push_back(appChunk->getMsgId());
                                        sendReply(answer);
                                    }
                                }
                            }
                        }
                    } else {
                        auto appChunk = encapsulatedChunk->peek<CosimaApplicationChunk>(inet::b(0), encapsulatedChunk->getChunkLength());
                        if (appChunk->isTrafficMessage()) {
                            scheduler->log(nameStr + " received traffic message at time "+ simTime().str(), "info");
                            return;
                        } else {
                            bool alreadyReceived = (std::find(receivedMsgIds.begin(), receivedMsgIds.end(), appChunk->getMsgId()) != receivedMsgIds.end());
                            if (alreadyReceived) {
                                return;
                            }
                            answer->setFalsified(appChunk->isFalsified());
                            answer->setContent(appChunk->getContent());
                            answer->setReceiver(appChunk->getReceiver());
                            simtime_t delay =
                                    simTime() - appChunk->getCreationTimeOmnetpp();
                            auto delay_i = 0U;
                            delay_i = ceil(delay.dbl()*1000);
                            answer->setDelay(delay_i);
                            answer->setSender(appChunk->getSender());
                            answer->setMsgId(appChunk->getMsgId());
                            answer->setCreationTime(appChunk->getCreationTimeCoupling());
                            foundApplicationChunk = true;
                            receivedMsgIds.push_back(appChunk->getMsgId());
                            sendReply(answer);
                        }
                    }
                } else if (chunk->getClassName() == std::string("CosimaApplicationChunk")) {
                    auto cosimaApplicationChunk = recvPacket->peekAt<CosimaApplicationChunk>(offset, length);
                    if (cosimaApplicationChunk->isTrafficMessage()) {
                        scheduler->log(nameStr + " received traffic message at time "+ simTime().str(), "info");
                        return;
                    } else {
                        replyContent = cosimaApplicationChunk->getContent();
                        simtime_t delay = simTime() - cosimaApplicationChunk->getCreationTimeOmnetpp();
                        auto delay_i = 0U;
                        delay_i = ceil(delay.dbl()*1000);
                        replyReceiver = cosimaApplicationChunk->getReceiver();
                        auto replySender = cosimaApplicationChunk->getSender();
                        auto msgId = cosimaApplicationChunk->getMsgId();
                        auto creationTime = cosimaApplicationChunk->getCreationTimeCoupling();
                        answer->setFalsified(cosimaApplicationChunk->isFalsified());
                        answer->setContent(replyContent);
                        answer->setReceiver(replyReceiver);
                        answer->setDelay(delay_i);
                        answer->setSender(replySender);
                        answer->setMsgId(msgId);
                        answer->setCreationTime(creationTime);
                        foundApplicationChunk = true;
                        receivedMsgIds.push_back(msgId);
                        sendReply(answer);
                    }

                } else {
                    offset += chunk->getChunkLength();
                    if (offset >= recvPacket->getTotalLength()) {
                        scheduler->log("Couldn't find CosimaApplicationChunk in packet: " + recvPacket->str(), "warning");
                        break;
                    }
                    answer->setDelay(delay_d);
                }
            }
            delete answer;
            delete msg;
        }
    }
}

void AgentAppTcp::sendData(const char *receiverName, const char *messageId) {
    int clientId = getModuleIdByName(receiverName);
    if(clientSockets.find(clientId) != clientSockets.end()) {
        inet::TcpSocket clientSocket;
        clientSocket = clientSockets[clientId];

        auto packet = getPacketForModuleId(clientId);

        if (packet == nullptr) {
            scheduler->log(nameStr + " has not saved packet for clientId " + std::to_string(clientId), "warning");
        }

        if (packet) {
            inet::Packet *sending;
            sending = packet->dup();
            sending->setTimestamp(packet->getTimestamp());
            int numBytes = sending->getByteLength();
            emit(inet::packetSentSignal, sending);
            clientSocket.send(sending);
            packetsSent++;
            bytesSent += numBytes;
            if (packet->getOwner() == this) {
                delete packet;
            }
        }
    } else {
        scheduler->log(nameStr + ": socket not connected to " + receiverName, "warning");
        connect(receiverName, scheduler->getPortForModule(receiverName), messageId);
    }

}


void AgentAppTcp::sendReply(CosimaSchedulerMessage *reply) {
    scheduler->sendToCoupledSimulation(reply);
}

void AgentAppTcp::socketClosed(inet::TcpSocket *socket) {
    scheduler->log(nameStr + ": socket closed.", "debug");

    TcpAppBase::socketClosed(socket);
    if (operationalState == State::STOPPING_OPERATION && !this->socket.isOpen())
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void AgentAppTcp::socketFailure(inet::TcpSocket *socket, int code) {
    scheduler->log(nameStr + ": socket failure with code:" + std::to_string(code), "debug");
    TcpAppBase::socketFailure(socket, code);
}

void AgentAppTcp::handleStartOperation(inet::LifecycleOperation *operation) {
    const char *localAddress = par("localAddress");
    int localPort = par("localPort");

    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(
            localAddress[0] ? inet::L3Address(localAddress) : inet::L3Address(),
                    localPort);
    serverSocket.listen();
}

void AgentAppTcp::handleStopOperation(inet::LifecycleOperation *operation) {
    if (socket.isOpen()) close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void AgentAppTcp::handleCrashOperation(inet::LifecycleOperation *operation) {
}

void AgentAppTcp::finish() {
}
