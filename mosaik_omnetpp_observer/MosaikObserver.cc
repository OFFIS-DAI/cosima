/*
 * MosaikObserver.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */
#include "MosaikObserver.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include "MosaikMessage_m.h"
#include "SocketAgentAppTcp.h"
#include "message.pb.h"
#include "MosaikCtrlMsg_m.h"
#include "ControlType_m.h"
#include "MosaikObserverModule.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

Register_Class(MosaikObserver);

using namespace omnetpp;

MosaikObserver::MosaikObserver() : cScheduler() {
    listenerSocket = INVALID_SOCKET;
    connSocket = INVALID_SOCKET;
    socketEvent = nullptr;
    numBytesPtr = nullptr;
    module = nullptr;
    notificationMsg = nullptr;
    recvBuffer = nullptr;
    recvBufferSize = 0;
    numModules = 0;
    numOfBytes = 0;
    numBytesPtr = nullptr;
    realtime = false;
    port = 4242;
    baseTime = 0;
    module = nullptr;
}

MosaikObserver::~MosaikObserver() = default;

std::string MosaikObserver::info() const {
    return ("scheduler with socket option for mosaik integration");
}

void MosaikObserver::startRun() {
    if (initsocketlibonce() != 0)
        throw cRuntimeError("MosaikObserver: Cannot initialize socket library");

    if (getSimulation()->getEnvir()->getConfig()->getConfigValue(
            "experiment-label") != nullptr) {
        const char *expected_value = "real-time";
        const char *given_value =
                getSimulation()->getEnvir()->getConfig()->getConfigValue(
                        "experiment-label");
        if (strcmp(given_value, expected_value) == 0) {
            realtime = true;
        } else {
            realtime = false;
        }
    } else {
        realtime = false;
    }

    if (realtime) {
        baseTime = opp_get_monotonic_clock_usecs();
    }
    setupListener();
}

void MosaikObserver::setupListener() {
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket == INVALID_SOCKET)
        throw cRuntimeError("MosaikObserver: cannot create socket");

    // Setting this option makes it possible to kill the sample and restart
    // it right away without having to change the port it is listening on.
    // Not using SO_REUSEADDR as per: https://stackoverflow.com/a/34812994
    // Correction: There is no SO_REUSEPORT on Windows, so SO_REUSEADDR it is.
    int enable = 1;
    if (setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR,
            (const char *)&enable, sizeof(int)) < 0)
        throw cRuntimeError("MosaikObserver: cannot set socket option");

    sockaddr_in sinInterface;
    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = INADDR_ANY;
    sinInterface.sin_port = htons(port);
    if (bind(listenerSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in)) ==
            SOCKET_ERROR)
        throw cRuntimeError("MosaikObserver: socket bind() failed");

    listen(listenerSocket, SOMAXCONN);
}

void MosaikObserver::endRun() {
}

void MosaikObserver::executionResumed() {
    if (realtime) {
        baseTime = opp_get_monotonic_clock_usecs();
        baseTime = baseTime - simTime().inUnit(SIMTIME_US);
    } else {
        cScheduler::executionResumed();
    }
}

void MosaikObserver::setInterfaceModule(cModule *mod, char *buf, int bufSize,
        int *nBytesPtr) {
    std::string module_name = std::string(mod->getName());
    if (module_name.compare("observerModule") == 0) {
        observerModule = mod;
    } else {
        if (!mod || !buf || !bufSize || !nBytesPtr)
                throw cRuntimeError(
                        "MosaikObserver: setInterfaceModule(): arguments must be non-nullptr");
        module = mod;
        recvBuffer = buf;
        recvBufferSize = bufSize;
        numBytesPtr = nBytesPtr;
        // possible to add several modules to array
        addModule(mod);
    }
}

void MosaikObserver::addModule(cModule *mod) {
    modules[numModules] = mod;
    numModules = numModules + 1;
}

int MosaikObserver::getPortForModule(std::string text) {
    cModule *submod = getReceiverModule(text);
    return (submod->par("localPort"));
}

cModule *MosaikObserver::getReceiverModule(std::string text) {
    // get corresponding module to server name
    cModule *found_module = nullptr;
    for (int i = 0; i < numModules; i++) {
        std::string module = std::string(modules[i]->getParentModule()->getName());
        if (module.compare(text) == 0) {
            found_module = modules[i];
        }
    }
    return (found_module);
}

char *MosaikObserver::getRecvBuffer() {
    // utility function for SocketAgentApp
    char *copy = recvBuffer;
    *numBytesPtr = 0;
    return (copy);
}

bool MosaikObserver::receiveWithTimeout(long usec) {
    recvBuffer = getRecvBuffer();
    // prepare sets for select()
    fd_set readFDs = {};
    fd_set writeFDs = {};
    fd_set exceptFDs = {};
    FD_ZERO(&readFDs);
    FD_ZERO(&writeFDs);
    FD_ZERO(&exceptFDs);

    // if we're connected, watch connSocket, otherwise accept new connections
    if (connSocket != INVALID_SOCKET)
        FD_SET(connSocket, &readFDs);
    else
        FD_SET(listenerSocket, &readFDs);

    timeval timeout = {};
    timeout.tv_sec = 0;
    timeout.tv_usec = usec;

    bool a = connSocket == INVALID_SOCKET;

    if (select(FD_SETSIZE, &readFDs, &writeFDs, &exceptFDs, &timeout) > 0) {
        // Something happened on one of the sockets -- handle them
        if (connSocket != INVALID_SOCKET && FD_ISSET(connSocket, &readFDs)) {
            // receive from connSocket
            char *bufPtr = recvBuffer + (*numBytesPtr);
            int bufLeft = recvBufferSize - (*numBytesPtr);
            if (bufLeft <= 0)
                throw cRuntimeError(
                        "MosaikObserver: interface module's recvBuffer is full");
            int nBytes = recv(connSocket, bufPtr, bufLeft, 0);
            if (nBytes == SOCKET_ERROR) {
                EV << "MosaikObserver: socket error " << sock_errno() << "\n";
                closesocket(connSocket);
                connSocket = INVALID_SOCKET;
            } else if (nBytes == 0) {
                EV << "MosaikObserver: socket closed by the client\n";
                if (shutdown(connSocket, SHUT_WR) == SOCKET_ERROR) {
                    EV << "cMosaikObserverRT: shutdown failed, socket already closed."
                            << "\n";
                    connSocket = INVALID_SOCKET;
                } else {
                    closesocket(connSocket);
                    connSocket = INVALID_SOCKET;
                }
            } else {
                // schedule socketEvent for the interface module
                (*numBytesPtr) += nBytes;
                std::string text = std::string(recvBuffer);
                EV << "received Text: " << text << endl;
                std::cout << "received Text: " << text << endl;
                if (realtime) {
                    int64_t currentTime = opp_get_monotonic_clock_usecs();
                    simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
                    ASSERT(eventTime >= simTime());
                }
                return (handleCmsg());
            }
        } else if (FD_ISSET(listenerSocket, &readFDs)) {
            // accept connection, and store FD in connSocket
            sockaddr_in sinRemote;
            int addrSize = sizeof(sinRemote);
            connSocket = accept(listenerSocket, (sockaddr *)&sinRemote,
                    (socklen_t *)&addrSize);
            if (connSocket == INVALID_SOCKET)
                throw cRuntimeError("MosaikObserver: accept() failed");
            EV << "MosaikObserver: connected!\n";
        }
    }
    return (false);
}

bool MosaikObserver::handleCmsg() {
    bool insertedEvent = false;
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    CosimaMsgGroup pmsg_group;
    pmsg_group.ParseFromString(recvBuffer);
    for(int i=0; i<pmsg_group.msg().size(); i++) {
        int type = pmsg_group.msg().Get(i).type();

        if (type == CosimaMsgGroup::CosimaMsg::INFO) {
            std::cout << "command type: message" << endl;
            // get sender from message
            std::string sender = pmsg_group.msg().Get(i).sender();
            std::string receiver = pmsg_group.msg().Get(i).receiver();
            std::string content = pmsg_group.msg().Get(i).content();
            int size = pmsg_group.msg().Get(i).size();
            int maxAdvance = pmsg_group.msg().Get(i).max_advance();
            std::cout << "max advance: " << maxAdvance << endl;
            double mosaikSimTime = pmsg_group.msg().Get(i).simtime() * 1.0;
            double stepSize = pmsg_group.msg().Get(i).stepsize() * 1.0;
            std::cout << "stepSize is " << stepSize << endl;
            std::cout << "simTime is " << mosaikSimTime << endl;
            double adjustedMosaikSimTime = mosaikSimTime/1000 * stepSize;
            std::cout << "mosaikSimTime is " << mosaikSimTime << endl;

            EV << "Mosaik Observer received message with sender: " << sender << endl;
            module = getReceiverModule(sender);
            // schedule notificationMsg
            if (module) {
                std::string text = "{\"sender\": \"" + sender + "\", \"receiver\": \"" +
                        receiver + "\", \"size\": " + std::to_string(size) +
                        ", \"content\": \"" + content + "\" ";
                TikTokPacket *msg = new TikTokPacket();
                msg->setSender(sender.c_str());
                msg->setReceiver(receiver.c_str());
                msg->setSize(size);
                msg->setContent(content.c_str());
                cPacket *pkt = dynamic_cast<cPacket *>(msg);

                socketEvent = dynamic_cast<cMessage *>(pkt);

                if (strcmp(module->getNedTypeName(), "SocketAgentAppTcp") == 0) {
                    SocketAgentAppTcp *app = dynamic_cast<SocketAgentAppTcp *>(module);
                    app->putMessage(socketEvent, adjustedMosaikSimTime);
                    insertedEvent = true;
                } else if (strcmp(module->getNedTypeName(), "SocketAgentApp") == 0) {
                    socketEvent->setArrival(module->getId(), -1, adjustedMosaikSimTime);
                    getSimulation()->getFES()->insert(socketEvent);
                    insertedEvent = true;
                }

                cModule *moduleObserver = nullptr;
                for (int i = 0; i < numModules; i++) {
                    std::string module = std::string(modules[i]->getName());
                    if (module.compare("observerModule") == 0) {
                        moduleObserver = modules[i];
                    }
                }
                MosaikObserverModule *mod = dynamic_cast<MosaikObserverModule *>(observerModule);
                mod->cancelMaxAdvanceEvent();
                mod->max_adv_event->setCtrlType(ControlType::MaxAdvance);
                double maxAdvance_d = maxAdvance / 1000.0 * stepSize;
                mod->max_adv_event->setArrival(observerModule->getId(), -1, maxAdvance_d);
                getSimulation()->getFES()->insert(mod->max_adv_event);
                std::cout << "MosaikObserver: MaxAdvanced inserted at " << maxAdvance_d << endl;

            } else {
                throw cRuntimeError("Can't find module in modules array.");
            }

        } else if (type == CosimaMsgGroup::CosimaMsg::CMD) {
            getSimulation()->callFinish();
        } else if (type == CosimaMsgGroup::CosimaMsg::WAITING) {
            std::cout << "waiting received " << endl;
            // Pretend to have an inserted event to not longer wait for messages
            insertedEvent = true;
        }
    }
    memset(recvBuffer, 0, recvBufferSize);
    return (insertedEvent);
}

int MosaikObserver::receiveUntil(int64_t targetTime) {
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking getEnvir()->idle()
    int64_t currentTime;

    if (realtime) {
        currentTime = opp_get_monotonic_clock_usecs();
    } else {
        currentTime = simTime().inUnit(SIMTIME_US);
    }

    while (targetTime - currentTime >= 200000) {
        if (receiveWithTimeout(100000)) {
            return (1);
        }

        if (realtime) {
            // update simtime before calling envir's idle()
            currentTime = opp_get_monotonic_clock_usecs();
            simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
            ASSERT(eventTime >= simTime());
            sim->setSimTime(eventTime);
        }
        if (getEnvir()->idle()) {
            return (-1);
        }
        currentTime = opp_get_monotonic_clock_usecs();
    }

    // difference is now at most 100ms, do it at once
    long remaining = targetTime - currentTime;
    if (remaining > 0)
        if (receiveWithTimeout(remaining)) {
            return (1);
        }

    return (0);
}

cEvent *MosaikObserver::guessNextEvent() {
    return (sim->getFES()->peekFirst());
}

cEvent *MosaikObserver::takeNextEvent() {
    // assert that we've been configured
    if (!module)
        throw cRuntimeError(
                "MosaikObserver: setInterfaceModule() not called: it must be called "
                "from a module's initialize() function");

    // calculate target time
    int64_t targetTime;
    cEvent *event = sim->getFES()->peekFirst();
    // std::cout << "MosaikObserver: take next event" <<  event << endl;
    if (!event) {
        // if there are no events, wait until something comes from outside
        // TBD: obey simtimelimit, cpu-time-limit
        // This way targetTime will always be "as far in the future as possible",
        // considering
        // how integer overflows work in conjunction with comparisons in C++ (in
        // practice...)
        targetTime = INT64_MAX;

    } else {
        // use time of next event
        simtime_t eventSimtime = event->getArrivalTime();
        if (realtime) {
            targetTime = baseTime + eventSimtime.inUnit(SIMTIME_US);
        } else {
            targetTime = eventSimtime.inUnit(SIMTIME_US);
        }
    }

    // if needed, wait until that time arrives
    int64_t currentTime;

    if (realtime) {
        currentTime = opp_get_monotonic_clock_usecs();
    } else {
        currentTime = simTime().inUnit(SIMTIME_US);
    }

    if (targetTime > currentTime) {
        int status = receiveUntil(targetTime);
        if (status == -1) return (nullptr);                   // interrupted by user
        if (status == 1) event = sim->getFES()->peekFirst();  // received something
    } else {
        // we're behind -- customized versions of this class may
        // alert if we're too much behind, whatever that means
    }

    // remove event from FES and return it
    cEvent *tmp = sim->getFES()->removeFirst();
    ASSERT(tmp == event);
    return (event);
}

void MosaikObserver::putBackEvent(cEvent *event) {
    sim->getFES()->putBackFirst(event);
}

void MosaikObserver::sendBytes(cMessage *reply) {
    //if (connSocket == INVALID_SOCKET)
    //    throw cRuntimeError("cSocketRTScheduler: sendBytes(): no connection");

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    CosimaMsgGroup pmsg_group;
    CosimaMsgGroup::CosimaMsg* pmsg = pmsg_group.add_msg();

    if (typeid(*reply) == typeid(TikTokPacket)) {
        TikTokPacket *treply = dynamic_cast<TikTokPacket *>(reply);
        std::string sender = treply->getSender();
        std::string receiver = treply->getReceiver();
        std::string content = treply->getContent();
        double delay = treply->getDelay();
        std::cout << "Mosaik Observer sends message back to mosaik! " << content
                << " with delay: " << delay << endl;

        pmsg->set_sender(sender);
        pmsg->set_receiver(receiver);
        pmsg->set_content(content);
        pmsg->set_delay(delay);
        pmsg->set_type(CosimaMsgGroup::CosimaMsg::INFO);

        std::string msg;
        pmsg_group.SerializeToString(&msg);
        numOfBytes = strlen(msg.c_str());

        send(connSocket, msg.c_str(), numOfBytes, 0);
        std::cout << "receive new messages from mosaik after Delay message" << endl;
        receiveUntil(INT64_MAX);

    } else if (typeid(*reply) == typeid(MosaikCtrlMsg)) {
        pmsg->set_type(CosimaMsgGroup::CosimaMsg::MAX_ADVANCE);
        std::string msg;
        pmsg_group.SerializeToString(&msg);

        numOfBytes = strlen(msg.c_str());
        std::cout << "SimTime: " << simTime() << endl;

        send(connSocket, msg.c_str(), numOfBytes, 0);
        std::cout << "receive new messages from mosaik after MaxAdvance message" << endl;
        receiveUntil(INT64_MAX);

    }



}

