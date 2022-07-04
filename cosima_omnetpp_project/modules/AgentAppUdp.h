#ifndef AGENTAPPUDP_H
#define AGENTAPPUDP_H

#pragma once

#include <omnetpp.h>
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "../modules/MosaikScheduler.h"
using namespace inet;

using namespace omnetpp;

class AgentAppUdp : public cSimpleModule {
private:
    inet::UdpSocket socketudp;

    cGate *connectGate;

    std::string nameStr;

public:
    AgentAppUdp(){}
    ~AgentAppUdp();

protected:

    MosaikScheduler *scheduler;
    /**
     * Handles incoming messages.
     * Messages can either be from scheduler (from mosaik) or
     * they can be received via the inet network.
     */
    void handleMessage(cMessage *msg) override;
    /**
     * This method is called whenever an incoming message is
     * a message from the scheduler. In that case the message is now
     * forwarded over the network in OMNeT++.
     */
    void handleSocketEvent(cMessage *msg);
    /**
     * Send a reply to the scheduler after sending a message
     * over the network.
     */
    void sendReply(MosaikSchedulerMessage *reply);

    /**
     * Initializes the app when called in the stage of application layer.
     */
    void initialize(int stage) override;
    /**
     *      * Returns the number of init stages.
     */
    int numInitStages() const override { return (inet::NUM_INIT_STAGES); }
    /**
     * Called when data arrives at socket.
     */

};

#endif /* AGENTAPP_H */
