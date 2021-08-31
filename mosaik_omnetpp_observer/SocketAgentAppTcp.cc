/*
 * SocketAgentAppTcp.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */
#include <cmath>

#include "SocketAgentAppTcp.h"

#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include <string.h>
#include <algorithm>
#include "MosaikMessage_m.h"
#include "MosaikObserver.h"
#include "TikTokPacket_m.h"
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
    numRecvBytes = 0;
    observer = nullptr;
}

SocketAgentAppTcp::~SocketAgentAppTcp() {
    //delete and cancel all timers used
    auto iter = timer.begin();

    while (iter != timer.end()) {
        std::cout << "Cancel timer for client" << iter->first << "\n";
        cancelAndDelete(iter->second);
        ++iter;
    }
}

void SocketAgentAppTcp::initialize(int stage) {
    TcpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        // get intern socket scheduler from simulation and cast to MosaikObserver
        observer =
                check_and_cast<MosaikObserver *>(getSimulation()->getScheduler());
        // register module at scheduler
        observer->setInterfaceModule(this, recvBuffer, 4000, &numRecvBytes);
    }
}

void SocketAgentAppTcp::putMessage(cMessage *msg, double mosaikSimTime) {
    // message can be socket event -> call handleSocketEvent()
    // or message from network -> handle message
    std::cout << "SocketAgentAppTcp of " << this->getParentModule()->getName()
                                                                                                                                                            << " received message " << msg << endl;

    if (msg->getKind() == 0) {
        if (msg->getArrivalGate() != gate("socketIn")) {
            // message is placed from observer
            handleSocketEvent(msg, mosaikSimTime);
        }
    }else{
        delete msg;
    }
}

void SocketAgentAppTcp::handleSocketEvent(cMessage *msg, double mosaikSimTime) {
    // make packet
    if (typeid(*msg) == typeid(TikTokPacket)) {
        std::cout << "Message is TikTokPacket" << endl;
        TikTokPacket *tmsg = dynamic_cast<TikTokPacket *>(msg);
        const char *content = tmsg->getContent();

        std::cout << "SocketAgentAppTcp: Handle socket event, msg: " << content << endl;

        std::string receiverName = tmsg->getReceiver();
        std::string senderName = tmsg->getSender();
        int msgSize = tmsg->getSize();

        // get corresponding Port for receiver name
        int receiverPort = observer->getPortForModule(receiverName.c_str());

        std::cout << "SocketAgentAppTcp: Message receiver: " << receiverName << ", message sender: " << senderName
                << ", msgSize: " << msgSize << ", receiverPort: " << receiverPort
                << endl;

        packet = new inet::Packet();
        const auto &payload = inet::makeShared<MosaikMessage>();
        payload->setContent(content);
        payload->setReceiver(receiverName.c_str());
        payload->setSender(senderName.c_str());
        payload->setChunkLength(inet::B(msgSize));
        std::cout << "SocketAgentAppTcp: set CreationTime to " << observer->getSimulation()->getSimTime() << endl;
        payload->setCreationTime(mosaikSimTime);
        packet->insertAtBack(payload);
        packet->setTimestamp(mosaikSimTime);
        packets.push_back(packet);

        int clientId = atoi(&receiverName[6]);

        Timer *timerMsg = getTimerForModuleId(clientId);
        // type 0 means connect
        timerMsg->setTimerType(0);
        timerMsg->setReceiverName(receiverName.c_str());
        timerMsg->setReceiverPort(receiverPort);
        timerMsg->setArrival(this->getId(), -1, simTime());
        observer->getSimulation()->getFES()->insert(timerMsg);

        std::cout << "SocketAgentAppTcp: send packet " << packet << " to port " << receiverPort
                << endl;
        delete msg;
    }
}

Timer *SocketAgentAppTcp::getTimerForModuleId(int clientId) {
    auto iter = timer.begin();

    // if timer is already created for client id, return it
    while (iter != timer.end()) {
        if(iter->first == clientId) {
            return iter->second;
        }
        iter++;
    }

    // else create new timer and add it to the timer map
    std::cout << "creating new timer" << endl;
    Timer *newTimer = new Timer();
    timer[clientId] = newTimer;
    return newTimer;
}

void SocketAgentAppTcp::handleReply(TikTokPacket *reply) {
    std::cout << "Set message for observer " << reply << endl;
    observer->sendBytes(reply);
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


void SocketAgentAppTcp::connect(const char *receiver_name, int receiver_port)
{
    std::cout << "SocketAgentAppTcp: connect " << this->getParentModule()->getName() << " to " << receiver_name << endl;
    inet::TcpSocket clientSocket;
    int clientId = atoi(&receiver_name[6]);
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
        int clientId = atoi(&receiver_name[6]);
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
    inet::L3AddressResolver().tryResolve(receiver_name, destination);
    if (destination.isUnspecified()) {
        std::cout << "SocketAgentAppTcp: Connecting to " << receiver_name << " port=" << receiver_port << ": cannot resolve destination address\n";
    }
    else {
        std::cout << "SocketAgentAppTcp: Connecting to " << receiver_name << "(" << destination << ") port=" << receiver_port << endl;

        clientSocket.connect(destination, receiver_port);

        numSessions++;
        emit(connectSignal, 1L);
    }
    clientSockets[clientId] = clientSocket;

}

void SocketAgentAppTcp::renew(const char *receiver_name) {
    int clientId = atoi(&receiver_name[6]);
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
    std::string moduleName = observer->getModuleNameFromPort(socket->getRemotePort());
    if(moduleName != "") {
        Timer *timerMsg = getTimerForModuleId(atoi(&moduleName[6]));
        // type 1 means send
        timerMsg->setTimerType(1);
        timerMsg->setReceiverName(moduleName.c_str());
        timerMsg->setArrival(this->getId(), -1, simTime());
        observer->getSimulation()->getFES()->insert(timerMsg);
    }
}

void SocketAgentAppTcp::socketDataArrived(inet::TcpSocket *socket,
        inet::Packet *msg, bool urgent) {
    std::cout << "SocketAgentAppTcp: socket data " << msg << "arrived at " << this->getParentModule()->getName() << endl;
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
            TikTokPacket *answer = new TikTokPacket();

            // message is ACL message from other node in simulation over UDP socket
            // now handle reply to Mango
            inet::b offset = inet::b(0);  // start from the beginning
            bool found_acl_part = false;

            while (auto chunk = recv_packet->peekAt(offset)->dupShared()) {  // for each chunk
                if (found_acl_part) {
                    delete recv_packet;
                    break;
                }
                auto length = chunk->getChunkLength();

                if (chunk->getClassName() == std::string("MosaikMessage")) {
                    reply_content =
                            recv_packet->peekAt<MosaikMessage>(offset, length)->getContent();
                    simtime_t delay =
                            observer->getSimulation()->getSimTime() - recv_packet->peekAt<MosaikMessage>(offset, length)->getCreationTime();
                    double delay_d = round((delay).dbl()*1000)/1000;
                    reply_receiver =
                            recv_packet->peekAt<MosaikMessage>(offset, length)->getReceiver();
                    std::string reply_sender =
                            recv_packet->peekAt<MosaikMessage>(offset, length)->getSender();
                    answer->setContent(reply_content.c_str());
                    answer->setReceiver(reply_receiver.c_str());
                    answer->setDelay(delay_d);
                    answer->setSender(reply_sender.c_str());
                    found_acl_part = true;
                    handleReply(answer);

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
    int clientId = atoi(&receiver_name[6]);
    if(clientSockets.find(clientId) != clientSockets.end()) {
        inet::TcpSocket clientSocket;
        clientSocket = clientSockets[clientId];

        if (packets.size() > 0) {
            packet = packets.front();
            packets.pop_front();
            inet::Packet *sending = packet->dup();
            sending->setTimestamp(packet->getTimestamp());
            int numBytes = sending->getByteLength();
            emit(inet::packetSentSignal, sending);
            clientSocket.send(sending);

            packetsSent++;
            bytesSent += numBytes;
            delete packet;
        } else {
            std::cout << "SocketAgentAppTcp: no packet to send" << endl;
        }
    } else {
        std::cout << "SocketAgentAppTcp: socket not connected to " << receiver_name << endl;
        connect(receiver_name, observer->getPortForModule(receiver_name));
    }

}

void SocketAgentAppTcp::socketClosed(inet::TcpSocket *socket) {
    std::cout << "SocketAgentAppTcp: socket closed " << endl;
    TcpAppBase::socketClosed(socket);
    if (operationalState == State::STOPPING_OPERATION && !this->socket.isOpen())
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void SocketAgentAppTcp::socketFailure(inet::TcpSocket *socket, int code) {
    std::cout << "SocketAgentAppTcp: socket failure" << endl;
    TcpAppBase::socketFailure(socket, code);
}

void SocketAgentAppTcp::handleStartOperation(
        inet::LifecycleOperation *operation) {
    const char *localAddress = par("localAddress");
    int localPort = par("localPort");

    serverSocket.setOutputGate(gate("socketOut"));
    serverSocket.setCallback(this);
    serverSocket.bind(
            localAddress[0] ? inet::L3Address(localAddress) : inet::L3Address(),
                    localPort);
    serverSocket.listen();
}

void SocketAgentAppTcp::handleStopOperation(
        inet::LifecycleOperation *operation) {
    if (socket.isOpen()) close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void SocketAgentAppTcp::handleCrashOperation(
        inet::LifecycleOperation *operation) {
}

void SocketAgentAppTcp::finish() {
}
