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
}

MosaikSchedulerModule::~MosaikSchedulerModule() {
    cancelAndDelete(max_adv_event);
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
            std::cout << "MosaikSchedulerModule: received max advance event." << endl;
            scheduler->sendToMosaik(msg);
        } else {
            // is message group event
            std::cout << "MosaikSchedulerModule: received event in order to send info back to mosaik." << endl;
            scheduler->sendMsgGroupToMosaik();
            delete msg;
        }
    } else {
        std::cout << "MosaikSchedulerModule: received unknown message." << endl;
        delete msg;
    }


}

void MosaikSchedulerModule::cancelMaxAdvanceEvent() {
    cancelEvent(max_adv_event);
}

