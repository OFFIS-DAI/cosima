/*
 * SocketAgentAppTcp.cc
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

#include "SocketAgentAppTcp.h"
#include "messages/MosaikApplicationChunk_m.h"
#include "MosaikScheduler.h"
#include "messages/MosaikSchedulerMessage_m.h"
#include "messages/MosaikCtrlEvent_m.h"
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

Define_Module(SocketAgentAppTcp);

SocketAgentAppTcp::SocketAgentAppTcp() {
    scheduler = nullptr;
}

SocketAgentAppTcp::~SocketAgentAppTcp() {
    //delete and cancel all timers used
    auto iter = timer.begin();

    while (iter != timer.end()) {
        std::list<Timer * > timer_for_module = iter->second;
        std::list<Timer *>::iterator it;
        for (it = timer_for_module.begin(); it != timer_for_module.end(); ++it) {
            scheduler->log(nameStr + ": cancel timer for client with ID " + std::to_string(iter->first));
            cancelAndDelete(*it);
        }
        ++iter;
    }
}

void SocketAgentAppTcp::initialize(int stage) {
    TcpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        // get intern socket scheduler from simulation and cast to MosaikScheduler
        scheduler =
                check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());
        // register module at scheduler
        scheduler->setInterfaceModule(this, false);
    }
    // get name of parent module as string for logging
    nameStr = this->getParentModule()->getName();
}

void SocketAgentAppTcp::handleMessageWhenUp(cMessage *msg)
{
    if(typeid(*msg) == typeid(Timer)) {
        // message is timer object in order to send data over the network
        handleTimer(msg);
    }
    else if (msg->getKind() == 0 and msg->getArrivalGate() != gate("socketIn")) {
        // message is an event from the scheduler (from mosaik)
        handleSocketEvent(msg, msg->getArrivalTime().dbl());
    }
    else {
        // message is an event at the socket in the network
        TcpAppBase::socket.processMessage(msg);
    }
}

void SocketAgentAppTcp::handleSocketEvent(cMessage *msg, double mosaikSimTime) {
    // make packet
    if (typeid(*msg) == typeid(MosaikSchedulerMessage)) {
        MosaikSchedulerMessage *msg_casted = dynamic_cast<MosaikSchedulerMessage *>(msg);
        const char *content = msg_casted->getContent();

        // get content from message
        std::string receiverName = msg_casted->getReceiver();
        std::string senderName = msg_casted->getSender();
        std::string msgId = msg_casted->getMsgId();
        int msgSize = msg_casted->getSize();
        int creation_time = msg_casted->getCreationTime();

        // get corresponding port for receiver name
        int receiverPort = scheduler->getPortForModule(receiverName.c_str());
        scheduler->log(nameStr + ": send message with content '" +
                content + "' to " + receiverName + " with port " + std::to_string(receiverPort) + " at time " + std::to_string(mosaikSimTime));

        // make packet
        inet::Packet *packet = new inet::Packet();
        const auto &payload = inet::makeShared<MosaikApplicationChunk>();
        payload->setContent(content);
        payload->setReceiver(receiverName.c_str());
        payload->setSender(senderName.c_str());
        payload->setChunkLength(inet::B(msgSize));
        payload->setCreationTime(mosaikSimTime);
        payload->setMsgId(msgId.c_str());
        payload->setCreationTimeMosaik(creation_time);
        packet->insertAtBack(payload);
        packet->setTimestamp(mosaikSimTime);

        int clientId = getModuleIdByName(receiverName.c_str());
        setPacketForModuleId(clientId, packet);


        Timer *timerMsg = getTimerForModuleId(clientId);
        // type 0 means connect
        timerMsg->setTimerType(0);
        timerMsg->setReceiverName(receiverName.c_str());
        timerMsg->setReceiverPort(receiverPort);

        // schedule timer as self message
        scheduleAt(simTime(), timerMsg);

        delete msg;
    }
}

Timer *SocketAgentAppTcp::getTimerForModuleId(int clientId) {
    auto iter = timer.begin();

    // if timer is already created for client id, return it
    while (iter != timer.end()) {
        if (iter->first == clientId) {
            std::list<Timer * > timer_for_module = iter->second;
            std::list<Timer *>::iterator it;
            for (it = timer_for_module.begin(); it != timer_for_module.end(); ++it) {
                Timer *matchedTimer = *it;
                if(not matchedTimer->isScheduled()) {
                    return *it;
                }
            }
            Timer *newTimer = new Timer();
            timer[clientId].push_back(newTimer);
            return newTimer;
        }
        ++iter;
    }

    // else create new timer and add it to the timer map
    Timer *newTimer = new Timer();
    std::list<Timer *> timer_for_module;
    timer_for_module.push_back(newTimer);
    timer[clientId] = timer_for_module;
    return newTimer;
}

void SocketAgentAppTcp::setPacketForModuleId(int clientId, inet::Packet *packet) {
    packetToClient[clientId] = packet;
}

inet::Packet *SocketAgentAppTcp::getPacketForModuleId(int clientId) {
    auto iter = packetToClient.begin();

    // if timer is already created for client id, return it
    while (iter != packetToClient.end()) {
        if (iter->first == clientId) {
            inet::Packet *packet = packetToClient[clientId];
            return packet;
        }
        ++iter;
    }
    return nullptr;
}

int SocketAgentAppTcp::getModuleIdByName(const char *module_name)
{
    if(module_name != nullptr) {
        cModule *moduleObject = scheduler->getReceiverModule(module_name);
        return moduleObject->getId();
    }
    return -1;
}


void SocketAgentAppTcp::handleTimer(cMessage *msg) {
    if(typeid(*msg) == typeid(Timer)) {
        Timer *recvtimer = dynamic_cast<Timer *>(msg);
        switch (recvtimer->getTimerType()) {
        case -1:
            break;

        case 0:
            connect(recvtimer->getReceiverName(), recvtimer->getReceiverPort());
            break;

        case 1:
            sendData(recvtimer->getReceiverName());
            break;

        case 2:
            close();
            break;

        case 3:
            renew(recvtimer->getReceiverName());
            break;
        }
    }
}


void SocketAgentAppTcp::connect(const char *receiverName, int receiverPort)
{
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
        int clientId = getModuleIdByName(receiverName);
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
                    ": cannot resolve destination address");
        }
        else {
            std::string receiverNameStr = receiverName;
            scheduler->log(nameStr + ": connecting to " + receiverNameStr + "(" + destination.str() + ") and port " + std::to_string(receiverPort));

            clientSocket.connect(destination, receiverPort);

            numSessions++;
            emit(connectSignal, 1L);
        }
        clientSockets[clientId] = clientSocket;
    } catch(...) {
        scheduler->log(nameStr + ": Error when trying to resolve L3 address");
        MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
        notification_message->setTransmission_error(true);
        notification_message->setSender(this->getParentModule()->getName());
        notification_message->setReceiver(receiverName);
        scheduler->sendToMosaik(notification_message);
    }
}

void SocketAgentAppTcp::renew(const char *receiver_name) {
    int clientId = getModuleIdByName(receiver_name);
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

void SocketAgentAppTcp::socketEstablished(inet::TcpSocket *socket) {
    std::string moduleName = scheduler->getModuleNameFromPort(socket->getRemotePort());
    if(moduleName != "") {
        Timer *timerMsg = getTimerForModuleId(getModuleIdByName(moduleName.c_str()));
        // type 1 means send
        timerMsg->setTimerType(1);
        timerMsg->setReceiverName(moduleName.c_str());
        scheduleAt(simTime(), timerMsg);
    }
}

void SocketAgentAppTcp::socketDataArrived(inet::TcpSocket *socket,
        inet::Packet *msg, bool urgent) {
    scheduler->log(nameStr + ": socket data arrived.");
    inet::Packet *copy = msg->dup();
    simtime_t delay;
    double delay_d = 0;
    std::string reply_receiver = "";

    packetsRcvd++;
    bytesRcvd += msg->getByteLength();
    emit(inet::packetReceivedSignal, msg);

    if (msg->getArrivalGate() == gate("socketIn")) {

        inet::Packet *recv_packet = dynamic_cast<inet::Packet *>(copy);
        if (recv_packet != nullptr) {
            std::string reply_content = "0";
            MosaikSchedulerMessage *answer = new MosaikSchedulerMessage();
            answer->setTransmission_error(false);

            inet::b offset = inet::b(0);  // start from the beginning
            bool found_acl_part = false;

            while (auto chunk = recv_packet->peekAt(offset)->dupShared()) {  // for each chunk
                if (found_acl_part) {
                    delete recv_packet;
                    break;
                }
                auto length = chunk->getChunkLength();

                if (chunk->getClassName() == std::string("MosaikApplicationChunk")) {
                    reply_content =
                            recv_packet->peekAt<MosaikApplicationChunk>(offset, length)->getContent();
                    simtime_t delay =
                            simTime() - recv_packet->peekAt<MosaikApplicationChunk>(offset, length)->getCreationTime();
                    int delay_i = ceil(delay.dbl()*1000);
                    reply_receiver =
                            recv_packet->peekAt<MosaikApplicationChunk>(offset, length)->getReceiver();
                    std::string reply_sender =
                            recv_packet->peekAt<MosaikApplicationChunk>(offset, length)->getSender();
                    std::string msgId =
                            recv_packet->peekAt<MosaikApplicationChunk>(offset, length)->getMsgId();
                    int creation_time =
                            recv_packet->peekAt<MosaikApplicationChunk>(offset, length)->getCreationTimeMosaik();
                    answer->setContent(reply_content.c_str());
                    answer->setReceiver(reply_receiver.c_str());
                    answer->setDelay(delay_i);
                    answer->setSender(reply_sender.c_str());
                    answer->setMsgId(msgId.c_str());
                    answer->setCreationTime(creation_time);
                    found_acl_part = true;
                    sendReply(answer);

                } else {
                    offset += chunk->getChunkLength();
                    answer->setDelay(delay_d);
                }
            }
            delete answer;
            delete msg;
        }
    }
}

void SocketAgentAppTcp::sendData(const char *receiver_name) {
    int clientId = getModuleIdByName(receiver_name);
    if(clientSockets.find(clientId) != clientSockets.end()) {
        inet::TcpSocket clientSocket;
        clientSocket = clientSockets[clientId];

        inet::Packet *packet;
        packet = getPacketForModuleId(clientId);

        if (packet) {
            inet::Packet *sending = packet->dup();
            sending->setTimestamp(packet->getTimestamp());
            int numBytes = sending->getByteLength();
            emit(inet::packetSentSignal, sending);

            clientSocket.send(sending);
            packetsSent++;
            bytesSent += numBytes;
            delete packet;
        }
    } else {
        scheduler->log(nameStr + ": socket not connected to " + receiver_name);
        connect(receiver_name, scheduler->getPortForModule(receiver_name));
    }

}


void SocketAgentAppTcp::sendReply(MosaikSchedulerMessage *reply) {
    scheduler->sendToMosaik(reply);
}

void SocketAgentAppTcp::socketClosed(inet::TcpSocket *socket) {
    scheduler->log(nameStr + ": socket closed.");

    TcpAppBase::socketClosed(socket);
    if (operationalState == State::STOPPING_OPERATION && !this->socket.isOpen())
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void SocketAgentAppTcp::socketFailure(inet::TcpSocket *socket, int code) {
    scheduler->log(nameStr + ": socket failure with code:" + std::to_string(code));
    TcpAppBase::socketFailure(socket, code);
    MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
    notification_message->setTransmission_error(true);
    notification_message->setSender(this->getParentModule()->getName());
    scheduler->sendToMosaik(notification_message);

}

void SocketAgentAppTcp::handleStartOperation(inet::LifecycleOperation *operation) {
    const char *localAddress = par("localAddress");
    int localPort = par("localPort");

    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(
            localAddress[0] ? inet::L3Address(localAddress) : inet::L3Address(),
                    localPort);
    serverSocket.listen();
}

void SocketAgentAppTcp::handleStopOperation(inet::LifecycleOperation *operation) {
    if (socket.isOpen()) close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void SocketAgentAppTcp::handleCrashOperation(inet::LifecycleOperation *operation) {
}

void SocketAgentAppTcp::finish() {
}
