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

#include "../messages/CosimaApplicationChunk_m.h"
#include "../messages/CosimaCtrlEvent_m.h"
#include "../modules/CosimaScheduler.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "../messages/CosimaSchedulerMessage_m.h"

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

    // get intern socket scheduler from simulation and cast to CosimaScheduler
    scheduler = check_and_cast<CosimaScheduler *>(getSimulation()->getScheduler());
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
            auto answer = new CosimaSchedulerMessage();

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

                if (chunk->getClassName() == std::string("inet::SliceChunk")) {
                    auto newPacket = packet->peekData<inet::SliceChunk>(0);
                    auto encapsulatedChunk = newPacket->getChunk();
                    auto appChunk = encapsulatedChunk->peek<CosimaApplicationChunk>(inet::b(0), encapsulatedChunk->getChunkLength());
                    answer->setContent(appChunk->getContent());
                    answer->setReceiver(appChunk->getReceiver());
                    simtime_t delay = simTime() - appChunk->getCreationTimeOmnetpp();
                    auto delay_i = 0U;
                    delay_i = ceil(delay.dbl()*1000);
                    answer->setDelay(delay_i);
                    answer->setSender(appChunk->getSender());
                    answer->setMsgId(appChunk->getMsgId());
                    answer->setCreationTime(appChunk->getCreationTimeCoupling());
                    foundApplicationChunk = true;

                } else if (chunk->getClassName() == std::string("CosimaApplicationChunk")) {
                    replyContent =
                            packet->peekAt<CosimaApplicationChunk>(offset, length)->getContent();
                    auto replyReceiver =
                            packet->peekAt<CosimaApplicationChunk>(offset, length)->getReceiver();
                    auto replySender =
                            packet->peekAt<CosimaApplicationChunk>(offset, length)->getSender();
                    auto msgId =
                            packet->peekAt<CosimaApplicationChunk>(offset, length)->getMsgId();
                    auto creationTime =
                            packet->peekAt<CosimaApplicationChunk>(offset, length)->getCreationTimeCoupling();
                    answer->setContent(replyContent);
                    answer->setReceiver(replyReceiver);
                    answer->setDelay(delay_i);
                    answer->setSender(replySender);
                    answer->setMsgId(msgId);
                    answer->setCreationTime(creationTime);
                    foundApplicationChunk = true;
                } else {
                    offset += chunk->getChunkLength();
                    if (offset >= packet->getTotalLength()) {
                        scheduler->log("Couldn't find CosimaApplicationChunk in packet: " + packet->str(), "warning");
                        break;
                    }
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
    if (typeid(*msg) == typeid(CosimaSchedulerMessage)) {
        CosimaSchedulerMessage *msgCasted = dynamic_cast<CosimaSchedulerMessage *>(msg);

        // get content from message
        auto content = msgCasted->getContent();
        auto receiverName = msgCasted->getReceiver();
        auto senderName = msgCasted->getSender();
        auto msgId = msgCasted->getMsgId();
        auto msgSize = msgCasted->getSize();
        auto creationTime = msgCasted->getCreationTime();

        // get corresponding port for receiver name
        int receiverPort = scheduler->getPortForModule(receiverName);
        std::string contentStr = content;

        scheduler->log(nameStr + ": send message " + msgId + + " to " + receiverName + " with port " + std::to_string(receiverPort) + " at time "
                        + std::to_string(creationTime), "info");
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
        payload->setCreationTimeCoupling(creationTime);
        packet->insertAtBack(payload);

        // get destination
        inet::L3Address destAddress;
        try {
            destAddress = inet::L3AddressResolver().resolve(receiverName);
            // send packet
            socketudp.sendTo(packet, destAddress, receiverPort);
        } catch(...) {
            scheduler->log(nameStr + ": Error when trying to resolve L3 address", "warning");
            CosimaSchedulerMessage *notificationMessage = new CosimaSchedulerMessage();
            notificationMessage->setTransmission_error(true);
            notificationMessage->setSender(nameStr.c_str());
            notificationMessage->setReceiver(receiverName);
            scheduler->sendToCoupledSimulation(notificationMessage);
        }
        delete msgCasted;
    } else {
        delete msg;
    }
}

void AgentAppUdp::sendReply(CosimaSchedulerMessage *reply) {
    scheduler->sendToCoupledSimulation(reply);
}

