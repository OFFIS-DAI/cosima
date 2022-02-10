/*
 * MosaikScenarioManager.cc
 *
 *  Created on: Nov 23, 2021
 *      Author: malin
 */
#include <string.h>
#include <omnetpp.h>
#include <algorithm>
#include "MosaikScenarioManager.h"
#include "messages/MosaikCtrlEvent_m.h"

Define_Module(MosaikScenarioManager);

MosaikScenarioManager::~MosaikScenarioManager() {
    changedNetworkModules.clear();
}


void MosaikScenarioManager::initialize(int stage) {
    scheduler->log("MosaikScenarioManager: initialize.");
    // get intern socket scheduler from simulation and cast to MosaikScheduler
    scheduler =
            check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());

    // register module at scheduler
    scheduler->setScenarioManager(this);

}

void MosaikScenarioManager::handleMessage(cMessage *msg) {
    std::string msgStr = msg->getDisplayString();

    scheduler->log("MosaikScenarioManager: received message " + msgStr);
    if (typeid(*msg) == typeid(MosaikCtrlEvent)) {
        MosaikCtrlEvent *event = dynamic_cast<MosaikCtrlEvent *>(msg);
        if (event->getCtrlType() == 3) {
            // is disconnect event
            disconnect(event->getModuleName());
        } else if (event->getCtrlType() ==4) {
            // is connect event
            connect(event->getModuleName());
        }
    }
    delete msg;
}

void MosaikScenarioManager::disconnect(const char *moduleName) {
    std::string nameStr = moduleName;

    scheduler->log("MosaikScenarioManager: disconnect " + nameStr);

    MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
    notification_message->setDisconnected_event(true);
    notification_message->setSender(moduleName);
    notification_message->setConnection_change_successful(true);

    std::string connectedModule = moduleName;
    std::string thisName = getFullPath();
    std::size_t pos = thisName.find(".");      // position of "." in string
    std::string moduleNameStr = thisName.substr(0, pos+1); // get name of network
    moduleNameStr += moduleName; // concatenate network name with module name
    cModule *module = getModuleByPath(moduleNameStr.c_str()); // get module object

    try {
        // save information about module for later
        networkModule disconnectedModule;
        disconnectedModule.moduleName = moduleName;
        disconnectedModule.moduleObject = module;
        for (int i = 0; i < module->gateCount() / 2; i++) {
            // get gates of module
            cGate *in_gate = module->gate("ethg$i", i)->getIncomingTransmissionChannel()->getSourceGate();
            cGate *out_gate = module->gate("ethg$o", i);
            disconnectedModule.in_gates.push_back(in_gate);
            disconnectedModule.connected_to_in_gates.push_back(in_gate->getNextGate());

            //set properties of current channel on new channel
            cDatarateChannel *channelIn = cDatarateChannel::create("channel");
            cDatarateChannel *currentChannelIn = dynamic_cast<cDatarateChannel *> (in_gate->getChannel());
            if (currentChannelIn->getTransmissionFinishTime() != -1) {
                MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
                notification_message->setTransmission_error(true);
                scheduler->sendToMosaik(notification_message);
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
            cDatarateChannel *currentChannelOut = dynamic_cast<cDatarateChannel *> (out_gate->getChannel());
            if (currentChannelOut->getTransmissionFinishTime() != -1) {
                MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
                notification_message->setTransmission_error(true);
                scheduler->sendToMosaik(notification_message);
            }
            channelOut->setDatarate(currentChannelOut->getDatarate());
            channelOut->setComponentType(currentChannelOut->getComponentType());
            channelOut->setDelay(currentChannelOut->getDelay().dbl());
            channelIn->setBitErrorRate(currentChannelIn->getBitErrorRate());
            disconnectedModule.connected_to_out_channels.push_back(channelOut);

            // disconnect gates and delete channels
            in_gate->disconnect();
            out_gate->disconnect();
        }
        changedNetworkModules.push_back(disconnectedModule);

    } catch (...) {
        notification_message->setConnection_change_successful(false);
        scheduler->log("MosaikScenarioManager: ERROR when trying to disconnect " + nameStr + ".");
    }
    scheduler->sendToMosaik(notification_message);
}

void MosaikScenarioManager::connect(const char *moduleName) {
    std::string nameStr = moduleName;

    scheduler->log("MosaikScenarioManager: connect " + nameStr);

    MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
    notification_message->setReconnected_event(true);
    notification_message->setSender(moduleName);
    notification_message->setConnection_change_successful(false);

    std::string moduleNameStr = moduleName;

    std::list<networkModule>::iterator it;
    for (it = changedNetworkModules.begin(); it != changedNetworkModules.end(); ++it){
        if (moduleNameStr.compare(it->moduleName) == 0) {
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

                notification_message->setConnection_change_successful(true);
            } catch(...) {
                scheduler->log("MosaikScenarioManager: ERROR when trying to reconnect " + nameStr + ".");
            }

        }
    }
    scheduler->sendToMosaik(notification_message);
}
