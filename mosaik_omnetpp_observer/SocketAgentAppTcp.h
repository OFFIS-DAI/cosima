/*
 * SocketAgentAppTcp.h
 *
 *  Created on: May 25, 2021
 *      Author: malin
 */

#ifndef SOCKETAGENTAPPTCP_H_
#define SOCKETAGENTAPPTCP_H_

#include <vector>
#include <algorithm>
#include "MosaikObserver.h"
#include "inet/applications/tcpapp/TcpAppBase.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include "inet/common/lifecycle/NodeStatus.h"


using namespace omnetpp;

class SocketAgentAppTcp : public inet::TcpAppBase {
private:
    MosaikObserver *observer;

    int numRecvBytes;
    inet::TcpSocket serverSocket;

public:
    SocketAgentAppTcp();
    virtual ~SocketAgentAppTcp();
    char recvBuffer[4000];
    void putMessage(cMessage *msg, double mosaikSimTime);

protected:
    int connRecv = 0;
    int connSend = 0;
    cMessage *timeoutMsg = nullptr;
    cMessage *timeoutMsgAlternative = nullptr;
    inet::Packet *packet = nullptr;

    /**
     * Initialize module and register at observer.
     */
    void initialize(int stage) override;
    /**
     * Return number of init stages.
     */
    int numInitStages() const override { return (inet::NUM_INIT_STAGES); }
    /**
     * Handle event from socket to mosaik.
     */
    void handleSocketEvent(cMessage *msg, double mosaikSimTime);
    /**
     * Handle reply with delay to mosaik.
     */
    void handleReply(TikTokPacket *reply);  // const char *reply
    /**
     * Handle timer for TCP connection.
     */
    virtual void handleTimer(cMessage *msg) override;
    /**
     * Is called as soon as socket connection is established.
     */
    virtual void socketEstablished(inet::TcpSocket *socket) override;
    /**
     * Data arrived at the socket over inet network.
     * The delay is to be calculated and send back to mosaik.
     */
    virtual void socketDataArrived(inet::TcpSocket *socket, inet::Packet *msg, bool urgent) override;
    /**
     * Socket is closed, cancel timer message.
     */
    virtual void socketClosed(inet::TcpSocket *socket) override;
    /**
     * Socket connection failed, cancel timer message.
     */
    virtual void socketFailure(inet::TcpSocket *socket, int code) override;
    /**
     * Set up server socket for incoming connections.
     */
    virtual void handleStartOperation(inet::LifecycleOperation *operation) override;
    /**
     * Cancel timer event in stop operation.
     */
    virtual void handleStopOperation(inet::LifecycleOperation *operation) override;
    /**
     * Cancel timer event in crash operation.
     */
    virtual void handleCrashOperation(inet::LifecycleOperation *operation) override;
    /**
     * Send data over inet network.
     */
    virtual void sendData();
    /**
     * Renew server socket for new incoming connections.
     */
    void renew();

    /**
     * Is to be called at the end of the simulation.
     */
    virtual void finish() override;
};




#endif /* SOCKETAGENTAPPTCP_H_ */
