/*
 * MosaikScheduler.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */
#include "MosaikScheduler.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>
#include "messages/MosaikApplicationChunk_m.h"
#include "messages/message.pb.h"
#include "messages/MosaikCtrlEvent_m.h"
#include "messages/ControlType_m.h"
#include "MosaikSchedulerModule.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

Register_Class(MosaikScheduler);

using namespace omnetpp;

MosaikScheduler::MosaikScheduler() : cScheduler() {
    // variables for socket connection to mosaik
    listenerSocket = INVALID_SOCKET;
    connSocket = INVALID_SOCKET;
    recvBufferSize = 4000;
    numModules = 0;
    numOfBytes = 0;
    int numRecvBytes = 0;
    numBytesPtr = &numRecvBytes;

    // message for scheduling socket events from mosaik
    socketEvent = nullptr;

    // properties for real-time simulation
    realtime = false;
    baseTime = 0;

    // TCP port to mosaik
    port = 4242;

}

MosaikScheduler::~MosaikScheduler() = default;

std::string MosaikScheduler::info() const {
    return ("scheduler with socket option for mosaik integration");
}

void MosaikScheduler::startRun() {
    if (initsocketlibonce() != 0)
        throw cRuntimeError("MosaikScheduler: Cannot initialize socket library");

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

void MosaikScheduler::setupListener() {
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket == INVALID_SOCKET)
        throw cRuntimeError("MosaikScheduler: cannot create socket");

    // Setting this option makes it possible to kill the sample and restart
    // it right away without having to change the port it is listening on.
    // Not using SO_REUSEADDR as per: https://stackoverflow.com/a/34812994
    // Correction: There is no SO_REUSEPORT on Windows, so SO_REUSEADDR it is.
    int enable = 1;
    if (setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR,
            (const char *)&enable, sizeof(int)) < 0)
        throw cRuntimeError("MosaikScheduler: cannot set socket option");

    sockaddr_in sinInterface;
    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = INADDR_ANY;
    sinInterface.sin_port = htons(port);
    if (bind(listenerSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in)) ==
            SOCKET_ERROR)
        throw cRuntimeError("MosaikScheduler: socket bind() failed");

    listen(listenerSocket, SOMAXCONN);
}

void MosaikScheduler::endRun() {
}

void MosaikScheduler::executionResumed() {
    if (realtime) {
        baseTime = opp_get_monotonic_clock_usecs();
        baseTime = baseTime - simTime().inUnit(SIMTIME_US);
    } else {
        cScheduler::executionResumed();
    }
}

void MosaikScheduler::setInterfaceModule(cModule *mod, bool isMosaikSchedulerModule) {
    std::string module_name = std::string(mod->getName());
    if (isMosaikSchedulerModule) {
        schedulerModule = mod;
    } else {
        if (!mod) {
            throw cRuntimeError(
                    "MosaikScheduler: setInterfaceModule() not called with a module object");
        }
        // possible to add several modules to array
        addModule(mod);
    }
}

void MosaikScheduler::addModule(cModule *mod) {
    modules[numModules] = mod;
    numModules = numModules + 1;
}

int MosaikScheduler::getPortForModule(std::string module_name) {
    cModule *submod = getReceiverModule(module_name);
    return (submod->par("localPort"));
}

std::string MosaikScheduler::getModuleNameFromPort(int port) {
    for (int i = 0; i < numModules; i++) {
        if(modules[i]->par("localPort").intValue() == port) {
            return modules[i]->getParentModule()->getName();
        }
    }
    return "";
}

cModule *MosaikScheduler::getReceiverModule(std::string module_name) {
    // get corresponding module to server name
    cModule *matching_module = nullptr;
    for (int i = 0; i < numModules; i++) {
        std::string module = std::string(modules[i]->getParentModule()->getName());
        if (module.compare(module_name) == 0) {
            matching_module = modules[i];
        }
    }
    return (matching_module);
}

bool MosaikScheduler::receiveWithTimeout(long usec) {
    // prepare sets for select()
    fd_set readFDs = {};
    fd_set writeFDs = {};
    fd_set exceptFDs = {};
    FD_ZERO(&readFDs);
    FD_ZERO(&writeFDs);
    FD_ZERO(&exceptFDs);

    *numBytesPtr = 0;

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
                        "MosaikScheduler: interface module's recvBuffer is full");
            int nBytes = recv(connSocket, bufPtr, bufLeft, 0);
            if (nBytes == SOCKET_ERROR) {
                EV << "MosaikScheduler: socket error " << sock_errno() << "\n";
                closesocket(connSocket);
                connSocket = INVALID_SOCKET;
            } else if (nBytes == 0) {
                EV << "MosaikScheduler: socket closed by the client\n";
                if (shutdown(connSocket, SHUT_WR) == SOCKET_ERROR) {
                    EV << "MosaikScheduler: shutdown failed, socket already closed."
                            << "\n";
                    connSocket = INVALID_SOCKET;
                } else {
                    closesocket(connSocket);
                    connSocket = INVALID_SOCKET;
                }
            } else {
                // schedule socketEvent for the interface module
                (*numBytesPtr) += nBytes;
                if (realtime) {
                    int64_t currentTime = opp_get_monotonic_clock_usecs();
                    simtime_t eventTime(currentTime - baseTime, SIMTIME_US);
                    ASSERT(eventTime >= simTime());
                }
                return (handleMsgFromMosaik());
            }
        } else if (FD_ISSET(listenerSocket, &readFDs)) {
            // accept connection, and store FD in connSocket
            sockaddr_in sinRemote;
            int addrSize = sizeof(sinRemote);
            connSocket = accept(listenerSocket, (sockaddr *)&sinRemote,
                    (socklen_t *)&addrSize);
            if (connSocket == INVALID_SOCKET)
                throw cRuntimeError("MosaikScheduler: accept() failed");
            EV << "MosaikScheduler: connected!\n";
        }
    }
    return (false);
}

bool MosaikScheduler::handleMsgFromMosaik() {
    bool insertedEvent = false;

    // get message object from socket data
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    CosimaMsgGroup pmsg_group;
    pmsg_group.ParseFromString(recvBuffer);

    // iterate over protobuf messages in message group
    for(int i=0; i<pmsg_group.msg().size(); i++) {
        int type = pmsg_group.msg().Get(i).type();

        if (type == CosimaMsgGroup::CosimaMsg::INFO) {
            // get information from message
            std::string sender = pmsg_group.msg().Get(i).sender();
            std::string receiver = pmsg_group.msg().Get(i).receiver();
            std::string content = pmsg_group.msg().Get(i).content();
            std::string msgId = pmsg_group.msg().Get(i).msgid();
            int size = pmsg_group.msg().Get(i).size();
            int maxAdvance_i = pmsg_group.msg().Get(i).max_advance();
            // calculate from milliseconds to seconds
            double maxAdvance = (maxAdvance_i * 1.0)/1000;
            stepSize = pmsg_group.msg().Get(i).stepsize() * 1.0;

            // calculate simulation time to seconds
            double adjustedMosaikSimTime = (pmsg_group.msg().Get(i).simtime() * 1.0) / 1000;

            EV << "Mosaik Scheduler received message with sender: " << sender << " for simTime " << adjustedMosaikSimTime << " and id " << msgId << endl;

            // look for receiver module in registered modules
            cModule *module = getReceiverModule(sender);

            // schedule message
            if (module) {
                // create message with parameters from mosaik
                MosaikSchedulerMessage *msg = new MosaikSchedulerMessage();
                msg->setSender(sender.c_str());
                msg->setReceiver(receiver.c_str());
                msg->setSize(size);
                msg->setContent(content.c_str());
                msg->setMsgId(msgId.c_str());

                // cast message
                cPacket *pkt = dynamic_cast<cPacket *>(msg);
                socketEvent = dynamic_cast<cMessage *>(pkt);

                // schedule message as an event at the sender module
                socketEvent->setArrival(module->getId(), -1, adjustedMosaikSimTime);
                getSimulation()->getFES()->insert(socketEvent);
                insertedEvent = true;
                std::cout << "MosaikScheduler: message with content '" << content << " and sender " << sender << " and id " << msgId << " inserted." << endl;

                // schedule max advance event
                MosaikSchedulerModule *schedulerModuleObject = dynamic_cast<MosaikSchedulerModule *>(schedulerModule);
                schedulerModuleObject->cancelMaxAdvanceEvent();
                schedulerModuleObject->max_adv_event->setCtrlType(ControlType::MaxAdvance);
                if (maxAdvance >= simTime().dbl()) {
                    schedulerModuleObject->max_adv_event->setArrival(schedulerModule->getId(), -1, maxAdvance);
                    getSimulation()->getFES()->insert(schedulerModuleObject->max_adv_event);
                    std::cout << "MosaikScheduler: max advanced event inserted for time " << maxAdvance << " at simtime " << simTime() << endl;
                } else {
                    std::cout << "max advance " << maxAdvance << " at time " << simTime() << " not inserted" << endl;
                }


            } else {
                throw cRuntimeError("Can't find module in modules array.");
            }

        } else if (type == CosimaMsgGroup::CosimaMsg::CMD) {
            getSimulation()->callFinish();
        } else if (type == CosimaMsgGroup::CosimaMsg::WAITING) {
            std::string msgId = pmsg_group.msg().Get(i).msgid();
            std::cout << "MosaikScheduler: received waiting message " << simTime() << " with Id " << msgId << endl;
            // Pretend to have an inserted event to not longer wait for messages
            // Substract 1 because mosaik time is sim time + 1 since agents output time is always one step further.
            double waiting_msg_time = ((pmsg_group.msg().Get(i).simtime()-1) * 1.0) / 1000;
            if(waiting_msg_time >= simTime().dbl()) {
                std::cout << "waiting message time: " << waiting_msg_time << endl;
                std::cout << "simtime: " << simTime() << endl;
                insertedEvent = true;
            } else {
                std::cout << "waiting message time: " << waiting_msg_time << endl;
                std::cout << "simtime: " << simTime() << endl;
                std::cout << "continue waiting at time " << simTime() << endl;
            }
        }
    }
    memset(recvBuffer, 0, recvBufferSize);
    return (insertedEvent);
}

int MosaikScheduler::receiveUntil(int64_t targetTime) {
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

cEvent *MosaikScheduler::guessNextEvent() {
    return (sim->getFES()->peekFirst());
}

cEvent *MosaikScheduler::takeNextEvent() {
    // calculate target time
    int64_t targetTime;
    cEvent *event = sim->getFES()->peekFirst();
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
        if(eventSimtime < lastEventTime) {
            std::cout << "ATTENTION! MosaikScheduler: try to execute event for simtime " << eventSimtime << ", but last event was at time " << lastEventTime << endl;
            throw cRuntimeError("Wrong simulation order.");
        }
        lastEventTime = eventSimtime;
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

void MosaikScheduler::putBackEvent(cEvent *event) {
    sim->getFES()->putBackFirst(event);
}

void MosaikScheduler::sendMsgGroupToMosaik() {
    // serialize message
    std::string msg;
    pmsg_group.SerializeToString(&msg);
    numOfBytes = strlen(msg.c_str());

    // send message over TCP socket
    send(connSocket, msg.c_str(), numOfBytes, 0);

    std::cout << "Mosaik Scheduler: send message group back to mosaik for step: " << pmsg_group.current_mosaik_step() << endl;

    // initialize new message group
    pmsg_group.Clear();
    CosimaMsgGroup *new_pmsg_group = new CosimaMsgGroup();
    pmsg_group = *new_pmsg_group;

    std::cout << "MosaikScheduler: receive new messages from mosaik." << simTime() << endl;
    receiveUntil(INT64_MAX);
}

void MosaikScheduler::sendToMosaik(cMessage *reply) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // round up current simTime
    int current_step = ceil(simTime().dbl()*1000);
    double current_step_d = (current_step * 1.0) / 1000;

    if(pmsg_group.current_mosaik_step() == current_step) {
        std::cout << "MosaikScheduler: request to send message for the same step " << current_step << " back to mosaik." << endl;
    } else {
        pmsg_group.set_current_mosaik_step(current_step);

        // get scheduler module
        MosaikSchedulerModule *schedulerModuleObject = dynamic_cast<MosaikSchedulerModule *>(schedulerModule);
        MosaikCtrlEvent* end_of_step_event = new MosaikCtrlEvent("send message to mosaik");
        end_of_step_event->setCtrlType(ControlType::EndOfStep);
        end_of_step_event->setArrival(schedulerModule->getId(), -1, current_step_d);
        getSimulation()->getFES()->insert(end_of_step_event);
        std::cout << "MosaikScheduler: insert event 'send message to mosaik' for time " << current_step_d << endl;

    }
    CosimaMsgGroup::CosimaMsg* pmsg = pmsg_group.add_msg();

    if (typeid(*reply) == typeid(MosaikSchedulerMessage)) {
        MosaikSchedulerMessage *reply_msg = dynamic_cast<MosaikSchedulerMessage *>(reply);

        // get message content
        std::string sender = reply_msg->getSender();
        std::string receiver = reply_msg->getReceiver();
        std::string content = reply_msg->getContent();
        std::string msgId = reply_msg->getMsgId();
        double delay = reply_msg->getDelay();
        delay = delay/1000;

        pmsg->set_sender(sender);
        pmsg->set_receiver(receiver);
        pmsg->set_content(content);
        pmsg->set_simtime(current_step);
        pmsg->set_msgid(msgId);
        pmsg->set_type(CosimaMsgGroup::CosimaMsg::INFO);

        std::cout << "MosaikScheduler: send message with content '" << content << " and sender " << sender
                << "' and delay " << delay << " and Id " << msgId << " back to mosaik. " << endl;

        if(reply_msg->getTransmission_error()) {
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::TRANSMISSION_ERROR);
            std::stringstream errorCounterStr;
            errorCounterStr << errorCounter;
            std::string m_id = "ErrorMessage_" + errorCounterStr.str();
            pmsg->set_msgid(m_id);
            errorCounter += 1;

            std::cout << "MosaikScheduler: send notification for transmission error for message with sender " << sender <<
                    " and receiver " << receiver << " and id " << m_id << " back to mosaik. " << endl;
        } else {
            double delay = reply_msg->getDelay();
            delay = delay/1000;

            pmsg->set_delay(delay);
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::INFO);
            std::cout << "MosaikScheduler: send message with content '" << content << "' and sender " << sender
                << " and delay " << delay << " back to mosaik. " << endl;
        }

    } else if (typeid(*reply) == typeid(MosaikCtrlEvent)) {
        // message is max advance event

        // set type to protobuf message and serialize message
        pmsg->set_type(CosimaMsgGroup::CosimaMsg::MAX_ADVANCE);
        std::stringstream maxAdvanceCounterStr;
        maxAdvanceCounterStr << maxAdvanceCounter;
        std::string m_id = "MaxAdvanceMessage_" + maxAdvanceCounterStr.str();
        pmsg->set_msgid(m_id);
        maxAdvanceCounter += 1;

        std::cout << "MosaikScheduler: send message with max advance info back to mosaik with Message id " << m_id << endl;

    }

}

