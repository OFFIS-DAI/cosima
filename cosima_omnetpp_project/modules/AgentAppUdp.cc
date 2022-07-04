/*
 * AgentAppUdp.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 *
 *  The AgentAppUdp represents the implementation of the application layer (and transport layer)
 *  of the end devices, which represent the agents in mosaik on the OMNeT++ side.
 *  The AgentAppUdp sends messages in OMNeT++ over UDP.
 *
 */

#include "AgentAppUdp.h"

#include <string.h>
#include <omnetpp.h>
#include <algorithm>
#include <omnetpp/platdep/sockets.h>

#include "../modules/MosaikScheduler.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "../messages/MosaikApplicationChunk_m.h"
#include "../messages/MosaikSchedulerMessage_m.h"
#include "../messages/MosaikCtrlEvent_m.h"

using namespace inet;
using namespace omnetpp;

Define_Module(AgentAppUdp);

AgentAppUdp::~AgentAppUdp() {
    scheduler = nullptr;
}

void AgentAppUdp::initialize(int stage) {
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
}


void AgentAppUdp::handleMessage(cMessage *msg) {
    // message can be socket event -> call handleSocketEvent()
    // or message from network -> handle message

    if (msg->getArrivalGate() == gate("socketIn")) {
        inet::Packet *packet = dynamic_cast<inet::Packet *>(msg);
        if (packet != nullptr) {
            auto replyContent = "";
            auto answer = new MosaikSchedulerMessage();

            // calculate delay
            simtime_t delay =
                    scheduler->getSimulation()->getSimTime() - msg->getCreationTime();
            scheduler->log(nameStr + ": received message at time " + simTime().str() + " with delay " + delay.str());
            auto delay_i = 0U;
            delay_i = ceil(delay.dbl()*1000);

            // handle reply to scheduler
            inet::b offset = inet::b(0);  // start from the beginning
            auto foundApplicationChunk = false;

            while (auto chunk=packet->peekAt(offset)->dupShared()) {  // for each chunk
                if (foundApplicationChunk) {
                    // message is from other SocketAgent
                    delete packet;
                    break;
                }
                auto length = chunk->getChunkLength();

                if (chunk->getClassName() == std::string("MosaikApplicationChunk")) {
                    replyContent =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getContent();
                    auto replyReceiver =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getReceiver();
                    auto replySender =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getSender();
                    auto msgId =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getMsgId();
                    auto creationTime =
                            packet->peekAt<MosaikApplicationChunk>(offset, length)->getCreationTimeMosaik();
                    answer->setContent(replyContent);
                    answer->setReceiver(replyReceiver);
                    answer->setDelay(delay_i);
                    answer->setSender(replySender);
                    answer->setMsgId(msgId);
                    answer->setCreationTime(creationTime);
                    foundApplicationChunk = true;
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

void AgentAppUdp::handleSocketEvent(cMessage *msg) {
    // make packet
    if (typeid(*msg) == typeid(MosaikSchedulerMessage)) {
        MosaikSchedulerMessage *msgCasted = dynamic_cast<MosaikSchedulerMessage *>(msg);

        // get content from message
        auto content = msgCasted->getContent();
        auto receiverName = msgCasted->getReceiver();
        auto senderName = msgCasted->getSender();
        auto msgId = msgCasted->getMsgId();
        auto msgSize = msgCasted->getSize();
        auto creationTime = msgCasted->getCreationTime();

        // get corresponding port for receiver name
        int receiverPort = scheduler->getPortForModule(receiverName);

        scheduler->log(nameStr + ": send message with content '" +
                content + "' to " + receiverName + " with port " + std::to_string(receiverPort) + " at time " + simTime().str() + " with id " + msgId);

        // make packet
        auto packet = new inet::Packet();
        const auto &payload = inet::makeShared<MosaikApplicationChunk>();
        payload->setContent(content);
        payload->setReceiver(receiverName);
        payload->setSender(senderName);
        payload->setChunkLength(inet::B(msgSize));
        payload->setCreationTime(simTime());
        payload->setMsgId(msgId);
        payload->setCreationTimeMosaik(creationTime);
        packet->insertAtBack(payload);

        // get destination
        inet::L3Address destAddress;
        try {
            destAddress = inet::L3AddressResolver().resolve(receiverName);
            // send packet
            socketudp.sendTo(packet, destAddress, receiverPort);
        } catch(...) {
            scheduler->log(nameStr + ": Error when trying to resolve L3 address");
            MosaikSchedulerMessage *notificationMessage = new MosaikSchedulerMessage();
            notificationMessage->setTransmission_error(true);
            notificationMessage->setSender(nameStr.c_str());
            notificationMessage->setReceiver(receiverName);
            scheduler->sendToMosaik(notificationMessage);
        }
        delete msgCasted;
    } else {
        delete msg;
    }
}

void AgentAppUdp::sendReply(MosaikSchedulerMessage *reply) {
    scheduler->sendToMosaik(reply);
}

