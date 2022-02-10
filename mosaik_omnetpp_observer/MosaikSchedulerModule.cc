/*
 * MosaikSchedulerModule.cc
 *
 */
#include <string.h>
#include <omnetpp.h>

#include "MosaikSchedulerModule.h"
#include "MosaikScheduler.h"
#include "messages/MosaikSchedulerMessage_m.h"

Define_Module(MosaikSchedulerModule);

MosaikSchedulerModule::MosaikSchedulerModule() {
    scheduler = nullptr;
    max_adv_event = new MosaikCtrlEvent("hello max advanced");
    until_event = new MosaikCtrlEvent("simulation end in mosaik");
    until_event->setCtrlType(2);
}

MosaikSchedulerModule::~MosaikSchedulerModule() {
    cancelMaxAdvanceEvent();
    delete(max_adv_event);
    cancelUntilEvent();
    delete(until_event);
}


void MosaikSchedulerModule::initialize(int stage){
      scheduler = check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());
      // register module at scheduler
      scheduler->setInterfaceModule(this, true);
}

void MosaikSchedulerModule::handleMessage(cMessage *msg){
    if (typeid(*msg) == typeid(MosaikCtrlEvent)) {
        MosaikCtrlEvent *event = dynamic_cast<MosaikCtrlEvent *>(msg);
        if (event->getCtrlType() == 0) {
            // is max advance event
            scheduler->log("MosaikSchedulerModule: received max advance event.");
            scheduler->sendToMosaik(msg);
        } else if (event->getCtrlType() == 2) {
            // is until event
            scheduler->log("MosaikSchedulerModule: received until event.");
            scheduler->until_reached = true;
        } else {
            // is message group event
            scheduler->log("MosaikSchedulerModule: received event in order to send info back to mosaik.");
            scheduler->sendMsgGroupToMosaik();
            delete msg;
        }
    } else {
        scheduler->log("MosaikSchedulerModule: received unknown message.");
        delete msg;
    }


}

void MosaikSchedulerModule::cancelMaxAdvanceEvent() {
    cancelEvent(max_adv_event);
}

void MosaikSchedulerModule::cancelUntilEvent() {
    cancelEvent(until_event);
}

