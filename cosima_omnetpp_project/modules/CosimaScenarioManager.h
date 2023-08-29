/*
 * CosimaScenarioManager.h
 *
 *  Created on: Nov 23, 2021
 *      Author: malin
 */
#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>

#include "CosimaScheduler.h"
#include "inet/common/lifecycle/LifecycleController.h"

using namespace omnetpp;

#ifndef COSIMASCENARIOMANAGER_H_
#define COSIMASCENARIOMANAGER_H_

class CosimaScenarioManager: public omnetpp::cSimpleModule {

public:
    CosimaScenarioManager() {}
    ~CosimaScenarioManager();

protected:
    CosimaScheduler *scheduler;

    struct networkModule {
      std::string moduleName;
      cModule *moduleObject;
      std::list<cGate*> in_gates;
      std::list<cGate*> connected_to_in_gates;
      std::list<cChannel*> connected_to_in_channels;
      std::list<cGate*> out_gates;
      std::list<cGate*> connected_to_out_gates;
      std::list<cChannel*> connected_to_out_channels;
    };
    std::list<networkModule> changedNetworkModules;

    virtual void initialize(int stage) override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;

    void handleTraffic(cModule *sourceModule, const char *source, const char *destination, int packet_length, int stop, int interval);

    CosimaSchedulerMessage *disconnect(const char *moduleName);
    CosimaSchedulerMessage *connect(const char *moduleName);

};



#endif /* COSIMASCENARIOMANAGER_H_ */
