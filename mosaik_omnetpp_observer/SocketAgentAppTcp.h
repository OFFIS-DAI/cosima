/*
 * SocketAgentAppTcp.h
 *
 *  Created on: May 25, 2021
 *      Author: malin
 *
 *  The SocketAgentAppTCP represents the implementation of the application layer (and transport layer)
 *  of the end devices, which represent the agents in mosaik on the OMNeT++ side.
 *  The SocketAgentAppTCP sends messages in OMNeT++ over TCP.
 */

#ifndef SOCKETAGENTAPPTCP_H_
#define SOCKETAGENTAPPTCP_H_

#include <vector>
#include <algorithm>
#include "MosaikScheduler.h"
#include "messages/Timer_m.h"
#include "inet/applications/tcpapp/TcpAppBase.h"
#include "inet/common/lifecycle/LifecycleOperation.h"
#include "inet/common/lifecycle/NodeStatus.h"


using namespace omnetpp;

class SocketAgentAppTcp : public inet::TcpAppBase {
private:
    MosaikScheduler *scheduler;
    inet::TcpSocket serverSocket;
    std::map<int, inet::TcpSocket> clientSockets;
    std::map<int,std::list<Timer *>> timer;

public:
    SocketAgentAppTcp();
    virtual ~SocketAgentAppTcp();

protected:
    std::map<int, inet::Packet *> packetToClient;

    /**
     * Initialize module and register at Scheduler.
     */
    void initialize(int stage) override;
    /**
     * Return number of init stages.
     */
    int numInitStages() const override { return (inet::NUM_INIT_STAGES); }
    /**
     * Overwrites message of TcpAppBase to be able to receive messages from MosaikScheduler
     */
    void handleMessageWhenUp(cMessage *msg);
    /**
     * Handle event from socket to mosaik.
     */
    void handleSocketEvent(cMessage *msg, double mosaikSimTime);
    /**
     * Handle reply with delay to mosaik.
     */
    void sendReply(MosaikSchedulerMessage *reply);  // const char *reply
    /**
     * Timer objects are saved in a map, this method returns the timer for a given client id
     */
    Timer *getTimerForModuleId(int clientId);
    /**
     * Packets are saved in a map, this method returns the packet for a given client id.
     * If no packet is saved for that id, the method returns nullptr.
     */
    void setPacketForModuleId(int clientId, inet::Packet*);
    /**
     * Packets are saved in a map, this method saves the packet for a given client id.
     * If no packet is saved for that id, the method returns nullptr.
     */
    inet::Packet *getPacketForModuleId(int clientId);
    /**
     * Convenience method to get id of module by module name.
     * Returns ID of module if module can be found as a registered module at the MosaikObserver,
     * otherwise returns -1.
     */
    int getModuleIdByName(const char *module_name);
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
    virtual void sendData(const char *receiver_name);
    /**
     * Renew server socket for new incoming connections.
     */
    void renew(const char *receiver_name);

    /**
     * Is called to connect to another client.
     */
    void connect(const char *receiver_name, int receiver_port);

    /**
     * Is to be called at the end of the simulation.
     */
    virtual void finish() override;


};




#endif /* SOCKETAGENTAPPTCP_H_ */
