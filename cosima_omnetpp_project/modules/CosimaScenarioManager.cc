/*
 * CosimaScenarioManager.cc
 *
 *  Created on: Nov 23, 2021
 *      Author: malin
 */
#include <string.h>
#include <omnetpp.h>
#include <algorithm>
#include "../messages/AttackEvent_m.h"
#include "CosimaScenarioManager.h"
#include "../messages/CosimaCtrlEvent_m.h"

Define_Module(CosimaScenarioManager);

CosimaScenarioManager::~CosimaScenarioManager() {
    changedNetworkModules.clear();
}


void CosimaScenarioManager::initialize(int stage) {
    scheduler->log("CosimaScenarioManager: initialize.");
    // get intern socket scheduler from simulation and cast to CosimaScheduler
    scheduler =
            check_and_cast<CosimaScheduler *>(getSimulation()->getScheduler());

    // register module at scheduler
    scheduler->setScenarioManager(this);

}

void CosimaScenarioManager::handleMessage(cMessage *msg) {
    if (msg == nullptr or msg->getDisplayString() == nullptr) {
        throw cRuntimeError("CosimaScenarioManager: Received nullptr message");
    }

    std::list<CosimaSchedulerMessage *> notificationMessages;
    if (typeid(*msg) == typeid(CosimaCtrlEvent)) {
        CosimaCtrlEvent *event = dynamic_cast<CosimaCtrlEvent *>(msg);
        if (event->getCtrlType() == 6) {
            cModule *receiverModule = scheduler->getReceiverModule(event->getSource());
            if (receiverModule == nullptr) {
                throw cRuntimeError("CosimaScenarioManager: Can't resolve traffic sender module.");
            }
            handleTraffic(receiverModule, event->getSource(), event->getDestination(), event->getPacketLength(), event->getStop(), event->getInterval());
        }
        for (int i = 0; i < event->getModuleNamesArraySize(); i++) {
            if (strcmp(event->getModuleNames(i), "") == 0) {
                throw cRuntimeError("CosimaScenarioManager: Received message without module name.");
            }
            if (event->getCtrlType() == 3) {
                // is disconnect event
                notificationMessages.push_back(disconnect(event->getModuleNames(i)));
            } else if (event->getCtrlType() ==4) {
                // is connect event
                notificationMessages.push_back(connect(event->getModuleNames(i)));
            }
        }
    } else if (typeid(*msg) == typeid(AttackEvent)) {
        AttackEvent *event = dynamic_cast<AttackEvent *>(msg);
        auto moduleName = event->getAttacked_module();
        auto moduleObj = scheduler->getAttackNetworkLayerModule(moduleName);
        auto newEvent = event->dup();
        newEvent->setArrival(moduleObj->getId(), -1, simTime());
        getSimulation()->getFES()->insert(newEvent);
    }

    for (auto const& msg : notificationMessages) {
        if (msg != nullptr) {
            scheduler->sendToCoupledSimulation(msg);
        }
    }

    delete msg;
}

void CosimaScenarioManager::handleTraffic(cModule *sourceModule, const char *source, const char *destination, int packet_length, int stop, int interval) {
    CosimaCtrlEvent *trafficEvent = new CosimaCtrlEvent();
    trafficEvent->setCtrlType(Traffic);
    trafficEvent->setDestination(destination);

    // is time < stop: schedule new event
    trafficEvent->setPacketLength(packet_length);
    trafficEvent->setArrival(sourceModule->getId(), -1, simTime());
    getSimulation()->getFES()->insert(trafficEvent);

    if (simTime().inUnit(SimTimeUnit::SIMTIME_MS) + interval <= stop) {
        double intervalDbl = interval * 1.0;
        intervalDbl = intervalDbl/1000;
        omnetpp::simtime_t newTrafficTime = simTime() + intervalDbl;

        CosimaCtrlEvent *newTrafficEvent = new CosimaCtrlEvent();
        newTrafficEvent->setCtrlType(Traffic);
        newTrafficEvent->setSource(source);
        newTrafficEvent->setDestination(destination);
        newTrafficEvent->setStop(stop);
        newTrafficEvent->setInterval(interval);
        newTrafficEvent->setPacketLength(packet_length);
        scheduleAt(newTrafficTime, newTrafficEvent);
    }
}

CosimaSchedulerMessage *CosimaScenarioManager::disconnect(const char *moduleName) {
    std::string nameStr = moduleName;

    scheduler->log("CosimaScenarioManager: disconnect " + nameStr);

    auto thisName = getFullPath();
    std::size_t pos = thisName.find(".");      // position of "." in string
    auto moduleNameStr = thisName.substr(0, pos+1); // get name of network
    moduleNameStr += moduleName; // concatenate network name with module name
    auto module = getModuleByPath(moduleNameStr.c_str()); // get module object

    if (module == nullptr) {
        return nullptr;
    }

    auto notificationMessage = new CosimaSchedulerMessage();
    notificationMessage->setDisconnected_event(true);
    notificationMessage->setSender(moduleName);
    bool disconnectedGate = false;

    try {
        // save information about module for later
        networkModule disconnectedModule;
        disconnectedModule.moduleName = moduleName;
        disconnectedModule.moduleObject = module;
        for (auto i = 0; i < module->gateCount() / 2; i++) {
            // get gates of module
            if (not module->hasGate("ethg$i", i) or not module->hasGate("ethg$o", i)) {
                continue;
            }
            if(not module->gate("ethg$i", i)->isConnectedOutside() or not module->gate("ethg$o", i)->isConnectedOutside()) {
                continue;
            }
            cGate *in_gate = module->gate("ethg$i", i)->getIncomingTransmissionChannel()->getSourceGate();
            cGate *out_gate = module->gate("ethg$o", i);
            disconnectedModule.in_gates.push_back(in_gate);
            disconnectedModule.connected_to_in_gates.push_back(in_gate->getNextGate());

            //set properties of current channel on new channel
            cDatarateChannel *channelIn = cDatarateChannel::create("channel");
            cDatarateChannel *currentChannelIn = dynamic_cast<cDatarateChannel *> (in_gate->getChannel());
            cDatarateChannel *currentChannelOut = dynamic_cast<cDatarateChannel *> (out_gate->getChannel());

            if (currentChannelIn->getTransmissionFinishTime() != -1 or currentChannelOut->getTransmissionFinishTime() != -1) {
                auto notification_message = new CosimaSchedulerMessage();
                notification_message->setTransmission_error(true);
                notification_message->setSender(moduleName);
                scheduler->sendToCoupledSimulation(notification_message);
            }
            channelIn->setDatarate(currentChannelIn->getDatarate());
            channelIn->setComponentType(currentChannelIn->getComponentType());
            channelIn->setDelay(currentChannelIn->getDelay().dbl());
            channelIn->setBitErrorRate(currentChannelIn->getBitErrorRate());
            disconnectedModule.connected_to_in_channels.push_back(channelIn);

            //set properties of current channel on new channel
            disconnectedModule.out_gates.push_back(out_gate);
            disconnectedModule.connected_to_out_gates.push_back(out_gate->getNextGate());
            cDatarateChannel *channelOut = cDatarateChannel::create("channel");
            channelOut->setDatarate(currentChannelOut->getDatarate());
            channelOut->setComponentType(currentChannelOut->getComponentType());
            channelOut->setDelay(currentChannelOut->getDelay().dbl());
            channelIn->setBitErrorRate(currentChannelIn->getBitErrorRate());
            disconnectedModule.connected_to_out_channels.push_back(channelOut);

            // disconnect gates and delete channels
            if(in_gate->isConnected()) {
                in_gate->disconnect();
                disconnectedGate = true;
            }
            if(out_gate->isConnected()) {
                out_gate->disconnect();
                disconnectedGate = true;
            }
        }
        changedNetworkModules.push_back(disconnectedModule);

    } catch (...) {
        scheduler->log("CosimaScenarioManager: ERROR when trying to disconnect " + nameStr + ".", "warning");
    }
    notificationMessage->setConnection_change_successful(disconnectedGate);

    return notificationMessage;
}

CosimaSchedulerMessage *CosimaScenarioManager::connect(const char *moduleName) {
    std::string nameStr = moduleName;

    scheduler->log("CosimaScenarioManager: connect " + nameStr);

    auto notificationMessage = new CosimaSchedulerMessage();
    notificationMessage->setReconnected_event(true);
    notificationMessage->setSender(moduleName);
    notificationMessage->setConnection_change_successful(false);

    std::list<networkModule>::iterator it;
    for (it = changedNetworkModules.begin(); it != changedNetworkModules.end(); ++it){
        if (nameStr.compare(it->moduleName) == 0) {
            try {
                while (it->out_gates.size()>0) {
                    it->out_gates.front()->connectTo(it->connected_to_out_gates.front(), it->connected_to_out_channels.front());
                    it->out_gates.pop_front();
                    it->connected_to_out_gates.pop_front();
                    it->connected_to_out_channels.pop_front();
                }
                while (it->in_gates.size()>0) {
                    it->in_gates.front()->connectTo(it->connected_to_in_gates.front(), it->connected_to_in_channels.front());
                    it->in_gates.pop_front();
                    it->connected_to_in_gates.pop_front();
                    it->connected_to_in_channels.pop_front();
                }

                notificationMessage->setConnection_change_successful(true);
            } catch(...) {
                scheduler->log("CosimaScenarioManager: ERROR when trying to reconnect " + nameStr + ".", "warning");
            }

        }
    }
    return notificationMessage;
}
