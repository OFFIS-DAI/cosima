/*
 * CosimaScheduler.h
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 *
 *  The CosimaScheduler replaces the OMNeT++ internal scheduler.
 *  In addition to the usual functions of a scheduler,
 *  a TCP connection to python can be established and incoming messages
 *  can be inserted into the simulation as events.
 *
 */
#pragma once

#include <vector>
#ifndef __COSIMASCHEDULER_H__
#define __COSIMASCHEDULER_H__

#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>

#include "../messages/CosimaSchedulerMessage_m.h"
#include "../messages/message.pb.h"

class CosimaSchedulerModule;
class CosimaScenarioManager;

class CosimaScheduler : public omnetpp::cScheduler
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
    CosimaSchedulerModule *getSchedulerModule() { return schedulerModule; }
    bool socketToCoupledSimulationInitialized() { return listenerSocket != -1; }

protected:
    CosimaSchedulerModule *schedulerModule;
    CosimaScenarioManager *scenarioManager;

    // save time of last event
    omnetpp::simtime_t lastEventTime;

    /**
     * Current max_advance; this should be the minimum of the max_advance from
     * mosaik and the time of our current reply.
     */
    omnetpp::simtime_t maxAdvance;

    /**
     * List of messages to be sent back to mosaik.  When adding a reply to this
     * list, `maxAdvance` should be updated to the reply's time if it is earlier
     * than the current `maxAdvance`.
     */
    std::list<void*> replies;

    // TCP port to coupled simulation
    auto static const PORT = 4242;

    SOCKET listenerSocket;

    // registered modules that represent agents in coupled simulation
    std::map<std::string, omnetpp::cSimpleModule*> modules = {};

    std::list<omnetpp::cModule*> attackModules = {};

    /**
     * Initialize socket in order to listen to incoming connections from coupled simulation.
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
    CosimaScheduler();

    /**
     * Destructor.
     */
    virtual ~CosimaScheduler();

    /**
     * Return a description for the GUI.
     */
    virtual std::string str() const override;

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
     * Use the given `CosimaSchedulerModule` as the scheduler module for this
     * simulation. This method should be called once from a
     * `CosimaSchedulerModule`'s `initialize` method.
     */
    virtual void setSchedulerModule(CosimaSchedulerModule* mod);

    /***
     * Register network layer for attack at CosimaScheduler.
     */
    void setAttackNetworkLayer(omnetpp::cModule *networkLayerModule);


    /**
     * Register scenario manager at scheduler module.
     */
    virtual void setScenarioManager(CosimaScenarioManager *manager);

    /**
     * Make a simple module available to mosaik using the entity ID `eid`.
     * Whenever data arrives for that entity, it will be scheduled as a self-
     * message of type `CosimaSchedulerMessage` at the given module.
     * This method should be called in the registered module's `initialize`
     * method.
     */
    virtual void registerModule(std::string eid, omnetpp::cSimpleModule *module);

    /**
     * Searches in registered modules for module with given port and returns name of module.
     * Otherwise returns nullptr.
     *
     */
    virtual std::string getModuleNameFromPort(int port);

    /**
     * Find the (registered) App module based on the name of the client
     * containing it.
     */
    virtual omnetpp::cSimpleModule *getModuleByEid(std::string module_name);
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
     * Is being called at the end of the simulation in coupled simulation in order to finish simulation in OMNeT++.
     */
    virtual void endSimulation();
    /**
     * Is called in order to inform coupled simulation about continued waiting from OMNeT++ side.
     */
    void informCoupledSimulationAboutWaiting();
    /**
     * Add message to message group in order to send message back to coupled simulation.
     */
    virtual void sendToCoupledSimulation(omnetpp::cMessage *reply);

    /**
     * Send message group back to coupled simulation.
     */
    virtual void sendMsgGroupToCoupledSimulation(bool isWaitingMsg);

    /**
     * Handle message from coupled simulation
     */
    int handleMsgFromCoupledSimulation(std::vector<char> data);
    /**
     * Setter methods.
     */
    void setUntilReached(bool untilReachedValue);
    void setInitialMessageReceived(bool value);
    /**
     * Getter method
     */
    bool getUntilReached();

private:
    void handleInitialMessage(InitialMessage);
    void handleInfoMessage(InfoMessage);
    void handleSynchronizationMessage(SynchronisationMessage);
    void handleInfrastructureMessage(InfrastructureMessage, std::vector<std::string>, std::vector<std::string>);
    void handleTrafficMessage(TrafficMessage);
    void handleAttackMessage(AttackMessage);

    void handleIcmpError(omnetpp::cEvent*);

};

#endif
