/*
 * AgentAppUdp.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 *
 *  The AgentAppUdp represents the implementation of the application layer (and
 * transport layer) of the end devices, which represent the agents in mosaik on
 * the OMNeT++ side. The AgentAppUdp sends messages in OMNeT++ over UDP.
 *
 */

#include "AgentAppUdp.h"

#include <algorithm>
#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include <string.h>

#include "../messages/CosimaApplicationChunk_m.h"
#include "../messages/CosimaCtrlEvent_m.h"
#include "../messages/CosimaSchedulerMessage_m.h"
#include "../modules/CosimaScheduler.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "omnetpp/checkandcast.h"
#include "../util.h"

using namespace inet;
using namespace omnetpp;

Define_Module(AgentAppUdp);

AgentAppUdp::~AgentAppUdp()
{
    scheduler = nullptr;
}

void
AgentAppUdp::initialize(int stage)
{
    // get name of parent module as string for logging
    clientName = this->getParentModule()->getName();

    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    // get intern socket scheduler from simulation and cast to CosimaScheduler
    scheduler =
      check_and_cast<CosimaScheduler*>(getSimulation()->getScheduler());
    // register module at scheduler
    scheduler->registerModule(clientName, this);
    // initialize socket in simulation in OMNeT++ and bind to local port
    // local port can be set in .ini file
    socketudp.setOutputGate(gate("socketOut"));
    int localPort = par("localPort");
    socketudp.bind(localPort);
}

void
AgentAppUdp::handleMessage(cMessage* msg)
{
    // message can be from scheduler -> call handleSchedulerEvent()
    // or message from network -> handle message

    if (msg->getArrivalGate() != gate("socketIn")) {
        handleSchedulerMessage(check_and_cast<CosimaSchedulerMessage*>(msg));
        return;
    }

    inet::Packet* packet = check_and_cast<inet::Packet*>(msg);
    auto replyContent = "";
    auto answer = new CosimaSchedulerMessage();

    // calculate delay
    simtime_t delay =
      scheduler->getSimulation()->getSimTime() - msg->getCreationTime();
    scheduler->log(clientName + ": received message at time " + simTime().str() +
                   " with delay " + delay.str());
    int delay_i = to_mosaik_time(delay);

    // handle reply to scheduler
    inet::b offset = inet::b(0); // start from the beginning
    auto foundApplicationChunk = false;

    while (auto chunk = packet->peekAt(offset)->dupShared()) { // for each chunk
        if (foundApplicationChunk) {
            // message is from other SocketAgent
            delete packet;
            break;
        }
        auto length = chunk->getChunkLength();

        if (chunk->getClassName() == std::string("inet::SliceChunk")) {
            auto newPacket = packet->peekData<inet::SliceChunk>(0);
            auto encapsulatedChunk = newPacket->getChunk();
            auto appChunk = encapsulatedChunk->peek<CosimaApplicationChunk>(
              inet::b(0), encapsulatedChunk->getChunkLength());
            answer->setContent(appChunk->getContent());
            answer->setReceiver(appChunk->getReceiver());
            simtime_t delay = simTime() - appChunk->getCreationTimeOmnetpp();
            auto delay_i = 0U;
            delay_i = to_mosaik_time(delay);
            answer->setDelay(delay_i);
            answer->setSender(appChunk->getSender());
            answer->setMsgId(appChunk->getMsgId());
            answer->setCreationTime(appChunk->getCreationTimeCoupling());
            foundApplicationChunk = true;

        } else if (chunk->getClassName() ==
                   std::string("CosimaApplicationChunk")) {
            replyContent =
              packet->peekAt<CosimaApplicationChunk>(offset, length)
                ->getContent();
            auto replyReceiver =
              packet->peekAt<CosimaApplicationChunk>(offset, length)
                ->getReceiver();
            auto replySender =
              packet->peekAt<CosimaApplicationChunk>(offset, length)
                ->getSender();
            auto msgId = packet->peekAt<CosimaApplicationChunk>(offset, length)
                           ->getMsgId();
            auto creationTime =
              packet->peekAt<CosimaApplicationChunk>(offset, length)
                ->getCreationTimeCoupling();
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
                scheduler->log(
                  "Couldn't find CosimaApplicationChunk in packet: " +
                    packet->str(),
                  "warning");
                break;
            }
            answer->setDelay(delay_i);
        }
    }
    sendReply(answer);
    delete answer;
}

void
AgentAppUdp::handleSchedulerMessage(CosimaSchedulerMessage* msg)
{
    // get content from message
    std::string content = msg->getContent();
    auto receiverName = msg->getReceiver();
    auto senderName = msg->getSender();
    auto msgId = msg->getMsgId();
    auto msgSize = msg->getSize();
    auto creationTime = msg->getCreationTime();

    // get corresponding port for receiver name
    int receiverPort = scheduler->getPortForModule(receiverName);

    scheduler->log(clientName + ": send message " + msgId + +" to " +
                     receiverName + " with port " +
                     std::to_string(receiverPort) + " at time " +
                     std::to_string(creationTime),
                   "info");
    // scheduler->log("content is: " + contentStr);

    // make packet
    auto packet = new inet::Packet();
    const auto& payload = inet::makeShared<CosimaApplicationChunk>();
    payload->setContent(content.c_str());
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
    } catch (...) {
        scheduler->log(clientName + ": Error when trying to resolve L3 address",
                       "warning");
        CosimaSchedulerMessage* notificationMessage =
          new CosimaSchedulerMessage();
        notificationMessage->setTransmission_error(true);
        notificationMessage->setSender(clientName.c_str());
        notificationMessage->setReceiver(receiverName);
        scheduler->sendToCoupledSimulation(notificationMessage);
    }
    delete msg;
}

void
AgentAppUdp::sendReply(CosimaSchedulerMessage* reply)
{
    scheduler->sendToCoupledSimulation(reply);
}
