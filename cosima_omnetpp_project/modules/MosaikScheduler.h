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

#include "../messages/MosaikSchedulerMessage_m.h"

class MosaikScheduler : public omnetpp::cScheduler
{
public:
    /**
     * Method can be called from modules in order to get matching port for module name.
     */
    int getPortForModule(std::string module_name);

    /**
     * Helper functions for testing.
     */
    std::list<omnetpp::cModule*> getModuleList();
    omnetpp::cModule *getSchedulerModule() { return schedulerModule; }
    bool socketToMosaikInitialized() { return listenerSocket != -1; }

protected:
    omnetpp::cModule *schedulerModule;
    omnetpp::cModule *scenarioManager;

    // save time of last event
    omnetpp::simtime_t lastEventTime;

    // TCP port to mosaik
    auto static const PORT = 4242;

    SOCKET listenerSocket;

    // registered modules that represent agents in mosaik
    int numModules;
    std::list<omnetpp::cModule*> modules = {};

    std::list<omnetpp::cModule*> attackModules = {};

    // event at socket
    omnetpp::cMessage *socketEvent;


    /**
     * Initialize socket in order to listen to incoming connections from mosaik.
     */
    virtual void setupListener();
    /**
     * Receive data from socket until a given time.
     */
    virtual void receive();


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

    int getNumberOfSavedMessages();

    void printCurrentTime();

    std::string getCurrentTime();

    void log(std::string info);

    void log(std::string info, std::string logLevel);

    /**
     * To be called from the module which wishes to receive data from the
     * socket. The method must be called from the module's initialize()
     * function.
     */
    virtual void setInterfaceModule(omnetpp::cModule *module, bool isMosaikSchedulerModule);

    /***
     * Register network layer for attack at MosaikScheduler.
     */
    void setAttackNetworkLayer(omnetpp::cModule *networkLayerModule);


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
    omnetpp::cModule *getAttackNetworkLayerModule(std::string module_name);


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
     * Writes string with indicators for simulation to file.
     */
    void writeSimulationSnapshot();
    /**
     * Is being called at the end of the simulation in mosaik in order to finish simulation in OMNeT++.
     */
    virtual void endSimulation();
    /**
     * Is called in order to inform mosaik about continued waiting from OMNeT++ side.
     */
    void informMosaikAboutWaiting();
    /**
     * Add message to message group in order to send message back to mosaik.
     */
    virtual void sendToMosaik(omnetpp::cMessage *reply);

    /**
     * Send message group back to mosaik.
     */
    virtual void sendMsgGroupToMosaik(bool isWaitingMsg);

    /**
     * Handle message from mosaik
     */
    int handleMsgFromMosaik(std::vector<char> data);
    /**
     * Setter methods.
     */
    void setUntilReached(bool untilReachedValue);
    void setInitialMessageReceived(bool value);
    /**
     * Getter method
     */
    bool getUntilReached();

};

#endif
