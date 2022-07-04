/*
 * MosaikScenarioManager.cc
 *
 *  Created on: Nov 23, 2021
 *      Author: malin
 */
#include "../modules/MosaikScenarioManager.h"

#include <string.h>
#include <omnetpp.h>
#include <algorithm>
#include "../messages/MosaikCtrlEvent_m.h"

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
    if (msg == nullptr or msg->getDisplayString() == nullptr) {
        throw cRuntimeError("MosaikScenarioManager: Received nullptr message");
    }
    std::string msgStr = msg->getDisplayString();

    scheduler->log("MosaikScenarioManager: received message " + msgStr);
    std::list<MosaikSchedulerMessage *> notificationMessages;
    if (typeid(*msg) == typeid(MosaikCtrlEvent)) {
        MosaikCtrlEvent *event = dynamic_cast<MosaikCtrlEvent *>(msg);
        for (int i = 0; i < event->getModuleNamesArraySize(); i++) {
            if (strcmp(event->getModuleNames(i), "") == 0) {
                throw cRuntimeError("MosaikScenarioManager: Received message without module name.");
            }
            if (event->getCtrlType() == 3) {
                // is disconnect event
                notificationMessages.push_back(disconnect(event->getModuleNames(i)));
            } else if (event->getCtrlType() ==4) {
                // is connect event
                notificationMessages.push_back(connect(event->getModuleNames(i)));
            }
        }
    }

    for (auto const& msg : notificationMessages) {
        if (msg != nullptr) {
            scheduler->sendToMosaik(msg);
        }
    }

    delete msg;
}

MosaikSchedulerMessage *MosaikScenarioManager::disconnect(const char *moduleName) {
    std::string nameStr = moduleName;

    scheduler->log("MosaikScenarioManager: disconnect " + nameStr);

    auto connectedModule = moduleName;
    auto thisName = getFullPath();
    std::size_t pos = thisName.find(".");      // position of "." in string
    auto moduleNameStr = thisName.substr(0, pos+1); // get name of network
    moduleNameStr += moduleName; // concatenate network name with module name
    auto module = getModuleByPath(moduleNameStr.c_str()); // get module object

    if (module == nullptr) {
        return nullptr;
    }

    auto notificationMessage = new MosaikSchedulerMessage();
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
                auto notification_message = new MosaikSchedulerMessage();
                notification_message->setTransmission_error(true);
                notification_message->setSender(moduleName);
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
        scheduler->log("MosaikScenarioManager: ERROR when trying to disconnect " + nameStr + ".");
    }
    notificationMessage->setConnection_change_successful(disconnectedGate);

    return notificationMessage;
}

MosaikSchedulerMessage *MosaikScenarioManager::connect(const char *moduleName) {
    std::string nameStr = moduleName;

    scheduler->log("MosaikScenarioManager: connect " + nameStr);

    auto notificationMessage = new MosaikSchedulerMessage();
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
                scheduler->log("MosaikScenarioManager: ERROR when trying to reconnect " + nameStr + ".");
            }

        }
    }
    return notificationMessage;
}
