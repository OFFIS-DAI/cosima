/*
 * MosaikSchedulerTest.cc
 *
 *  Created on: Feb 6, 2022
 *      Author: malin
 */

#include "main_test.h"
#include "../../modules/AgentAppTcp.h"
#include "../../modules/MosaikSchedulerModule.h"
#include "../../modules/MosaikScenarioManager.h"

class MosaikSchedulerTest : public BaseOppTest {
public:
    MosaikScheduler *scheduler;

    MosaikSchedulerTest() {
        scheduler = check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());
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
     * Creates MosaikSchedulerModule object.
     */
    MosaikSchedulerModule *createMosaikSchedulerModule() {
        MosaikSchedulerModule *schedulerModule = new MosaikSchedulerModule();
        return schedulerModule;
    }

    /**
     * Creates MosaikSchedulerMessage with delay info.
     */
    MosaikSchedulerMessage *createValidMosaikSchedulerMessageWithDelayInfo() {
        MosaikSchedulerMessage *message = new MosaikSchedulerMessage();
        message->setSender("sender");
        message->setReceiver("receiver");
        message->setMsgId("id");
        message->setContent("content");
        message->setDelay(10);
        message->setCreationTime(1);
        return message;
    }

    /**
     * Creates MosaikCtrlEvent.
     */
    MosaikCtrlEvent *createValidMosaikCtrlEvent() {
        return new MosaikCtrlEvent();
    }

};

/**
 * Test method startRun().
 * asserted result: listener socket for mosaik is initialized.
 */
TEST_F(MosaikSchedulerTest, TestSchedulerIsRunning) {
    scheduler->startRun();
    ASSERT_TRUE(scheduler->socketToMosaikInitialized());
}

/**
 * Test method setInterfaceModule().
 * AgentAppTcp registers at scheduler.
 * asserted result: scheduler adds application to internal list.
 */
TEST_F(MosaikSchedulerTest, AddModuleToList) {
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
TEST_F(MosaikSchedulerTest, AddMosaikSchedulerModuleToList) {
    MosaikSchedulerModule *schedulerModule = createMosaikSchedulerModule();
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
TEST_F(MosaikSchedulerTest, TryCallingSetInterfaceModuleWithNullptr) {
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
 * Test method sendToMosaik().
 * Method is called with null-pointer.
 * asserted result: error is thrown.
 */
TEST_F(MosaikSchedulerTest, TestSendToMosaikWithNullptr) {
    try {
        scheduler->sendToMosaik(nullptr);
        // test fails, if we reach this point
        FAIL();
    }
    catch(const cRuntimeError& expected) {
    }
    // no message should be added to pmsg group
    ASSERT_EQ(scheduler->getNumberOfMessagesInPMSGGroup(), 0);
}

/**
 * Test method sendToMosaik().
 * Method is called with delay reply from AgentAppUdp.
 * asserted result: current pmsg group for mosaik contains one element.
 */
TEST_F(MosaikSchedulerTest, TestSendToMosaikWithDelayMsg) {
    MosaikSchedulerMessage *delayMsg = createValidMosaikSchedulerMessageWithDelayInfo();
    scheduler->sendToMosaik(delayMsg);
    ASSERT_EQ(scheduler->getNumberOfMessagesInPMSGGroup(), 1);
    delete delayMsg;
}

/**
 * Test method sendToMosaik().
 * Method is called with control message.
 * asserted result: current pmsg group for mosaik contains two elements (one of the test before and one from this).
 */
TEST_F(MosaikSchedulerTest, TestSendToMosaikWithCtrlEvent) {
    MosaikCtrlEvent *event = createValidMosaikCtrlEvent();
    scheduler->sendToMosaik(event);
    ASSERT_EQ(scheduler->getNumberOfMessagesInPMSGGroup(), 2);
    delete event;
}

/**
 * Test method getPortForModule().
 * Method is called with invalid parameters.
 * asserted result: method should return -1.
 */
TEST_F(MosaikSchedulerTest, TestGetPortForModule) {
    MosaikSchedulerMock schedulerMock;
    // test with invalid module name
    ASSERT_EQ(schedulerMock.getPortForModule(""), -1);

    schedulerMock.deleteRegisteredModules();
    // test with unregistered module
    ASSERT_EQ(schedulerMock.getPortForModule("client"), -1);
}

/**
 * Test method handleMsgFromMosaik().
 * Method is called without any received data.
 * asserted result: method should return false.
 */
TEST_F(MosaikSchedulerTest, TestHandleMsgFromMosaik) {
    MosaikSchedulerModule *schedulerModule = new MosaikSchedulerModule();
    scheduler->setScenarioManager(new MosaikScenarioManager());
    scheduler->setInterfaceModule(schedulerModule, true);
    // no messages received, method should return false
    ASSERT_EQ(scheduler->handleMsgFromMosaik(), false);
    delete(schedulerModule);
}

/**
 * Test method takeNextEvent().
 * Method is called with no event in FES first, then with message in FES.
 * asserted result: method should return nullptr first, then message object.
 */
TEST_F(MosaikSchedulerTest, TestTakeNextEvent) {
    MosaikSchedulerMock schedulerMock;
    // no event in FES
    ASSERT_EQ(nullptr, schedulerMock.takeNextEvent());

    // insert message as event in FES
    cMessage *eventMsg = new cMessage();
    eventMsg->setArrivalTime(0);
    schedulerMock.getSimulation()->getFES()->insert(eventMsg);

    // initial message must be received in order to take event from FES
    schedulerMock.initial_message_received = true;
    // should return message object
    ASSERT_EQ(eventMsg, schedulerMock.takeNextEvent());
    delete eventMsg;
}
