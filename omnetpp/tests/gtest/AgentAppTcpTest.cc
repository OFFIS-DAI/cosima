/*
 * AgentAppTcpTest.cc
 *
 *  Created on: Feb 10, 2022
 *      Author: malin
 */

#include "main_test.h"
#include "../../modules/AgentAppTcp.h"
#include "../../modules/CosimaSchedulerModule.h"
#include "../../messages/CosimaApplicationChunk_m.h"

class AgentAppTcpMock : public AgentAppTcp {
public:
    AgentAppTcpMock() {
        this->addGate("socketIn", cGate::INPUT, 0);
        this->addGate("socketOut", cGate::OUTPUT, 0);;
    }

    bool handleSocketEvent(cMessage *msg, double couplingSimTime) {
        return AgentAppTcp::handleSocketEvent(msg, couplingSimTime);
    }

    Timer *getTimerForModuleId(int clientId) {
        return AgentAppTcp::getTimerForModuleId(clientId);
    }

    void setPacketForModuleId(int clientId, inet::Packet *packet) {
        return AgentAppTcp::setPacketForModuleId(clientId, packet);
    }

    inet::Packet *getPacketForModuleId(int clientId) {
        return AgentAppTcp::getPacketForModuleId(clientId);
    }

    int getModuleIdByName(const char *module_name) {
        return AgentAppTcp::getModuleIdByName(module_name);
    }

    void handleTimer(cMessage *msg) {
        return AgentAppTcp::handleTimer(msg);
    }

    void sendReply(CosimaSchedulerMessage *reply) {
        return AgentAppTcp::sendReply(reply);
    }

    void setScheduler(CosimaSchedulerMock *mock) {
        scheduler = mock;
    }

};

class AgentAppTcpTest : public BaseOppTest {
public:
    CosimaScheduler *scheduler;
    AgentAppTcp *app;
    AgentAppTcpMock appMock;

    AgentAppTcpTest() {
        scheduler = check_and_cast<CosimaScheduler *>(getSimulation()->getScheduler());
        app = new AgentAppTcp();
    }

    ~AgentAppTcpTest() {
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

    CosimaSchedulerMessage *createCosimaSchedulerMessageWithMissingInformation() {
        // create CosimaSchedulerMessage
        CosimaSchedulerMessage *msg = new CosimaSchedulerMessage();
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
 * Method is called with timer object.
 * asserted result: method should return false, because that's not what we expect.
 */
TEST_F(AgentAppTcpTest, TestHandleSocketEventWithWrongMsgType) {
    // create timer
    Timer *timer = new Timer();
    timer->setTimerType(-1);

    // execute method
    bool check = appMock.handleSocketEvent(timer, 0);

    // check expectation
    EXPECT_FALSE(check);
}

/**
 * Test method handleSocketEvent().
 * Method is called with message object, but negative simtime.
 * asserted result: method should return false, because messages can't be scheduled in the past.
 */
TEST_F(AgentAppTcpTest, TestHandleSocketEventWithWrongSimTime) {
    // create CosimaSchedulerMessage
    CosimaSchedulerMessage *msg = createCosimaSchedulerMessage();

    // execute method
    bool check = appMock.handleSocketEvent(msg, -1);

    // check expectation
    EXPECT_FALSE(check);
}

/**
 * Test method handleSocketEvent().
 * Method is called with message object, but the message is missing information.
 * asserted result: method should return false, because information can't be extracted.
 */
TEST_F(AgentAppTcpTest, TestHandleSocketEventWithMissingInformationInMessage) {
    // create CosimaSchedulerMessage
    CosimaSchedulerMessage *msg = createCosimaSchedulerMessageWithMissingInformation();

    // execute method
    bool check = appMock.handleSocketEvent(msg, -1);

    // check expectation
    EXPECT_FALSE(check);
}

/**
 * Test method getTimerForModuleId().
 * Method is first called two times with the same clientId, then with a different.
 * asserted result: first two timer objects are the same, the third one differs.
 */
TEST_F(AgentAppTcpTest, TestGetTimerForModuleId) {
    // generate first timer
    Timer *timer = appMock.getTimerForModuleId(0);
    // generate second timer (should be the same)
    Timer *sameTimer = appMock.getTimerForModuleId(0);

    // Timer objects should be the same, because it's the same clientId.
    EXPECT_EQ(timer, sameTimer);

    // generate third timer with different clientId
    Timer *differentTimer = appMock.getTimerForModuleId(1);

    // Timer objects should not be the same.
    EXPECT_NE(timer, differentTimer);
    EXPECT_NE(sameTimer, differentTimer);
}

/**
 * Test methods setPacketForModuleId() and getPacketForModuleId().
 * Methods are first called for the same clientId, then for a different.
 * asserted result: packets for same id should be the same.
 */
TEST_F(AgentAppTcpTest, TestSetAndGetPacketForClient) {
    // prepare packet
    int clientId = 0;
    inet::Packet *packet = createPacketWithCosimaApplicationChunk();

    // set packet for module
    appMock.setPacketForModuleId(clientId, packet);

    // packets should be the same
    EXPECT_EQ(packet, appMock.getPacketForModuleId(clientId));

    // prepare packet for client with id 1
    int otherClientId = 1;
    inet::Packet *otherPacket = createPacketWithCosimaApplicationChunk();

    // set packet for module
    appMock.setPacketForModuleId(otherClientId, otherPacket);

    // packets for client with id 1 should be the same
    EXPECT_EQ(otherPacket, appMock.getPacketForModuleId(otherClientId));
    // packets for client with id 0 and client with id 1 should not be the same
    EXPECT_NE(packet, appMock.getPacketForModuleId(otherClientId));

    // clean up
    delete packet;
    delete otherPacket;
}

/**
 * Test method getModuleIdByName().
 * Method is called with invalid parameters.
 * asserted result: module id should be -1.
 */
TEST_F(AgentAppTcpTest, TestGetModuleIdByName) {
    // should return -1 when called with nullptr
    EXPECT_EQ(appMock.getModuleIdByName(nullptr), -1);

    // should return -1 when called with empty string
    EXPECT_EQ(appMock.getModuleIdByName(""), -1);
}

/**
 * Test method handleTimer().
 * Method is called with invalid parameters.
 * asserted result: cRuntimeError should be thrown.
 */
TEST_F(AgentAppTcpTest, TestHandleTimerWithInvalidTimers) {
    // call method with wrong message object
    cMessage *msg = new cMessage();
    try {
        appMock.handleTimer(msg);
        // test fails, if we reach this point
        FAIL();
    }
    catch(const cRuntimeError& expected) {
    }

    // clean up
    delete msg;

    // call method with nullptr
    try {
        appMock.handleTimer(nullptr);
        // test fails, if we reach this point
        FAIL();
    }
    catch(const cRuntimeError& expected) {
    }
}

/**
 * Test method sendReply().
 * Method is called with CosimaSchedulerMessage.
 * asserted result: method sendToCoupledSimulation() should be called in scheduler.
 */
TEST_F(AgentAppTcpTest, TestSendReply) {
    // create mock object
    CosimaSchedulerMock schedulerMock;
    appMock.setScheduler(&schedulerMock);

    CosimaSchedulerMessage *msg = new CosimaSchedulerMessage();

    // method sendMsgGroupToCoupledSimulation() should be called in scheduler
    EXPECT_CALL(schedulerMock, sendToCoupledSimulation(msg)).Times(1);

    // call method
    appMock.sendReply(msg);

    // clean up
    delete msg;
}

