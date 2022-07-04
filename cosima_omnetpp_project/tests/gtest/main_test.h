/*
 * main_test.h
 *
 *  Created on: Feb 6, 2022
 *      Author: malin
 *  Basic code concept from https://github.com/roVer-HM/crownet/tree/master/crownet/tests/gtest/src.
 */

#ifndef TESTS_GTEST_MAIN_TEST_H_
#define TESTS_GTEST_MAIN_TEST_H_

#pragma once

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <omnetpp.h>
#include "inet/common/packet/Packet.h"
#include "inet/common/Units.h"
#include "inet/applications/base/ApplicationPacket_m.h"
#include "../../modules/MosaikScheduler.h"


using namespace inet;
using namespace omnetpp;

class MosaikSchedulerMock : public MosaikScheduler {
public:
    MosaikSchedulerMock() {
        numModules = 0;
        sim = new cSimulation("simulation", new cNullEnvir(0, nullptr, nullptr));
    }

    MOCK_METHOD(cModule *, getReceiverModule, (std::string module_name));
    MOCK_METHOD(void, sendToMosaik, (cMessage*));
    MOCK_METHOD(void, sendMsgGroupToMosaik, ());
    MOCK_METHOD(int, receiveUntil, (int64_t targetTime));
    MOCK_METHOD(void, endSimulation, ());

    int getPortForModule (std::string module_name) {
        return MosaikScheduler::getPortForModule(module_name);
    }

    void deleteRegisteredModules() {
        modules.clear();
        numModules = 0;
    }

    cEvent *takeNextEvent() {
        return MosaikScheduler::takeNextEvent();
    }

    auto getUntilReached() {
        auto untilReached = false;
        untilReached = MosaikScheduler::getUntilReached();
        return untilReached;
    }

    void setInitialMessageReceived() {
        MosaikScheduler::setInitialMessageReceived(true);
    }
};

class EmptyConfig : public cConfiguration {
protected:
    class NullKeyValue : public KeyValue {
    public:
        virtual const char *getKey() const { return nullptr; }
        virtual const char *getValue() const { return nullptr; }
        virtual const char *getBaseDirectory() const { return nullptr; }
    };
    NullKeyValue nullKeyValue;

    public:
    virtual const char *substituteVariables(const char *value) const {
        return value;
    }

    virtual const char *getConfigValue(const char *key) const { return nullptr; }
    virtual const KeyValue &getConfigEntry(const char *key) const {
        return nullKeyValue;
    }
    virtual const char *getPerObjectConfigValue(const char *objectFullPath,
            const char *keySuffix) const {
        return nullptr;
    }
    virtual const KeyValue &getPerObjectConfigEntry(const char *objectFullPath,
            const char *keySuffix) const {
        return nullKeyValue;
    }
};

class GtestEnv : public omnetpp::cNullEnvir {
public:
    // constructor
    GtestEnv(int ac, char **av, cConfiguration *c) : cNullEnvir(ac, av, c) {}

    // model parameters: accept defaults
    virtual void readParameter(cPar *par) {
        if (par->containsValue())
            par->acceptDefault();
        else
            throw cRuntimeError("no value for %s", par->getFullPath().c_str());
    }
    // send module log messages to stdout
    virtual void sputn(const char *s, int n) { (void)::fwrite(s, 1, n, stdout); }
};

/**
 * Base test fixture to access currently active simulation.
 */
class BaseOppTest : public ::testing::Test {
public:
    void setSimTime(simtime_t t) {
        auto sim = cSimulation::getActiveSimulation();
        sim->setSimTime(t);
    }

    Packet* build(Ptr<Chunk> content){
        auto pkt = new Packet();
        pkt->insertAtFront(content);
        return pkt;
    }

    Packet* build(b dataLength ){
        auto pkt = new Packet();
        auto content = makeShared<ApplicationPacket>();
        content->setChunkLength(dataLength);
        pkt->insertAtFront(content);
        return pkt;
    }

};




#endif /* TESTS_GTEST_MAIN_TEST_H_ */
