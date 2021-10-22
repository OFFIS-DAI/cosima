#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "fakeit.hpp"
#include "SocketAgentApp.h"
#include <string.h>
#include <omnetpp.h>

// Instantiate mock objects.
fakeit::Mock<cSimulation> mockSimulation;
fakeit::Mock<SocketAgentApp> mockApp;
fakeit::Mock<cMessage> mockMsg;
fakeit::Mock<cModule> mockModule;
fakeit::Mock<inet::Packet> mockPacket;
fakeit::Mock<TikTokPacket> mockTikTokPacket;

cSimulation &mockedSimulation = mockSimulation.get();
SocketAgentApp &mockedSocketAgentApp = mockApp.get();
cMessage &mockedMsg = mockMsg.get();
cModule &mockedModule = mockModule.get();
inet::Packet &mockedPacket = mockPacket.get();
TikTokPacket &mockedTikTokPacket = mockTikTokPacket.get();

TikTokPacket *answer = &mockedTikTokPacket;
cMessage *msgpacket = &mockedPacket;
cMessage *nullptrMsg = nullptr;


TEST_CASE("Instantiate SocketAgentApp") {
    fakeit::Mock<SocketAgentApp> mockApp;
    SocketAgentApp &socketAgentApp = mockApp.get();
    REQUIRE(socketAgentApp.numRecvBytes == 0);
}

TEST_CASE("Test handleMessage") {

    SECTION("Message is nullptr") {
        // mock method handleMessage()
        fakeit::Fake(Method(mockApp, handleMessage));
        // call handleMessage() with nullptr message
        mockedSocketAgentApp.handleMessage(nullptrMsg);
        // verify method is called
        fakeit::Verify(Method(mockApp,handleMessage));
        // verify method is called with argument nullptr
        fakeit::Verify(Method(mockApp,handleMessage).Using(nullptr));
    }

    SECTION("Message is INET packet from network") {

        SECTION("Message is nullptr") {
            // mock method handleSocketEvent()
            fakeit::Fake(Method(mockApp, handleMosaikMessage));

            // call handleMosaikMessage() with nullptr message
            // method should return false
            REQUIRE(mockedSocketAgentApp.handleMosaikMessage(nullptrMsg) == false);

        }

        SECTION("Message is inet::Packet") {

            // call handleMosaikMessage() with inet::Packet
            // method should return true
            //REQUIRE(mockedSocketAgentApp.handleMosaikMessage(msgpacket) == true);

            //fakeit::Fake(Method(mockSimulation, getSimTime));
            //fakeit::When(Method(mockSimulation, getSimTime)).Return(opp_get_monotonic_clock_usecs());

            //fakeit::Fake(Method(mockApp, handleReply));
            //fakeit::Fake(Method(mockApp, generateAnswer));
            //REQUIRE(mockedSocketAgentApp.handleMosaikMessage(msgpacket) == true);
            //mockedSocketAgentApp.handleMosaikMessage(msgpacket);

            //fakeit::Verify(Method(mockApp, handleMosaikMessage));
            //fakeit::Verify(Method(mockApp, handleReply));

            fakeit::When(Method(mockApp, handleSocketEvent)).Return(true);
            fakeit::When(Method(mockApp, handleMosaikMessage)).Return(true);
            fakeit::When(Method(mockApp, messageIsFromNetwork)).Return(true);

            mockedSocketAgentApp.handleMessage(msgpacket);
            fakeit::Verify(Method(mockApp,handleMosaikMessage));
        }
    }

    SECTION("Message is placed by observer") {
        // mock method handleSocketEvent()
        fakeit::Fake(Method(mockApp, handleSocketEvent));

        SECTION("Message is nullptr") {
            // call handleSocketEvent() with nullptr message
            // method should return false
            REQUIRE(mockedSocketAgentApp.handleSocketEvent(nullptrMsg) == false);
        }

        SECTION("Message is socket event") {
            //            fakeit::Fake(Method(mockApp, handleSocketEvent));
            //            fakeit::Fake(Method(mockApp, handleMosaikMessage));
            //            fakeit::Fake(Method(mockApp, messageIsFromNetwork));
            //
            //            mockedSocketAgentApp.handleMessage(msgpacket);
            //            fakeit::Verify(Method(mockApp,handleMosaikMessage));
        }
    }
}
