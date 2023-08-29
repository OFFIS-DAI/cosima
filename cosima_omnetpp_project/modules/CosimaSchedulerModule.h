/*
 * CosimaSchedulerModule.h
 *
 *  The CosimaSchedulerModule represents the CosimaScheduler as a cModule.
 *  This is needed in order to schedule the max advance event.
 *
 */

#ifndef COSIMASCHEDULERMODULE_H_
#define COSIMASCHEDULERMODULE_H_

#include <string.h>
#include <omnetpp.h>

#include "../messages/CosimaCtrlEvent_m.h"
#include "CosimaScheduler.h"

using namespace omnetpp;

class CosimaSchedulerModule: public cSimpleModule {

public:
    CosimaSchedulerModule();
    virtual ~CosimaSchedulerModule();
    CosimaCtrlEvent* maxAdvEvent;
    CosimaCtrlEvent* untilEvent;
    void cancelMaxAdvanceEvent();
    void cancelUntilEvent();

protected:
    CosimaScheduler *scheduler;

    // The following redefined virtual function holds the algorithm.
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;

};

#endif /* COSIMASCHEDULERMODULE_H_ */
