/*
 * CosimaSchedulerTest.cc
 *
 *  Created on: Feb 6, 2022
 *      Author: malin
 */

#include "main_test.h"
#include "../../modules/AgentAppTcp.h"
#include "../../modules/CosimaSchedulerModule.h"
#include "../../modules/CosimaScenarioManager.h"

class CosimaSchedulerTest : public BaseOppTest {
public:
    CosimaScheduler *scheduler;

    CosimaSchedulerTest() {
        scheduler = check_and_cast<CosimaScheduler *>(getSimulation()->getScheduler());
    }

    /**
     * Creates a new AgentAppTcp object and sets name to "client".
     */
    AgentAppTcp *createAgentApp() {
        AgentAppTcp *app = new AgentAppTcp();
        app->setName("client");
        return app;
    }

    /**
     * Creates CosimaSchedulerModule object.
     */
    CosimaSchedulerModule *createCosimaSchedulerModule() {
        CosimaSchedulerModule *schedulerModule = new CosimaSchedulerModule();
        return schedulerModule;
    }

    /**
     * Creates CosimaSchedulerMessage with delay info.
     */
    CosimaSchedulerMessage *createValidCosimaSchedulerMessageWithDelayInfo() {
        CosimaSchedulerMessage *message = new CosimaSchedulerMessage();
        message->setSender("sender");
        message->setReceiver("receiver");
        message->setMsgId("id");
        message->setContent("content");
        message->setDelay(10);
        message->setCreationTime(1);
        return message;
    }

    /**
     * Creates CosimaCtrlEvent.
     */
    CosimaCtrlEvent *createValidCosimaCtrlEvent() {
        return new CosimaCtrlEvent();
    }

};

/**
 * Test method setInterfaceModule().
 * AgentAppTcp registers at scheduler.
 * asserted result: scheduler adds application to internal list.
 */
TEST_F(CosimaSchedulerTest, AddModuleToList) {
    AgentAppTcp *app = createAgentApp();
    scheduler->setInterfaceModule(app, false);
    cModule *module = scheduler->getModuleList().front();

    // module should not be a nullptr
    ASSERT_NE(nullptr, module);
    // we ASSERT one element in the list of registered modules
    ASSERT_EQ(scheduler->getModuleList().size(), 1);
    // currently created AgentAppTcp should be that element
    ASSERT_EQ(module->getId(),app->getId());
    ASSERT_EQ(module->getClassName(), app->getClassName());

    // clean up
    delete app;
}

/**
 * Test method setInterfaceModule().
 * SchedulerModule registers at scheduler.
 * asserted result: scheduler saves schedulerModule.
 */
TEST_F(CosimaSchedulerTest, AddCosimaSchedulerModuleToList) {
    CosimaSchedulerModule *schedulerModule = createCosimaSchedulerModule();
    scheduler->setInterfaceModule(schedulerModule, true);
    cModule *module = scheduler->getModuleList().front();

    // we still assert one element in the list, scheduler Module should not be added
    ASSERT_EQ(scheduler->getModuleList().size(), 1);
    // schedulerModule should not be a nullptr
    ASSERT_NE(nullptr, schedulerModule);
    // module should still be registered as interface module
    ASSERT_NE(nullptr, module);
    // schedulerModule should be registered at scheduler
    ASSERT_EQ(scheduler->getSchedulerModule(), schedulerModule);

    // clean up
    delete schedulerModule;
}

/**
 * Test method setInterfaceModule().
 * Method is called with null-pointer.
 * asserted result: error is thrown.
 */
TEST_F(CosimaSchedulerTest, TryCallingSetInterfaceModuleWithNullptr) {
    // we assert a cRuntimeError
    try {
        scheduler->setInterfaceModule(nullptr, false);
        // test fails, if we reach this point
        FAIL();
    }
    catch(const cRuntimeError& expected) {
    }

    // we assert a cRuntimeError
    try {
        scheduler->setInterfaceModule(nullptr, true);
        // test fails, if we reach this point
        FAIL();
    }
    catch(const cRuntimeError& expected) {
    }

}

/**
 * Test method sendToCoupledSimulation().
 * Method is called with null-pointer.
 * asserted result: error is thrown.
 */
TEST_F(CosimaSchedulerTest, TestSendToCoupledSimulationWithNullptr) {
    try {
        scheduler->sendToCoupledSimulation(nullptr);
        // test fails, if we reach this point
        FAIL();
    }
    catch(const cRuntimeError& expected) {
    }
    ASSERT_EQ(scheduler->getNumberOfSavedMessages(), 0);
}

/**
 * Test method sendToCoupledSimulation().
 * Method is called with delay reply from AgentAppUdp.
 * asserted result: current pmsg group contains one element.
 */
TEST_F(CosimaSchedulerTest, TestSendToCoupledSimulationWithDelayMsg) {
    CosimaSchedulerMessage *delayMsg = createValidCosimaSchedulerMessageWithDelayInfo();
    scheduler->sendToCoupledSimulation(delayMsg);
    ASSERT_EQ(scheduler->getNumberOfSavedMessages(), 1);
    delete delayMsg;
}


/**
 * Test method getPortForModule().
 * Method is called with invalid parameters.
 * asserted result: method should return -1.
 */
TEST_F(CosimaSchedulerTest, TestGetPortForModule) {
    CosimaSchedulerMock schedulerMock;
    // test with invalid module name
    ASSERT_EQ(schedulerMock.getPortForModule(""), -1);

    schedulerMock.deleteRegisteredModules();
    // test with unregistered module
    ASSERT_EQ(schedulerMock.getPortForModule("client"), -1);
}

/**
 * Test method handleMsgFromCoupledSimulation().
 * Method is called without any received data.
 * asserted result: method should return false.
 */
TEST_F(CosimaSchedulerTest, TestHandleMsgFromCoupledSimulation) {
    CosimaSchedulerModule *schedulerModule = new CosimaSchedulerModule();
    scheduler->setScenarioManager(new CosimaScenarioManager());
    scheduler->setInterfaceModule(schedulerModule, true);
    // no messages received, method should return false
    std::vector<char> data;
    ASSERT_EQ(scheduler->handleMsgFromCoupledSimulation(data), false);
    delete(schedulerModule);
}

/**
 * Test method takeNextEvent().
 * Method is called with no event in FES first, then with message in FES.
 * asserted result: method should return nullptr first, then message object.
 */
TEST_F(CosimaSchedulerTest, TestTakeNextEvent) {
    CosimaSchedulerMock schedulerMock;
    // no event in FES
    ASSERT_EQ(nullptr, schedulerMock.takeNextEvent());

    // insert message as event in FES
    cMessage *eventMsg = new cMessage();
    eventMsg->setArrivalTime(0);
    schedulerMock.getSimulation()->getFES()->insert(eventMsg);

    // initial message must be received in order to take event from FES
    schedulerMock.setInitialMessageReceived();
    // should return message object
    ASSERT_EQ(eventMsg, schedulerMock.takeNextEvent());
    delete eventMsg;
}
