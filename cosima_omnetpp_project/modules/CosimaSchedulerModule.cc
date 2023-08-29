/*
 * CosimaSchedulerModule.cc
 *
 */
#include "CosimaSchedulerModule.h"

#include <string.h>
#include <omnetpp.h>

#include "../modules/CosimaScheduler.h"
#include "../messages/CosimaSchedulerMessage_m.h"

Define_Module(CosimaSchedulerModule);

CosimaSchedulerModule::CosimaSchedulerModule() {
    scheduler = nullptr;
    maxAdvEvent = new CosimaCtrlEvent("hello max advanced");
    maxAdvEvent->setCtrlType(ControlType::MaxAdvance);
    untilEvent = new CosimaCtrlEvent("simulation end in coupled simulation");
    untilEvent->setCtrlType(ControlType::Until);
}

CosimaSchedulerModule::~CosimaSchedulerModule() {
    cancelMaxAdvanceEvent();
    delete(maxAdvEvent);
    cancelUntilEvent();
    delete(untilEvent);
}


void CosimaSchedulerModule::initialize(int stage){
    scheduler = check_and_cast<CosimaScheduler *>(getSimulation()->getScheduler());
    // register module at scheduler
    scheduler->setInterfaceModule(this, true);
}

void CosimaSchedulerModule::handleMessage(cMessage *msg){
    if (typeid(*msg) == typeid(CosimaCtrlEvent)) {
        CosimaCtrlEvent *event = dynamic_cast<CosimaCtrlEvent *>(msg);
        if (event->getCtrlType() == 0) {
            // is max advance event
            scheduler->log("CosimaSchedulerModule: received max advance event at time " + simTime().str());
            scheduler->sendToCoupledSimulation(msg);
        } else if (event->getCtrlType() == 2) {
            // is until event
            scheduler->log("CosimaSchedulerModule: received until event.");
            scheduler->endRun();
            scheduler->setUntilReached(true);
        } else {
            // is message group event
            scheduler->log("CosimaSchedulerModule: received event in order to send info back to coupled simulation at time " + simTime().str());
            auto currentStep = 0U;
            currentStep = ceil(simTime().dbl()*1000);
            scheduler->sendMsgGroupToCoupledSimulation(false);
            delete msg;
        }
    } else {
        scheduler->log("CosimaSchedulerModule: received unknown message.", "warning");
        delete msg;
    }


}

void CosimaSchedulerModule::cancelMaxAdvanceEvent() {
    cancelEvent(maxAdvEvent);
}

void CosimaSchedulerModule::cancelUntilEvent() {
    cancelEvent(untilEvent);
}

