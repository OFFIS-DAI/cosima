/*
 * MosaikSchedulerModule.cc
 *
 */
#include "../modules/MosaikSchedulerModule.h"

#include <string.h>
#include <omnetpp.h>

#include "../modules/MosaikScheduler.h"
#include "../messages/MosaikSchedulerMessage_m.h"

Define_Module(MosaikSchedulerModule);

MosaikSchedulerModule::MosaikSchedulerModule() {
    scheduler = nullptr;
    maxAdvEvent = new MosaikCtrlEvent("hello max advanced");
    maxAdvEvent->setCtrlType(ControlType::MaxAdvance);
    untilEvent = new MosaikCtrlEvent("simulation end in mosaik");
    untilEvent->setCtrlType(ControlType::Until);
}

MosaikSchedulerModule::~MosaikSchedulerModule() {
    cancelMaxAdvanceEvent();
    delete(maxAdvEvent);
    cancelUntilEvent();
    delete(untilEvent);
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
            scheduler->log("MosaikSchedulerModule: received max advance event at time " + simTime().str());
            scheduler->sendToMosaik(msg);
        } else if (event->getCtrlType() == 2) {
            // is until event
            scheduler->log("MosaikSchedulerModule: received until event.");
            scheduler->endRun();
            scheduler->setUntilReached(true);
        } else {
            // is message group event
            scheduler->log("MosaikSchedulerModule: received event in order to send info back to mosaik at time " + simTime().str());
            auto currentStep = 0U;
            currentStep = ceil(simTime().dbl()*1000);
            scheduler->sendMsgGroupToMosaik(false);
            delete msg;
        }
    } else {
        scheduler->log("MosaikSchedulerModule: received unknown message.", "warning");
        delete msg;
    }


}

void MosaikSchedulerModule::cancelMaxAdvanceEvent() {
    cancelEvent(maxAdvEvent);
}

void MosaikSchedulerModule::cancelUntilEvent() {
    cancelEvent(untilEvent);
}

