/*
 * MosaikScenarioManager.h
 *
 *  Created on: Nov 23, 2021
 *      Author: malin
 */
#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include "MosaikScheduler.h"
#include "inet/common/lifecycle/LifecycleController.h"

using namespace omnetpp;

#ifndef MOSAIKSCENARIOMANAGER_H_
#define MOSAIKSCENARIOMANAGER_H_

class MosaikScenarioManager: public omnetpp::cSimpleModule {
private:
 MosaikScheduler *scheduler;

public:
    MosaikScenarioManager() {}
    ~MosaikScenarioManager();

protected:
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

    void disconnect(const char *moduleName);
    void connect(const char *moduleName);

};



#endif /* MOSAIKSCENARIOMANAGER_H_ */
