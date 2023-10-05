/*
 * ScenarioManagerTest.cc
 *
 *  Created on: Feb 21, 2022
 *      Author: malin
 */

#include "main_test.h"
#include "../../modules/CosimaScenarioManager.h"
#include "../../messages/CosimaCtrlEvent_m.h"


class CosimaScenarioManagerMock : public CosimaScenarioManager {
public:
    CosimaSchedulerMock schedulerMock;

    MOCK_METHOD(CosimaSchedulerMessage*, connect, (const char *));
    MOCK_METHOD(CosimaSchedulerMessage*, disconnect, (const char *));

    void handleMessage(cMessage *msg) {
        CosimaScenarioManager::handleMessage(msg);
    }

    void setMockObjectAsScheduler() {
        scheduler = &schedulerMock;
    }
};

class CosimaScenarioManagerTest : public BaseOppTest {
public:
    CosimaScenarioManagerMock scenarioManagerMock;

    CosimaCtrlEvent *createEventOfType(int type) {
        CosimaCtrlEvent *event = new CosimaCtrlEvent();
        event->setCtrlType(type);
        return event;
    }
};

/**
 * Test method handleMessage().
 * Calling with invalid message types.
 * Asserted result: exceptions are thrown.
 * Calling with valid message types (connect and disconnect).
 * Asserted result: no expection thrown.
 *
 * Possible error cases for methods connect() and disconnect() are also tested that way.
 */
TEST_F(CosimaScenarioManagerTest, TestHandleMessage) {
    // call handleMessage() with nullptr
    // we assert a cRuntimeError
    try {
        scenarioManagerMock.handleMessage(nullptr);
        // test fails, if we reach this point
        FAIL();
    }
    catch(const cRuntimeError& expected) {
    }

    // call handleMessage() with correct message type and ctrl type disconnect, should not thrown an exception
    CosimaCtrlEvent *disconnectEvent = createEventOfType(3);
    disconnectEvent->setModuleNamesArraySize(1);
    disconnectEvent->setModuleNames(0, "client");
    try {
        scenarioManagerMock.handleMessage(disconnectEvent);
    }
    catch(const cRuntimeError& expected) {
        // test fails, if we reach this point
        FAIL();
    }

    // call handleMessage() with correct message type and ctrl type connect, should not thrown an exception
    EXPECT_CALL(scenarioManagerMock.schedulerMock, sendToCoupledSimulation).WillOnce(testing::Return());
    CosimaCtrlEvent *connectEvent = createEventOfType(4);
    connectEvent->setModuleNamesArraySize(1);
    connectEvent->setModuleNames(0, "client");
    try {
        scenarioManagerMock.setMockObjectAsScheduler();
        scenarioManagerMock.handleMessage(connectEvent);
    }
    catch(const cRuntimeError& expected) {
        // test fails, if we reach this point
        FAIL();
    }
}
