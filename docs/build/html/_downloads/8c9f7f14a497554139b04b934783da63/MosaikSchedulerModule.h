/*
 * MosaikSchedulerModule.h
 *
 *  The MosaikSchedulerModule represents the MosaikScheduler as a cModule.
 *  This is needed in order to schedule the max advance event.
 *
 */

#ifndef MOSAIKSCHEDULERMODULE_H_
#define MOSAIKSCHEDULERMODULE_H_

#include <string.h>
#include <omnetpp.h>

#include "../modules/MosaikScheduler.h"
#include "../messages/MosaikCtrlEvent_m.h"

using namespace omnetpp;

class MosaikSchedulerModule: public cSimpleModule {

public:
    MosaikSchedulerModule();
    virtual ~MosaikSchedulerModule();
    MosaikCtrlEvent* maxAdvEvent;
    MosaikCtrlEvent* untilEvent;
    void cancelMaxAdvanceEvent();
    void cancelUntilEvent();

protected:
    MosaikScheduler *scheduler;

    // The following redefined virtual function holds the algorithm.
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;

};

#endif /* MOSAIKSCHEDULERMODULE_H_ */
