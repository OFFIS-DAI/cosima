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

#include "../modules/MosaikScheduler.h"
#include "../messages/MosaikApplicationChunk_m.h"
#include "../messages/MosaikSchedulerMessage_m.h"
#include "../messages/MosaikCtrlEvent_m.h"
#include "AgentAppTcp.h"


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
            scheduler->log(nameStr + ": cancel timer for client with ID " + std::to_string(iter->first));
            cancelAndDelete(*it);
        }
        ++iter;
    }
}

void AgentAppTcp::initialize(int stage) {
    TcpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        // get intern socket scheduler from simulation and cast to MosaikScheduler
        scheduler =
                check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());
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
        // message is an event from the scheduler (from mosaik)
        handleSocketEvent(msg, msg->getArrivalTime().dbl());
    }
    else {
        // message is an event at the socket in the network
        TcpAppBase::socket.processMessage(msg);
    }
}

bool AgentAppTcp::handleSocketEvent(cMessage *msg, double mosaikSimTime) {

    // mosaik simTime should have positive value
    if (mosaikSimTime < 0) {
        delete msg;
        return false;
    }

    // make packet
    if (typeid(*msg) == typeid(MosaikSchedulerMessage)) {
        MosaikSchedulerMessage *msgCasted = dynamic_cast<MosaikSchedulerMessage *>(msg);
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
            delete msg;
            return false;
        }
        scheduler->log(nameStr + ": send message with content '" +
                content + "' and msgId " + msgId + " to " + receiverName + " with port " + std::to_string(receiverPort) + " at time "
                + std::to_string(mosaikSimTime));

        // make packet
        auto packet = new inet::Packet();
        const auto &payload = inet::makeShared<MosaikApplicationChunk>();
        payload->setContent(content);
        payload->setReceiver(receiverName);
        payload->setSender(senderName);
        payload->setChunkLength(inet::B(msgSize));
        payload->setCreationTime(mosaikSimTime);
        payload->setMsgId(msgId);
        payload->setCreationTimeMosaik(creation_time);
        packet->insertAtBack(payload);
        packet->setTimestamp(mosaikSimTime);

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
    packetToClient[clientId] = packet;
}

inet::Packet *AgentAppTcp::getPacketForModuleId(int clientId) {
    auto iter = packetToClient.begin();

    // if timer is already created for client id, return it
    while (iter != packetToClient.end()) {
        if (iter->first == clientId) {
            auto packet = packetToClient[clientId];
            return packet;
        }
        ++iter;
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
    } else {
        throw cRuntimeError("AgentAppTcp: called handleTimer with non-Timer object.");
    }
}


void AgentAppTcp::connect(const char *receiverName, int receiverPort)
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
    auto moduleName = scheduler->getModuleNameFromPort(socket->getRemotePort());
    if(moduleName != "") {
        auto timerMsg = getTimerForModuleId(getModuleIdByName(moduleName.c_str()));
        // type 1 means send
        timerMsg->setTimerType(1);
        timerMsg->setReceiverName(moduleName.c_str());
        scheduleAt(simTime(), timerMsg);
    }
}

void AgentAppTcp::socketDataArrived(inet::TcpSocket *socket,
        inet::Packet *msg, bool urgent) {
    scheduler->log(nameStr + ": socket data arrived.");
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
            MosaikSchedulerMessage *answer = new MosaikSchedulerMessage();
            answer->setTransmission_error(false);

            inet::b offset = inet::b(0);  // start from the beginning
            auto foundApplicationChunk = false;

            while (auto chunk = recvPacket->peekAt(offset)->dupShared()) {  // for each chunk
                if (foundApplicationChunk) {
                    delete recvPacket;
                    break;
                }
                auto length = chunk->getChunkLength();

                if (chunk->getClassName() == std::string("MosaikApplicationChunk")) {
                    replyContent =
                            recvPacket->peekAt<MosaikApplicationChunk>(offset, length)->getContent();
                    simtime_t delay =
                            simTime() - recvPacket->peekAt<MosaikApplicationChunk>(offset, length)->getCreationTime();
                    auto delay_i = 0U;
                    delay_i = ceil(delay.dbl()*1000);
                    replyReceiver =
                            recvPacket->peekAt<MosaikApplicationChunk>(offset, length)->getReceiver();
                    auto replySender =
                            recvPacket->peekAt<MosaikApplicationChunk>(offset, length)->getSender();
                    auto msgId =
                            recvPacket->peekAt<MosaikApplicationChunk>(offset, length)->getMsgId();
                    auto creationTime =
                            recvPacket->peekAt<MosaikApplicationChunk>(offset, length)->getCreationTimeMosaik();
                    answer->setContent(replyContent);
                    answer->setReceiver(replyReceiver);
                    answer->setDelay(delay_i);
                    answer->setSender(replySender);
                    answer->setMsgId(msgId);
                    answer->setCreationTime(creationTime);
                    foundApplicationChunk = true;
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

void AgentAppTcp::sendData(const char *receiverName) {
    int clientId = getModuleIdByName(receiverName);
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
        scheduler->log(nameStr + ": socket not connected to " + receiverName);
        connect(receiverName, scheduler->getPortForModule(receiverName));
    }

}


void AgentAppTcp::sendReply(MosaikSchedulerMessage *reply) {
    scheduler->sendToMosaik(reply);
}

void AgentAppTcp::socketClosed(inet::TcpSocket *socket) {
    scheduler->log(nameStr + ": socket closed.");

    TcpAppBase::socketClosed(socket);
    if (operationalState == State::STOPPING_OPERATION && !this->socket.isOpen())
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void AgentAppTcp::socketFailure(inet::TcpSocket *socket, int code) {
    scheduler->log(nameStr + ": socket failure with code:" + std::to_string(code));
    TcpAppBase::socketFailure(socket, code);
    auto notificationMessage = new MosaikSchedulerMessage();
    notificationMessage->setTransmission_error(true);
    notificationMessage->setSender(this->getParentModule()->getName());
    scheduler->sendToMosaik(notificationMessage);

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
