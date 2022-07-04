/*
 * MosaikSchedulerModuleTest.cc
 *
 *  Created on: Feb 20, 2022
 *      Author: malin
 */
#include "main_test.h"
#include "../../modules/MosaikSchedulerModule.h"

class MosaikSchedulerModuleMock : public MosaikSchedulerModule {
public:
    MosaikSchedulerMock schedulerMock;

    void handleMessage(cMessage *msg) {
        MosaikSchedulerModule::handleMessage(msg);
    }
    void setMockObjectAsScheduler() {
        scheduler = &schedulerMock;
    }
    bool getUntilReachedValueFromSchedulerMock() {
        return schedulerMock.getUntilReached();
    }
};

class MosaikSchedulerModuleTest : public BaseOppTest {
public:
    MosaikSchedulerModule *schedulerModule;

    MosaikSchedulerModuleTest() {
        schedulerModule = new MosaikSchedulerModule();
    }

    ~MosaikSchedulerModuleTest() {
        delete schedulerModule;
    }
};


/*
 * Test constructor of MosaikSchedulerModule.
 * Asserted result: max advance event and until event should be initialized.
 */
TEST_F(MosaikSchedulerModuleTest, TestConstructor) {
    ASSERT_NE(schedulerModule->maxAdvEvent, nullptr);
    ASSERT_NE(schedulerModule->untilEvent, nullptr);
}

/**
 * Test method handleMsg() with different types of messages.
 */
TEST_F(MosaikSchedulerModuleTest, TestHandleMessage) {
    MosaikSchedulerModuleMock schedulerModuleMock;
    schedulerModuleMock.setMockObjectAsScheduler();

    // calling method with unknown message type -> sendToMosaik() should not be called in scheduler
    EXPECT_CALL(schedulerModuleMock.schedulerMock, sendToMosaik).Times(0);
    schedulerModuleMock.handleMessage(new cMessage());

    // calling method with MosaikCtrlEvent (type 0: max advance event)
    // -> sendToMosaik() should be called in scheduler
    EXPECT_CALL(schedulerModuleMock.schedulerMock, sendToMosaik).WillOnce(testing::Return());
    MosaikCtrlEvent *maxAdvEvent = new MosaikCtrlEvent();
    maxAdvEvent->setCtrlType(0);
    schedulerModuleMock.handleMessage(maxAdvEvent);
    delete maxAdvEvent;

    // calling method with MosaikCtrlEvent (type 2: until event)
    // -> until reached should be set in scheduler
    MosaikCtrlEvent *untilEvent = new MosaikCtrlEvent();
    untilEvent->setCtrlType(2);
    schedulerModuleMock.handleMessage(untilEvent);
    ASSERT_EQ(schedulerModuleMock.getUntilReachedValueFromSchedulerMock(), true);
    delete untilEvent;

    // calling method with MosaikCtrlEvent (type 1: end of step event)
    // -> method sendMsgGroupToMosaik() should be called in scheduler
    EXPECT_CALL(schedulerModuleMock.schedulerMock, sendMsgGroupToMosaik).WillOnce(testing::Return());
    MosaikCtrlEvent *stepEndEvent = new MosaikCtrlEvent();
    stepEndEvent->setCtrlType(1);
    schedulerModuleMock.handleMessage(stepEndEvent);
}




