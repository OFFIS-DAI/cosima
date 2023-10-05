/*
 * CosimaSchedulerModuleTest.cc
 *
 *  Created on: Feb 20, 2022
 *      Author: malin
 */
#include "main_test.h"
#include "../../modules/CosimaSchedulerModule.h"

class CosimaSchedulerModuleMock : public CosimaSchedulerModule {
public:
    CosimaSchedulerMock schedulerMock;

    void handleMessage(cMessage *msg) {
        CosimaSchedulerModule::handleMessage(msg);
    }
    void setMockObjectAsScheduler() {
        scheduler = &schedulerMock;
    }
    bool getUntilReachedValueFromSchedulerMock() {
        return schedulerMock.getUntilReached();
    }
};

class CosimaSchedulerModuleTest : public BaseOppTest {
public:
    CosimaSchedulerModule *schedulerModule;

    CosimaSchedulerModuleTest() {
        schedulerModule = new CosimaSchedulerModule();
    }

    ~CosimaSchedulerModuleTest() {
        delete schedulerModule;
    }
};


/*
 * Test constructor of CosimaSchedulerModule.
 * Asserted result: max advance event and until event should be initialized.
 */
TEST_F(CosimaSchedulerModuleTest, TestConstructor) {
    ASSERT_NE(schedulerModule->maxAdvEvent, nullptr);
    ASSERT_NE(schedulerModule->untilEvent, nullptr);
}

/**
 * Test method handleMsg() with different types of messages.
 */
TEST_F(CosimaSchedulerModuleTest, TestHandleMessage) {
    CosimaSchedulerModuleMock schedulerModuleMock;
    schedulerModuleMock.setMockObjectAsScheduler();

    // calling method with unknown message type -> sendToCoupledSimulation() should not be called in scheduler
    EXPECT_CALL(schedulerModuleMock.schedulerMock, sendToCoupledSimulation).Times(0);
    schedulerModuleMock.handleMessage(new cMessage());

    // calling method with CosimaCtrlEvent (type 0: max advance event)
    // -> sendToCoupledSimulation() should not be called in scheduler
    EXPECT_CALL(schedulerModuleMock.schedulerMock, sendToCoupledSimulation).WillOnce(testing::Return());
    CosimaCtrlEvent *maxAdvEvent = new CosimaCtrlEvent();
    maxAdvEvent->setCtrlType(0);
    schedulerModuleMock.handleMessage(maxAdvEvent);
    delete maxAdvEvent;

    // calling method with CosimaCtrlEvent (type 2: until event)
    // -> until reached should be set in scheduler
    CosimaCtrlEvent *untilEvent = new CosimaCtrlEvent();
    untilEvent->setCtrlType(2);
    schedulerModuleMock.handleMessage(untilEvent);
    ASSERT_EQ(schedulerModuleMock.getUntilReachedValueFromSchedulerMock(), true);
    delete untilEvent;

    // calling method with CosimaCtrlEvent (type 1: end of step event)
    // -> method sendMsgGroupToCosima() should be called in scheduler
    EXPECT_CALL(schedulerModuleMock.schedulerMock, sendMsgGroupToCoupledSimulation).WillOnce(testing::Return());
    CosimaCtrlEvent *stepEndEvent = new CosimaCtrlEvent();
    stepEndEvent->setCtrlType(1);
    schedulerModuleMock.handleMessage(stepEndEvent);
}




