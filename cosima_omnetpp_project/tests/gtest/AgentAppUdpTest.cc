/*
 * AgentAppUdpTest.cc
 *
 *  Created on: Feb 10, 2022
 *      Author: malin
 */

#include "main_test.h"
#include "../../modules/AgentAppUdp.h"
#include "../../modules/CosimaSchedulerModule.h"
#include "../../messages/CosimaApplicationChunk_m.h"

class AgentAppUdpMock : public AgentAppUdp {
public:
    // CosimaSchedulerMock scheduler;
    AgentAppUdpMock() {
    }

    void sendReply(CosimaSchedulerMessage *reply) {
        return;
    }

    void handleMessage(cMessage *msg) override {
        return AgentAppUdp::handleMessage(msg);
    }

    void handleSocketEvent(cMessage *msg) {
        return AgentAppUdp::handleSocketEvent(msg);
    }

    void initialize(int stage) override {
        return AgentAppUdp::initialize(stage);
    }

    int numInitStages() const override { return (inet::NUM_INIT_STAGES); }

    void setScheduler(CosimaSchedulerMock *mock) {
        scheduler = mock;
    }
};

class AgentAppUdpTest : public BaseOppTest {
public:
    CosimaScheduler *scheduler;
    AgentAppUdp *app;
    AgentAppUdpMock appMock;

    AgentAppUdpTest() {
        scheduler = check_and_cast<CosimaScheduler *>(getSimulation()->getScheduler());
        app = new AgentAppUdp();
    }

    ~AgentAppUdpTest() {
        delete app;
    }

    CosimaSchedulerMessage *createCosimaSchedulerMessage() {
        // create CosimaSchedulerMessage
        CosimaSchedulerMessage *msg = new CosimaSchedulerMessage();
        msg->setContent("content");
        msg->setSender("sender");
        msg->setReceiver("");
        msg->setMsgId("id");
        msg->setCreationTime(0);
        return msg;
    }

    inet::Packet *createPacketWithCosimaApplicationChunk() {
        inet::Packet *packet = new inet::Packet();
        const auto &payload = inet::makeShared<CosimaApplicationChunk>();
        payload->setContent("content");
        payload->setReceiver("receiver");
        payload->setSender("sender");
        payload->setChunkLength(inet::B(128));
        payload->setCreationTimeOmnetpp(0);
        payload->setMsgId("message1");
        payload->setCreationTimeCoupling(0);
        packet->insertAtBack(payload);
        packet->setTimestamp(0);
        return packet;
    }
};

/**
 * Test method handleSocketEvent().
 * Method is called with message object
 * asserted result: method calls sendToCoupledSimulation from scheduler
 */
TEST_F(AgentAppUdpTest, TestHandleSocketEvent) {
    // create CosimaSchedulerMessage
    CosimaSchedulerMessage *msg = createCosimaSchedulerMessage();

    CosimaSchedulerMock schedulerMock;
    appMock.setScheduler(&schedulerMock);

    // In this case, OMNeT tries to send the message. Since there is no connection, there is an error which is catched
    // within the function already. Then, a notification is sent to the scheduler.
    EXPECT_CALL(schedulerMock, sendToCoupledSimulation(testing::_)).Times(1);
    appMock.handleSocketEvent(msg);
}

TEST_F(AgentAppUdpTest, TestHandleSocketEventIrrelevantMsgType) {
    // create other message than CosimaSchedulerMessage
    inet::Packet *packet = createPacketWithCosimaApplicationChunk();

    CosimaSchedulerMock schedulerMock;
    appMock.setScheduler(&schedulerMock);

    // In this case, the message object is no CosimaSchedulerMessage and is therefore directly deleted.
    // No message is sent to the scheduler.
    EXPECT_CALL(schedulerMock, sendToCoupledSimulation(testing::_)).Times(0);

    appMock.handleSocketEvent(packet);
}

TEST_F(AgentAppUdpTest, TestHandleMessage) {
    // create CosimaSchedulerMessage
    CosimaSchedulerMessage *msg = createCosimaSchedulerMessage();

    CosimaSchedulerMock schedulerMock;
    appMock.setScheduler(&schedulerMock);

    // since the message has never been sent, the gate can not be checked
    // therefore, a runtime error is thrown
    EXPECT_CALL(schedulerMock, sendToCoupledSimulation(testing::_)).Times(0);
    try {
        appMock.handleMessage(msg);
    }
    catch (const cRuntimeError& e) {
    }

    delete msg;
}


