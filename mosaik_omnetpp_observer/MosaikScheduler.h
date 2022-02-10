/*
 * MosaikScheduler.h
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 *
 *  The MosaikScheduler replaces the OMNeT++ internal scheduler.
 *  In addition to the usual functions of a scheduler,
 *  a TCP connection to mosaik can be established and incoming messages
 *  can be inserted into the simulation as events.
 *
 */

#ifndef __MOSAIKSCHEDULER_H__
#define __MOSAIKSCHEDULER_H__

#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include "messages/message.pb.h"
#include "messages/MosaikSchedulerMessage_m.h"

class MosaikScheduler : public omnetpp::cScheduler
{
public:
    int port;
    // indicates whether simulation end is reached in mosaik
    bool until_reached = false;

    bool initial_message_received = false;
    /**
     * Method can be called from modules in order to get matching port for module name.
     */
    int getPortForModule(std::string module_name);

protected:
    omnetpp::cModule *schedulerModule;

    omnetpp::cModule *scenarioManager;

    // save time of last event
    omnetpp::simtime_t lastEventTime;

    // socket parameter
    int64_t baseTime; // in microseconds, as returned by opp_get_monotonic_clock_usecs()
    SOCKET listenerSocket;
    SOCKET connSocket;
    char recvBuffer[4000];
    int recvBufferSize;
    int *numBytesPtr;
    size_t numOfBytes;

    // counter for messages
    int maxAdvanceCounter = 0;
    int errorCounter = 0;
    int disconnectCounter = 0;
    int reconnectCounter = 0;

    // registered modules that represent agents in mosaik
    int numModules;
    omnetpp::cModule *modules[100];

    // event at socket
    omnetpp::cMessage *socketEvent;

    bool realtime;

    // mosaik parameter
    double stepSize;

    // message group to send messages to mosaik
    CosimaMsgGroup pmsg_group;

    /**
     * Initialize socket in order to listen to incoming connections from mosaik.
     */
    virtual void setupListener();
    /**
     * Receive data from socket.
     */
    virtual bool receiveWithTimeout(long usec);
    /**
     * Receive data from socket until a given time.
     */
    virtual int receiveUntil(int64_t targetTime);

public:
    /**
     * Constructor.
     */
    MosaikScheduler();

    /**
     * Destructor.
     */
    virtual ~MosaikScheduler();

    /**
     * Return a description for the GUI.
     */
    virtual std::string info() const override;

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() override;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() override;

    /**
     * Recalculates "base time" from current wall clock time.
     */
    virtual void executionResumed() override;

    void printCurrentTime();

    void log(std::string info);

    /**
     * To be called from the module which wishes to receive data from the
     * socket. The method must be called from the module's initialize()
     * function.
     */
    virtual void setInterfaceModule(omnetpp::cModule *module, bool isMosaikSchedulerModule);

    /**
     * Register scenario manager at scheduler module.
     */
    virtual void setScenarioManager(omnetpp::cModule *manager);

    /**
     * To be called within the scheduler to add module to list of modules.
     */
    virtual void addModule(omnetpp::cModule *module);

    /**
     * Searches in registered modules for module with given port and returns name of module.
     * Otherwise returns nullptr.
     *
     */
    virtual std::string getModuleNameFromPort(int port);

    /**
     * To be called to get receiver module from message.
     */
    virtual omnetpp::cModule *getReceiverModule(std::string module_name);


    /**
     * Returns the first event in the Future Event Set.
     */
    virtual omnetpp::cEvent *guessNextEvent() override;

    /**
     * Scheduler function -- it comes from the cScheduler interface.
     */
    virtual omnetpp::cEvent *takeNextEvent() override;

    /**
     * Undo takeNextEvent() -- it comes from the cScheduler interface.
     */
    virtual void putBackEvent(omnetpp::cEvent *event) override;
    /**
     * Is being called at the end of the simulation in mosaik in order to finish simulation in OMNeT++.
     */
    virtual void endSimulation();
    /**
     * Add message to message group in order to send message back to mosaik.
     */
    virtual void sendToMosaik(omnetpp::cMessage *reply);

    /**
     * Send message group back to mosaik.
     */
    virtual void sendMsgGroupToMosaik();

    /**
     * Handle message from mosaik
     */
    bool handleMsgFromMosaik();

};

#endif
