/*
 * MosaikObserver.h
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */

#ifndef __MOSAIKOBSERVER_H__
#define __MOSAIKOBSERVER_H__

#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include "message.pb.h"
#include "TikTokPacket_m.h"

class MosaikObserver : public omnetpp::cScheduler
{
public:
    int port;
    int getPortForModule(std::string text);
protected:

    omnetpp::cModule *module;
    omnetpp::cModule *observerModule;

    omnetpp::cMessage *notificationMsg;
    char *recvBuffer;
    int recvBufferSize;
    int *numBytesPtr;
    int numModules;
    omnetpp::cModule *modules[50];
    omnetpp::cMessage *socketEvent;
    bool realtime;
    char outputMsg[500];
    size_t numOfBytes;
    double simulationTimeOffset;
    double stepSize;


    // state
    int64_t baseTime; // in microseconds, as returned by opp_get_monotonic_clock_usecs()
    SOCKET listenerSocket;
    SOCKET connSocket;

    virtual void setupListener();
    virtual bool receiveWithTimeout(long usec);
    virtual int receiveUntil(int64_t targetTime);

public:
    /**
     * Constructor.
     */
    MosaikObserver();

    /**
     * Destructor.
     */
    virtual ~MosaikObserver();

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

    /**
     * To be called from the module which wishes to receive data from the
     * socket. The method must be called from the module's initialize()
     * function.
     */
    virtual void setInterfaceModule(omnetpp::cModule *module, char *recvBuffer, int recvBufferSize, int *numBytesPtr);

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
    virtual omnetpp::cModule *getReceiverModule(std::string text);

    /**
     * Get incoming messages from socket. Can be called from modules.
     */
    virtual char *getRecvBuffer();


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
     * Send message back to mosaik
     */
    virtual void sendBytes(omnetpp::cMessage *reply);

    /**
     * Handle message from mosaik
     */
    bool handleCmsg();

};

#endif
