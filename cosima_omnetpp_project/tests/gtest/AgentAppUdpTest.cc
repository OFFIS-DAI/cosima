/*
 * AgentAppUdpTest.cc
 *
 *  Created on: Feb 10, 2022
 *      Author: malin
 */

#include "main_test.h"
#include "../../modules/AgentAppUdp.h"
#include "../../modules/MosaikSchedulerModule.h"
#include "../../messages/MosaikApplicationChunk_m.h"

class AgentAppUdpMock : public AgentAppUdp {
public:
    // MosaikSchedulerMock scheduler;
    AgentAppUdpMock() {
    }

    void sendReply(MosaikSchedulerMessage *reply) {
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

    void setScheduler(MosaikSchedulerMock *mock) {
        scheduler = mock;
    }
};

class AgentAppUdpTest : public BaseOppTest {
public:
    MosaikScheduler *scheduler;
    AgentAppUdp *app;
    AgentAppUdpMock appMock;

    AgentAppUdpTest() {
        scheduler = check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());
        app = new AgentAppUdp();
    }

    ~AgentAppUdpTest() {
        delete app;
    }

    MosaikSchedulerMessage *createMosaikSchedulerMessage() {
        // create MosaikSchedulerMessage
        MosaikSchedulerMessage *msg = new MosaikSchedulerMessage();
        msg->setContent("content");
        msg->setSender("sender");
        msg->setReceiver("");
        msg->setMsgId("id");
        msg->setCreationTime(0);
        return msg;
    }

    inet::Packet *createPacketWithMosaikApplicationChunk() {
        inet::Packet *packet = new inet::Packet();
        const auto &payload = inet::makeShared<MosaikApplicationChunk>();
        payload->setContent("content");
        payload->setReceiver("receiver");
        payload->setSender("sender");
        payload->setChunkLength(inet::B(128));
        payload->setCreationTime(0);
        payload->setMsgId("message1");
        payload->setCreationTimeMosaik(0);
        packet->insertAtBack(payload);
        packet->setTimestamp(0);
        return packet;
    }
};

/**
 * Test method handleSocketEvent().
 * Method is called with message object
 * asserted result: method calls sendToMosaik from scheduler
 */
TEST_F(AgentAppUdpTest, TestHandleSocketEvent) {
    // create MosaikSchedulerMessage
    MosaikSchedulerMessage *msg = createMosaikSchedulerMessage();

    MosaikSchedulerMock schedulerMock;
    appMock.setScheduler(&schedulerMock);

    // In this case, OMNeT tries to send the message. Since there is no connection, there is an error which is catched
    // within the function already. Then, a notification is sent to the scheduler.
    EXPECT_CALL(schedulerMock, sendToMosaik(testing::_)).Times(1);
    appMock.handleSocketEvent(msg);
}

TEST_F(AgentAppUdpTest, TestHandleSocketEventIrrelevantMsgType) {
    // create other message than MosaikSchedulerMessage
    inet::Packet *packet = createPacketWithMosaikApplicationChunk();

    MosaikSchedulerMock schedulerMock;
    appMock.setScheduler(&schedulerMock);

    // In this case, the message object is no MosaikSchedulerMessage and is therefore directly deleted.
    // No message is sent to the scheduler.
    EXPECT_CALL(schedulerMock, sendToMosaik(testing::_)).Times(0);

    appMock.handleSocketEvent(packet);
}

TEST_F(AgentAppUdpTest, TestHandleMessage) {
    // create MosaikSchedulerMessage
    MosaikSchedulerMessage *msg = createMosaikSchedulerMessage();

    MosaikSchedulerMock schedulerMock;
    appMock.setScheduler(&schedulerMock);

    // since the message has never been sent, the gate can not be checked
    // therefore, a runtime error is thrown
    EXPECT_CALL(schedulerMock, sendToMosaik(testing::_)).Times(0);
    try {
        appMock.handleMessage(msg);
    }
    catch (const cRuntimeError& e) {
    }

    delete msg;
}


