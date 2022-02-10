/*
 * SocketAgentApp.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 *
 *  The SocketAgentApp represents the implementation of the application layer (and transport layer)
 *  of the end devices, which represent the agents in mosaik on the OMNeT++ side.
 *  The SocketAgentApp sends messages in OMNeT++ over UDP.
 *
 */
#include <string.h>
#include <omnetpp.h>
#include <algorithm>
#include <omnetpp/platdep/sockets.h>
#include "MosaikScheduler.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "messages/MosaikApplicationChunk_m.h"
#include "messages/MosaikSchedulerMessage_m.h"
#include "SocketAgentApp.h"
#include "messages/MosaikCtrlEvent_m.h"

using namespace inet;
using namespace omnetpp;

Define_Module(SocketAgentApp);

SocketAgentApp::~SocketAgentApp() {
    scheduler = nullptr;
}

void SocketAgentApp::initialize(int stage) {
    // get name of parent module as string for logging
    nameStr = this->getParentModule()->getName();

    if (stage != inet::INITSTAGE_APPLICATION_LAYER) return;

    // get intern socket scheduler from simulation and cast to MosaikScheduler
    scheduler = check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());
    // register module at scheduler
    scheduler->setInterfaceModule(this, false);
    // initialize socket in simulation in OMNeT++ and bind to local port
    // local port can be set in .ini file
    socketudp.setOutputGate(gate("socketOut"));
    int localPort = par("localPort");
    socketudp.bind(localPort);
    socketudp.setCallback(this);
}

void SocketAgentApp::socketDataArrived(UdpSocket *socket, Packet *packet) {
    scheduler->log("SocketAgentApp: socket data arrived");
}

void SocketAgentApp::handleMessage(cMessage *msg) {
    // message can be socket event -> call handleSocketEvent()
    // or message from network -> handle message

    if (msg->getArrivalGate() == gate("socketIn")) {
        inet::Packet *packet = dynamic_cast<inet::Packet *>(msg);
        if (packet != nullptr) {
            std::string reply_content = "0";
            MosaikSchedulerMessage *answer = new MosaikSchedulerMessage();

            // calculate delay
            simtime_t delay =
                    scheduler->getSimulation()->getSimTime() - msg->getCreationTime();
            scheduler->log(nameStr + ": received message at time " + simTime().str() + " with delay " + delay.str());
            int delay_i = ceil(delay.dbl()*1000);

            // handle reply to scheduler
            inet::b offset = inet::b(0);  // start from the beginning
            bool found_acl_part = false;

            while (auto chunk =
                    packet->peekAt(offset)->dupShared()) {  // for each chunk
                if (found_acl_part) {
                    // message is from other SocketAgent
                    delete packet;
                    break;
                }
                auto length = chunk->getChunkLength();

                if (chunk->getClassName() == std::string("MosaikApplicationChunk")) {
                    reply_content =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getContent();
                    std::string reply_receiver =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getReceiver();
                    std::string reply_sender =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getSender();
                    std::string msgId =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getMsgId();
                    int creationTime =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getCreationTimeMosaik();
                    answer->setContent(reply_content.c_str());
                    answer->setReceiver(reply_receiver.c_str());
                    answer->setDelay(delay_i);
                    answer->setSender(reply_sender.c_str());
                    answer->setMsgId(msgId.c_str());
                    answer->setCreationTime(creationTime);
                    found_acl_part = true;
                } else {
                    offset += chunk->getChunkLength();
                    answer->setDelay(delay_i);
                }
            }
            sendReply(answer);
            delete answer;
        }

    } else {
        // message is placed from scheduler
        handleSocketEvent(msg);
    }
}

void SocketAgentApp::handleSocketEvent(cMessage *msg) {
    // make packet
    if (typeid(*msg) == typeid(MosaikSchedulerMessage)) {
        MosaikSchedulerMessage *msg_casted = dynamic_cast<MosaikSchedulerMessage *>(msg);

        // get content from message
        const char *content = msg_casted->getContent();
        std::string receiverName = msg_casted->getReceiver();
        std::string senderName = msg_casted->getSender();
        std::string msgId = msg_casted->getMsgId();
        int msgSize = msg_casted->getSize();
        int creation_time = msg_casted->getCreationTime();

        // get corresponding port for receiver name
        int receiverPort = scheduler->getPortForModule(receiverName.c_str());

        scheduler->log(nameStr + ": send message with content '" +
                content + "' to " + receiverName + " with port " + std::to_string(receiverPort) + " at time " + simTime().str() + " with id " + msgId);

        // make packet
        inet::Packet *packet = new inet::Packet();
        const auto &payload = inet::makeShared<MosaikApplicationChunk>();
        payload->setContent(content);
        payload->setReceiver(receiverName.c_str());
        payload->setSender(senderName.c_str());
        payload->setChunkLength(inet::B(msgSize));
        payload->setCreationTime(simTime());
        payload->setMsgId(msgId.c_str());
        payload->setCreationTimeMosaik(creation_time);
        packet->insertAtBack(payload);

        // get destination
        inet::L3Address destAddress;
        try {
            destAddress = inet::L3AddressResolver().resolve(receiverName.c_str());
            // send packet
            socketudp.sendTo(packet, destAddress, receiverPort);
        } catch(...) {
            scheduler->log(nameStr + ": Error when trying to resolve L3 address");
            MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
            notification_message->setTransmission_error(true);
            notification_message->setSender(this->getParentModule()->getName());
            notification_message->setReceiver(receiverName.c_str());
            scheduler->sendToMosaik(notification_message);
        }
        delete msg_casted;
    } else {
        delete msg;
    }
}

void SocketAgentApp::socketErrorArrived(UdpSocket *socket, Indication *indication){
    scheduler->log("socketErrorArrived");
    MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
    notification_message->setTransmission_error(true);
    notification_message->setSender(this->getParentModule()->getName());
    scheduler->sendToMosaik(notification_message);
}

void SocketAgentApp::socketClosed(UdpSocket *socket) {
    scheduler->log("socketClosed");
}

void SocketAgentApp::sendReply(MosaikSchedulerMessage *reply) {
    scheduler->sendToMosaik(reply);
}

