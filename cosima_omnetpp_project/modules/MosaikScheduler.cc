/*
 * MosaikScheduler.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */
#include "../modules/MosaikScheduler.h"

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <ctime>
#include <iostream>
#include <typeinfo>
#include <chrono>
#include <thread>
#include <fstream>

#include "../modules/MosaikScenarioManager.h"
#include "../modules/MosaikSchedulerModule.h"
#include "../messages/message.pb.h"

#include "inet/common/packet/Packet.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"
#include "../messages/MosaikApplicationChunk_m.h"
#include "../messages/message.pb.h"
#include "../messages/MosaikCtrlEvent_m.h"
#include "../messages/ControlType_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"


using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

Register_Class(MosaikScheduler);

using namespace omnetpp;

// message group to send messages to mosaik
CosimaMsgGroup pmsg_group;

// counter for messages
auto maxAdvanceCounter = 0U;
auto errorCounter = 0U;
auto disconnectCounter = 0U;
auto reconnectCounter = 0U;
auto messageCounter = 0U;

// mosaik parameter
auto stepSize = 1.0;
auto until = 100.0;
auto untilReached = false; // indicates whether simulation end is reached in mosaik
auto initialMessageReceived = false;

auto receiveUntilLock = false;

bool containsOnlyValidFields(std::string const &str) {
    auto it = std::find_if(str.begin(), str.end(), [](char const &c) {
        return (!(std::isalpha(c) or std::isspace(c) or std::ispunct(c)));
    });
    return it == str.end();
}

MosaikScheduler::MosaikScheduler() : cScheduler() {
    // variables for socket connection to mosaik
    listenerSocket = INVALID_SOCKET;
    connSocket = INVALID_SOCKET;
    recvBufferSize = 1000000;
    numModules = 0;
    numOfBytes = 0;
    int numRecvBytes = 0;
    numBytesPtr = &numRecvBytes;

    // message for scheduling socket events from mosaik
    socketEvent = nullptr;

    baseTime = 0;

}

MosaikScheduler::~MosaikScheduler() = default;

std::string MosaikScheduler::info() const {
    return ("scheduler with socket option for mosaik integration");
}

void MosaikScheduler::startRun() {
    if (initsocketlibonce() != 0)
        throw cRuntimeError("MosaikScheduler: Cannot initialize socket library");
    setupListener();
}

void MosaikScheduler::setupListener() {
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket == INVALID_SOCKET)
        throw cRuntimeError("MosaikScheduler: cannot create socket");

    int enable = 1;
    if (setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR,
            (const char *)&enable, sizeof(int)) < 0)
        throw cRuntimeError("MosaikScheduler: cannot set socket option");

    sockaddr_in sinInterface;
    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = INADDR_ANY;
    sinInterface.sin_port = htons(PORT);
    if (bind(listenerSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in)) ==
            SOCKET_ERROR)
        throw cRuntimeError("MosaikScheduler: socket bind() failed");

    listen(listenerSocket, SOMAXCONN);
}

void MosaikScheduler::endRun() {
}

void MosaikScheduler::executionResumed() {
    cScheduler::executionResumed();
}

void MosaikScheduler::printCurrentTime() {
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int usec = curTime.tv_usec;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%H:%M:%S", timeinfo);

    char currentTime[84] = "";
    sprintf(currentTime, "%s:%d", buffer, usec);
    std::cout << currentTime << " ";
}

void MosaikScheduler::log(std::string info) {
    std::cout << "OMNeT++: ";
    printCurrentTime();
    std::cout << info << endl;
}

void MosaikScheduler::setInterfaceModule(cModule *mod, bool isMosaikSchedulerModule) {
    if (mod == nullptr) {
        throw cRuntimeError(
                "MosaikScheduler: setInterfaceModule() not called with a module object");
    }
    std::string module_name = std::string(mod->getName());
    if (isMosaikSchedulerModule) {
        schedulerModule = mod;
    } else {
        // possible to add several modules to array
        addModule(mod);
    }
}

std::list<omnetpp::cModule *> MosaikScheduler::getModuleList () {
    return modules;
}

void MosaikScheduler::setScenarioManager(cModule *manager) {
    log("MosaikScheduler: registered ScenarioManager.");
    scenarioManager = manager;
}

void MosaikScheduler::addModule(cModule *mod) {
    modules.push_back(mod);
    numModules = numModules + 1;
}

int MosaikScheduler::getPortForModule(std::string module_name) {
    if (module_name == "") {
        return -1;
    }
    cModule *submod = getReceiverModule(module_name);
    if (submod != nullptr and submod->hasPar("localPort")) {
        return (submod->par("localPort"));
    }
    return -1;
}

std::string MosaikScheduler::getModuleNameFromPort(int port) {
    for (auto const& module : modules) {
        if(module->par("localPort").intValue() == port) {
            return module->getParentModule()->getName();
        }
    }
    return "";
}

cModule *MosaikScheduler::getReceiverModule(std::string module_name) {
    // get corresponding module to server name
    cModule *matchingModule = nullptr;
    if (numModules == 0) {
        return nullptr;
    }
    for (auto const& module : modules) {
        std::string moduleNameCheck = std::string(module->getParentModule()->getName());
        if (moduleNameCheck.compare(module_name) == 0) {
            matchingModule = module;
        }
    }
    return (matchingModule);
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
                    EV << "MosaikScheduler: shutdown failed, socket already closed." << "\n";
                    connSocket = INVALID_SOCKET;
                } else {
                    closesocket(connSocket);
                    connSocket = INVALID_SOCKET;
                }
            } else {
                // schedule socketEvent for the interface module
                (*numBytesPtr) += nBytes;
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

    MosaikSchedulerModule *schedulerModuleObject = dynamic_cast<MosaikSchedulerModule *>(schedulerModule);
    MosaikScenarioManager *scenarioManagerObject = dynamic_cast<MosaikScenarioManager *>(scenarioManager);

    auto disconnectMsgCounter = 0U;
    MosaikCtrlEvent *disconnectEvent = new MosaikCtrlEvent();
    disconnectEvent->setModuleNamesArraySize(pmsg_group.infrastructure_messages().size());
    std::vector<std::string> disconnectModules;
    disconnectEvent->setCtrlType(Disconnect);

    auto connectMsgCounter = 0U;
    MosaikCtrlEvent *reconnectEvent = new MosaikCtrlEvent();
    reconnectEvent->setModuleNamesArraySize(pmsg_group.infrastructure_messages().size());
    std::vector<std::string> reconnectModules;
    reconnectEvent->setCtrlType(Reconnect);

    auto adjustedMosaikSimTime = 0.0;


    // iterate over protobuf messages in message group
    // get initial message
    for(auto i=0; i < pmsg_group.initial_messages().size(); i++) {
        InitialMessage initialMessage = pmsg_group.initial_messages(i);
        initialMessageReceived = true;
        // calculate from milliseconds to seconds
        until = initialMessage.until()*1.0/1000;
        // cancel until event
        schedulerModuleObject->cancelUntilEvent();
        // schedule until event
        schedulerModuleObject->untilEvent->setArrival(schedulerModule->getId(), -1, until);
        getSimulation()->getFES()->insert(schedulerModuleObject->untilEvent);
        log("MosaikScheduler: until event inserted for time " + std::to_string(until) + " at simtime " + simTime().str());

        stepSize = initialMessage.step_size();
    }

    // get info messages
    for(auto i=0; i < pmsg_group.info_messages().size(); i++) {
        InfoMessage infoMessage = pmsg_group.info_messages().Get(i);
        // get information from message
        auto msgId = infoMessage.msg_id();
        // calculate from milliseconds to seconds
        auto maxAdvance = (infoMessage.max_advance() * 1.0)/1000;
        auto sender = infoMessage.sender();
        auto receiver = infoMessage.receiver();
        auto size = infoMessage.size();
        auto content = infoMessage.content();
        auto msgCreationTime = infoMessage.creation_time();

        // increase messageCounter for snapshot
        messageCounter += 1;

        // look for receiver module in registered modules
        auto module = getReceiverModule(sender);

        // schedule message
        if (module) {
            // create message with parameters from mosaik
            MosaikSchedulerMessage *msg = new MosaikSchedulerMessage();
            msg->setSender(sender.c_str());
            msg->setReceiver(receiver.c_str());
            msg->setSize(size);
            msg->setContent(content.c_str());
            msg->setMsgId(msgId.c_str());
            msg->setCreationTime(msgCreationTime);

            // cast message
            cPacket *pkt = dynamic_cast<cPacket *>(msg);
            socketEvent = dynamic_cast<cMessage *>(pkt);


            adjustedMosaikSimTime = (infoMessage.sim_time() * 1.0) / 1000;

            log("Mosaik Scheduler received message with sender: " + sender + " for simTime " + std::to_string(adjustedMosaikSimTime) + " and id " + msgId );

            // schedule message as an event at the sender module
            socketEvent->setArrival(module->getId(), -1, adjustedMosaikSimTime);
            getSimulation()->getFES()->insert(socketEvent);
            insertedEvent = true;
            log("MosaikScheduler: message with content '" + content + " and sender " + sender + " and id " + msgId + " inserted.");
            // schedule max advance event
            schedulerModuleObject->cancelMaxAdvanceEvent();
            schedulerModuleObject->maxAdvEvent->setCtrlType(ControlType::MaxAdvance);
            if (maxAdvance >= simTime().dbl()) {
                schedulerModuleObject->maxAdvEvent->setArrival(schedulerModule->getId(), -1, maxAdvance);
                getSimulation()->getFES()->insert(schedulerModuleObject->maxAdvEvent);
                log("MosaikScheduler: max advanced event inserted for time " + std::to_string(maxAdvance) + " at simtime " + simTime().str());
            } else {
                log("max advance " + std::to_string(maxAdvance) + " at time " + simTime().str() + " not inserted");
            }

        } else {
            throw cRuntimeError("Can't find module in modules array.");
        }
    }

    // get synchronization messages
    for(auto i=0; i < pmsg_group.synchronisation_messages().size(); i++) {
        SynchronisationMessage syncMessage = pmsg_group.synchronisation_messages().Get(i);
        if (syncMessage.msg_type() == SynchronisationMessage_MsgType_WAITING) {
            std::string msgId = syncMessage.msg_id();
            log("MosaikScheduler: received waiting message with Id " + msgId);
            // Pretend to have an inserted event to not longer wait for messages
            // Substract 1 because mosaik time is sim time + 1 since agents output time is always one step further.
            // TODO: check if we have to subtract 1
            auto waitingMsgTime = ((syncMessage.sim_time()) * 1.0) / 1000;
            auto maxAdvance = ((syncMessage.max_advance()) * 1.0) / 1000;
            log("waiting msg time " + std::to_string(waitingMsgTime) + " sim time " + simTime().str() + " max advance " + std::to_string(maxAdvance));

            // schedule max advance event
            schedulerModuleObject->cancelMaxAdvanceEvent();
            schedulerModuleObject->maxAdvEvent->setCtrlType(ControlType::MaxAdvance);
            if (maxAdvance >= simTime().dbl()) {
                schedulerModuleObject->maxAdvEvent->setArrival(schedulerModule->getId(), -1, maxAdvance);
                getSimulation()->getFES()->insert(schedulerModuleObject->maxAdvEvent);
                log("MosaikScheduler: max advanced event inserted for time " + std::to_string(maxAdvance) + " at simtime " + simTime().str());
                if(waitingMsgTime >= simTime().dbl() or untilReached) {
                    log("MosaikScheduler: continue simulation.");
                    insertedEvent = true;
                } else {
                    log("MosaikScheduler: continue waiting at time " + simTime().str() );
                    informMosaikAboutWaiting();
                }
            } else {
                log("max advance " + std::to_string(maxAdvance) + " at time " + simTime().str() + " not inserted");
                log("MosaikScheduler: continue waiting at time " + simTime().str() );
                informMosaikAboutWaiting();
            }
        }

    }

    // get infrastructure messages
    for(auto i=0; i < pmsg_group.infrastructure_messages().size(); i++) {
        InfrastructureMessage infrastructureMessage = pmsg_group.infrastructure_messages().Get(i);
        adjustedMosaikSimTime = (infrastructureMessage.sim_time() * 1.0) / 1000;
        if (infrastructureMessage.change_module() != "") {
            if (infrastructureMessage.msg_type() == InfrastructureMessage_MsgType_DISCONNECT) {
                auto msgId = infrastructureMessage.msg_id();
                auto adjustedMosaikSimTime = (infrastructureMessage.sim_time() * 1.0) / 1000;
                log("MosaikScheduler: disconnect event inserted for simtime " + std::to_string(adjustedMosaikSimTime) + " for " + infrastructureMessage.change_module() + ".");
                disconnectModules.push_back((infrastructureMessage.change_module()).c_str());

            } else if (infrastructureMessage.msg_type() == InfrastructureMessage_MsgType_RECONNECT) {
                auto msgId = infrastructureMessage.msg_id();
                log("MosaikScheduler: reconnect event inserted for simtime " + std::to_string(adjustedMosaikSimTime) + " for " + infrastructureMessage.change_module() + ".");
                reconnectModules.push_back((infrastructureMessage.change_module()).c_str());
            }
        } else {
            log("Attention: no module name for infrastructure change set!");
        }

    }

    if(disconnectModules.size() > 0) {
        disconnectEvent->setModuleNamesArraySize(disconnectModules.size());
        auto counter = 0;
        for(const auto& module: disconnectModules) {
            disconnectEvent->setModuleNames(counter, module.c_str());
            counter++;
        }
        disconnectEvent->setArrival(scenarioManagerObject->getId(), -1, adjustedMosaikSimTime);
        getSimulation()->getFES()->insert(disconnectEvent);
        insertedEvent = true;
        disconnectModules.clear();
    } else {
        delete disconnectEvent;
    }

    if(reconnectModules.size() > 0) {
        reconnectEvent->setModuleNamesArraySize(reconnectModules.size());
        auto counter = 0;
        for(const auto& module: reconnectModules) {
            reconnectEvent->setModuleNames(counter, module.c_str());
            counter++;
        }
        reconnectEvent->setArrival(scenarioManagerObject->getId(), -1, adjustedMosaikSimTime);
        getSimulation()->getFES()->insert(reconnectEvent);
        insertedEvent = true;
        reconnectModules.clear();
    } else {
        delete reconnectEvent;
    }

    memset(recvBuffer, 0, recvBufferSize);
    return (insertedEvent);
}

void MosaikScheduler::informMosaikAboutWaiting() {
    MosaikCtrlEvent *waitingEvent = new MosaikCtrlEvent();
    waitingEvent->setCtrlType(ContinueWaiting);
    sendToMosaik(waitingEvent);
}

int MosaikScheduler::receiveUntil(int64_t targetTime) {
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking getEnvir()->idle()
    int64_t currentTime = simTime().inUnit(SIMTIME_US);


    while (targetTime - currentTime >= 200000) {
        if (receiveWithTimeout(100000)) {
            return (1);
        }

        if (getEnvir()->idle()) {
            return (-1);
        }
        currentTime = opp_get_monotonic_clock_usecs();
    }

    // difference is now at most 100ms, do it at once
    long remaining = targetTime - currentTime;
    if (remaining > 0) {
        if (receiveWithTimeout(remaining)) {
            return (1);
        }
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
        if (untilReached) {
            log("MosaikScheduler: reached until time in mosaik." );
            endSimulation();
        }
        targetTime = INT64_MAX;

    } else if (initialMessageReceived){
        try {
            // type 3 means: destination unreachable
            // code 0 means: destination network unreachable
            if (strcmp(event->getName(),"ICMP-error-#1-type3-code0") == 0) {
                log("error msg received");
                if (event->isMessage()) {
                    omnetpp::cMessage *msg = dynamic_cast<omnetpp::cMessage *>(event);
                    if (typeid(*msg) == typeid(inet::Packet)) {
                        inet::Packet *packet;
                        packet = dynamic_cast<inet::Packet *>(msg);
                        inet::b offset = inet::b(0);  // start from the beginning
                        bool foundHeader = false;
                        while (auto chunk = packet->peekAt(offset)->dupShared()) {  // for each chunk
                            if (foundHeader) {
                                break;
                            }
                            auto length = chunk->getChunkLength();
                            if (chunk->getClassName() == std::string("inet::Ipv4Header")) {
                                foundHeader = true;
                                int id = packet->peekAt<inet::Ipv4Header>(offset, length)->getIdentification();
                                inet::L3AddressResolver addressResolver;
                                cModule *sourceModule = addressResolver.findHostWithAddress(packet->peekAt<inet::Ipv4Header>(offset, length)->getSourceAddress());
                                if (sourceModule != nullptr) {
                                    log("ICMP error: id: " + std::to_string(id) + " address: " + packet->peekAt<inet::Ipv4Header>(offset, length)->getSourceAddress().str() + " module: " + sourceModule->getName() );
                                    if (getReceiverModule(sourceModule->getName()) == nullptr) {
                                        break;
                                    }
                                    log("MosaikScheduler: There was an error. Send notification message to mosaik " );
                                    MosaikSchedulerMessage *notification_message = new MosaikSchedulerMessage();
                                    notification_message->setTransmission_error(true);
                                    notification_message->setConnection_change_successful(true);
                                    notification_message->setSender(sourceModule->getName());
                                    sendToMosaik(notification_message);
                                }

                            } else {
                                offset += chunk->getChunkLength();
                            }
                        }
                    }
                }

            };
        } catch (...) {
            log("MosaikScheduler: couldn't find host");

        }

        // use time of next event
        simtime_t eventSimtime = event->getArrivalTime();
        if(eventSimtime < lastEventTime) {
            log("ATTENTION! MosaikScheduler: try to execute event for simtime " + eventSimtime.str() + ", but last event was at time " + lastEventTime.str() );
            throw cRuntimeError("Wrong simulation order.");
        }
        lastEventTime = eventSimtime;
        targetTime = eventSimtime.inUnit(SIMTIME_US);
    } else {
        log("Waiting for connection");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // if needed, wait until that time arrives
    int64_t currentTime;

    currentTime = simTime().inUnit(SIMTIME_US);


    if (targetTime > currentTime) {
        int status = receiveUntil(targetTime);
        if (status == -1) return (nullptr);                   // interrupted by user
        if (status == 1) event = sim->getFES()->peekFirst();  // received something
    } else {
        // we're behind -- customized versions of this class may
        // alert if we're too much behind, whatever that means
    }
    if (initialMessageReceived) {
        // remove event from FES and return it
        cEvent *tmp = sim->getFES()->removeFirst();
        ASSERT(tmp == event);
        return (event);
    }
    return nullptr;
}

void MosaikScheduler::putBackEvent(cEvent *event) {
    sim->getFES()->putBackFirst(event);
}

void MosaikScheduler::writeSimulationSnapshot() {
    // write snapshot to file
    eventnumber_t eventNumber = getSimulation()->getEventNumber();
    std::ofstream snapshotFile;
    snapshotFile.open ("../tests/integration_tests/snapshot.txt");
    snapshotFile << eventNumber << " " << messageCounter << " " << errorCounter << " " << maxAdvanceCounter
            << " " << disconnectCounter << " " << reconnectCounter << " " << lastEventTime.str();
    snapshotFile.close();
}


void MosaikScheduler::endSimulation() {
    writeSimulationSnapshot();
    log("MosaikScheduler: end simulation" );
    getSimulation()->callFinish();
    // close socket connection to mosaik
    closesocket(connSocket);
    getSimulation()->getActiveEnvir()->alert("End of simulation");

}

void MosaikScheduler::sendMsgGroupToMosaik() {
    if (getNumberOfMessagesInPMSGGroup() == 0) {
        log("MosaikScheduler: Don't send any message group back, because it contains no messages.");
        // initialize new message group
        pmsg_group.Clear();
        return;
    }
    try {
        // serialize message
        std::string msg;
        pmsg_group.SerializeToString(&msg);
        numOfBytes = strlen(msg.c_str());

        // send message over TCP socket
        send(connSocket, msg.c_str(), numOfBytes, 0);

        log("Mosaik Scheduler: send message group back to mosaik at simttime: " + simTime().str() );
        log("MosaikScheduler: number of messages: " + std::to_string(getNumberOfMessagesInPMSGGroup()));

        // initialize new message group
        pmsg_group.Clear();

        if (not untilReached and (until-simTime().dbl() > 0.001)) {
            log("MosaikScheduler: receive new messages from mosaik." + simTime().str() );
            if (receiveUntilLock) {
                receiveUntil(1000);
            }
            else {
                receiveUntilLock = true;
                receiveUntil(INT64_MAX);
                receiveUntilLock = false;
            }

        }
    } catch(...) {
        log("exception");
    }

}

int MosaikScheduler::getNumberOfMessagesInPMSGGroup() {
    return pmsg_group.info_messages().size() + pmsg_group.synchronisation_messages().size() + pmsg_group.infrastructure_messages().size();
}

void MosaikScheduler::setUntilReached(bool untilReachedValue) {
    untilReached = untilReachedValue;
}

bool MosaikScheduler::getUntilReached() {
    return untilReached;
}

void MosaikScheduler::setInitialMessageReceived(bool value) {
    initialMessageReceived = value;
}


void MosaikScheduler::sendToMosaik(cMessage *reply) {
    if (reply == nullptr) {
        throw cRuntimeError("MosaikScheduler: Message is nullptr");
    }
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // round up current simTime
    auto currentStep = 0U;
    currentStep = ceil(simTime().dbl()*1000);
    auto currentStep_d = (currentStep * 1.0) / 1000;
    MosaikCtrlEvent *replyEvent;

    auto scheduleMessage = true;
    if (typeid(*reply) == typeid(MosaikCtrlEvent)) {
        replyEvent = dynamic_cast<MosaikCtrlEvent *>(reply);
        if(replyEvent->getCtrlType() == ContinueWaiting) {
            scheduleMessage = false;
        }
    }
    if (scheduleMessage) {
        if(pmsg_group.current_mosaik_step() == currentStep) {
            log("MosaikScheduler: request to send message for the same step " + std::to_string(currentStep) + " back to mosaik." );
        } else if (getNumberOfMessagesInPMSGGroup() != 0) {
            currentStep = pmsg_group.current_mosaik_step();
            log("MosaikScheduler: send message back for step " + std::to_string(currentStep));
        } else {
            pmsg_group.set_current_mosaik_step(currentStep);

            // get scheduler module
            MosaikSchedulerModule *schedulerModuleObject = dynamic_cast<MosaikSchedulerModule *>(schedulerModule);
            MosaikCtrlEvent* endOfStepEvent = new MosaikCtrlEvent("send message to mosaik");
            endOfStepEvent->setCtrlType(ControlType::EndOfStep);
            endOfStepEvent->setArrival(schedulerModule->getId(), -1, currentStep_d+0.0000001);
            getSimulation()->getFES()->insert(endOfStepEvent);
            log("MosaikScheduler: insert event 'send message to mosaik' for time " + std::to_string(currentStep_d+0.0000001) );

        }
    }

    if (typeid(*reply) == typeid(MosaikSchedulerMessage)) {
        MosaikSchedulerMessage *replyMsg = dynamic_cast<MosaikSchedulerMessage *>(reply);

        // get message content
        auto sender = replyMsg->getSender();
        auto receiver = replyMsg->getReceiver();
        auto content = replyMsg->getContent();
        auto msgId = replyMsg->getMsgId();
        auto creationTime = replyMsg->getCreationTime();
        auto messageValid = true;

        std::stringstream logContent;

        //if (not containsOnlyValidFields(content)) {
          //  content = "Message not valid";
          //  messageValid = false;
       // }

        if(replyMsg->getTransmission_error()) {
            SynchronisationMessage* syncMessage = pmsg_group.add_synchronisation_messages();

            syncMessage->set_msg_type(SynchronisationMessage_MsgType_TRANSMISSION_ERROR);
            syncMessage->set_timeout(replyMsg->getTimeout());
            syncMessage->set_timeout_msg_id(replyMsg->getMsgId());

            std::stringstream errorCounterStr;
            errorCounterStr << errorCounter;
            auto mId = "ErrorMessage_" + errorCounterStr.str();
            syncMessage->set_msg_id(mId);
            errorCounter += 1;

            logContent << "MosaikScheduler: send notification for transmission error for message with sender " << sender <<
                    " and receiver " << receiver << " and messageId " << replyMsg->getMsgId() << " as message " <<  mId <<
                    " back to mosaik. ";
            log(logContent.str());
            logContent.str("");
        } else if (replyMsg->getDisconnected_event()) {
            InfrastructureMessage* infrastructureMessage = pmsg_group.add_infrastructure_messages();

            infrastructureMessage->set_msg_type(InfrastructureMessage_MsgType_DISCONNECT);

            std::stringstream disconnectCounterStr;
            disconnectCounterStr << disconnectCounter;
            auto mId = "DisconnectNotificationMessage_" + disconnectCounterStr.str();
            infrastructureMessage->set_msg_id(mId);
            infrastructureMessage->set_sim_time(currentStep);
            infrastructureMessage->set_connection_change_successful(replyMsg->getConnection_change_successful());
            disconnectCounter += 1;

            logContent << "MosaikScheduler: send notification for disconnect for client " << sender <<
                    " and id " << mId << " back to mosaik. ";
            log(logContent.str());
            logContent.str("");
        } else if (replyMsg->getReconnected_event()) {
            InfrastructureMessage* infrastructureMessage = pmsg_group.add_infrastructure_messages();

            infrastructureMessage->set_msg_type(InfrastructureMessage_MsgType_RECONNECT);
            std::stringstream reconnectCounterStr;
            reconnectCounterStr << reconnectCounter;
            auto mId = "ReconnectNotificationMessage_" + reconnectCounterStr.str();
            infrastructureMessage->set_msg_id(mId);
            infrastructureMessage->set_sim_time(currentStep);
            infrastructureMessage->set_connection_change_successful(replyMsg->getConnection_change_successful());
            reconnectCounter += 1;

            logContent << "MosaikScheduler: send notification for reconnect for client " << sender <<
                    " and id " << mId << " back to mosaik. ";
            log(logContent.str());
            logContent.str("");
        }
        else {
            InfoMessage* infoMessage = pmsg_group.add_info_messages();

            infoMessage->set_sender(sender);
            infoMessage->set_receiver(receiver);
            infoMessage->set_content(content);
            infoMessage->set_sim_time(currentStep);
            infoMessage->set_msg_id(msgId);
            infoMessage->set_creation_time(creationTime);
            infoMessage->set_is_valid(messageValid);
            logContent << "MosaikScheduler: send message with content '" << content << "' and sender " << sender << " and current step " << currentStep << " back to mosaik.";
            log(logContent.str());
        }

    } else if (typeid(*reply) == typeid(MosaikCtrlEvent)) {
        if (replyEvent->getCtrlType() == ContinueWaiting) {
            SynchronisationMessage* syncMessage = pmsg_group.add_synchronisation_messages();

            syncMessage->set_msg_type(SynchronisationMessage_MsgType_WAITING);
            syncMessage->set_sim_time(currentStep);


            log("MosaikScheduler: send message with waiting info back to mosaik");
            sendMsgGroupToMosaik();

        } else if (replyEvent->getCtrlType() == MaxAdvance) {
            SynchronisationMessage* syncMessage = pmsg_group.add_synchronisation_messages();

            syncMessage->set_msg_type(SynchronisationMessage_MsgType_MAX_ADVANCE);

            std::stringstream maxAdvanceCounterStr;
            maxAdvanceCounterStr << maxAdvanceCounter;
            auto mId = "MaxAdvanceMessage_" + maxAdvanceCounterStr.str();
            syncMessage->set_msg_id(mId);
            syncMessage->set_sim_time(currentStep);
            maxAdvanceCounter += 1;
            log("MosaikScheduler: send message with max advance info back to mosaik with Message id " + mId );

        }

    }

}

