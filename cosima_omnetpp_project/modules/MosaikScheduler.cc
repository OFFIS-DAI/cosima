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
    cModule *matching_module = nullptr;
    if (numModules == 0) {
        return nullptr;
    }
    for (auto const& module : modules) {
        std::string module_name_check = std::string(module->getParentModule()->getName());
        if (module_name_check.compare(module_name) == 0) {
            matching_module = module;
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


    int disconnectMsgCounter = 0;
    MosaikCtrlEvent *disconnect_event = new MosaikCtrlEvent();
    disconnect_event->setModuleNamesArraySize(pmsg_group.msg().size());
    std::vector<std::string> disconnectModules;
    disconnect_event->setCtrlType(Disconnect);

    int connectMsgCounter = 0;
    MosaikCtrlEvent *reconnect_event = new MosaikCtrlEvent();
    reconnect_event->setModuleNamesArraySize(pmsg_group.msg().size());
    std::vector<std::string> reconnectModules;
    reconnect_event->setCtrlType(Reconnect);

    double adjustedMosaikSimTime = 0.0;


    // iterate over protobuf messages in message group
    for(int i=0; i<pmsg_group.msg().size(); i++) {
        //TODO: is it possible to have different simtimes?
        // calculate simulation time to seconds
        adjustedMosaikSimTime = (pmsg_group.msg().Get(0).simtime() * 1.0) / 1000;
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

            int msg_creation_time = pmsg_group.msg().Get(i).creation_time();

            // increase messageCounter for snapshot
            messageCounter += 1;

            log("Mosaik Scheduler received message with sender: " + sender + " for simTime " + std::to_string(adjustedMosaikSimTime) + " and id " + msgId );

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
                msg->setCreationTime(msg_creation_time);

                // cast message
                cPacket *pkt = dynamic_cast<cPacket *>(msg);
                socketEvent = dynamic_cast<cMessage *>(pkt);

                // schedule message as an event at the sender module
                socketEvent->setArrival(module->getId(), -1, adjustedMosaikSimTime);
                getSimulation()->getFES()->insert(socketEvent);
                insertedEvent = true;
                log("MosaikScheduler: message with content '" + content + " and sender " + sender + " and id " + msgId + " inserted.");

                if (!is_time_based) {
                    // schedule max advance event
                    schedulerModuleObject->cancelMaxAdvanceEvent();
                    schedulerModuleObject->max_adv_event->setCtrlType(ControlType::MaxAdvance);
                    if (maxAdvance >= simTime().dbl()) {
                        schedulerModuleObject->max_adv_event->setArrival(schedulerModule->getId(), -1, maxAdvance);
                        getSimulation()->getFES()->insert(schedulerModuleObject->max_adv_event);
                        log("MosaikScheduler: max advanced event inserted for time " + std::to_string(maxAdvance) + " at simtime " + simTime().str());
                    } else {
                        log("max advance " + std::to_string(maxAdvance) + " at time " + simTime().str() + " not inserted");
                    }
                }
            } else {
                throw cRuntimeError("Can't find module in modules array.");
            }

        } else if (type == CosimaMsgGroup::CosimaMsg::CMD) {
            initial_message_received = true;
            int until_i = pmsg_group.msg().Get(i).until();
            // calculate from milliseconds to seconds
            double until = (until_i * 1.0)/1000;
            // cancel until event
            schedulerModuleObject->cancelUntilEvent();
            // schedule until event
            schedulerModuleObject->until_event->setArrival(schedulerModule->getId(), -1, until);
            getSimulation()->getFES()->insert(schedulerModuleObject->until_event);
            log("MosaikScheduler: until event inserted for time " + std::to_string(until) + " at simtime " + simTime().str());

            is_time_based = pmsg_group.msg().Get(i).is_time_based();
            if (is_time_based) {
                log("MosaikScheduler: is time based");
                // initially send max advance message
                double maxAdvance = simTime().dbl() + 0.001;
                MosaikCtrlEvent *max_advance = new MosaikCtrlEvent("max advance");
                max_advance->setCtrlType(ControlType::MaxAdvance);
                max_advance->setArrival(schedulerModule->getId(), -1, maxAdvance);
                getSimulation()->getFES()->insert(max_advance);
                log("MosaikScheduler: max advance event inserted for time " + std::to_string(maxAdvance) + " at simtime " + simTime().str());
            } else {
                log("MosaikScheduler: is not time based");
            }
        } else if (type == CosimaMsgGroup::CosimaMsg::WAITING) {
            std::string msgId = pmsg_group.msg().Get(i).msgid();
            log("MosaikScheduler: received waiting message " + simTime().str() + " with Id " + msgId);
            // Pretend to have an inserted event to not longer wait for messages
            // Substract 1 because mosaik time is sim time + 1 since agents output time is always one step further.
            // TODO: check if we have to subtract 1
            double waiting_msg_time = ((pmsg_group.msg().Get(i).simtime()-1) * 1.0) / 1000;
            log("waiting msg time " + std::to_string(waiting_msg_time) + " sim time " + simTime().str() );

            if(waiting_msg_time >= simTime().dbl() or until_reached) {
                log("MosaikScheduler: continue simulation.");
                insertedEvent = true;
            } else {
                log("MosaikScheduler: continue waiting at time " + simTime().str() );
                //informMosaikAboutWaiting();
            }
        } else if (type == CosimaMsgGroup::CosimaMsg::DISCONNECT) {
            std::string msgId = pmsg_group.msg().Get(i).msgid();
            double adjustedMosaikSimTime = (pmsg_group.msg().Get(i).simtime() * 1.0) / 1000;
            log("MosaikScheduler: disconnect event inserted for simtime " + std::to_string(adjustedMosaikSimTime) + " for " + pmsg_group.msg().Get(i).change_module() + ".");
            disconnectModules.push_back((pmsg_group.msg().Get(i).change_module()).c_str());

        } else if (type == CosimaMsgGroup::CosimaMsg::RECONNECT) {
            std::string msgId = pmsg_group.msg().Get(i).msgid();
            log("MosaikScheduler: reconnect event inserted for simtime " + std::to_string(adjustedMosaikSimTime) + " for " + pmsg_group.msg().Get(i).change_module() + ".");
            reconnectModules.push_back((pmsg_group.msg().Get(i).change_module()).c_str());
        }
    }
    if(disconnectModules.size() > 0) {
        disconnect_event->setModuleNamesArraySize(disconnectModules.size());
        int counter = 0;
        for(const auto& module: disconnectModules) {
            disconnect_event->setModuleNames(counter, module.c_str());
            counter++;
        }
        disconnect_event->setArrival(scenarioManagerObject->getId(), -1, adjustedMosaikSimTime);
        getSimulation()->getFES()->insert(disconnect_event);
        insertedEvent = true;
        disconnectModules.clear();
    } else {
        delete disconnect_event;
    }
    if(reconnectModules.size() > 0) {
        reconnect_event->setModuleNamesArraySize(reconnectModules.size());
        int counter = 0;
        for(const auto& module: reconnectModules) {
            reconnect_event->setModuleNames(counter, module.c_str());
            counter++;
        }
        reconnect_event->setArrival(scenarioManagerObject->getId(), -1, adjustedMosaikSimTime);
        getSimulation()->getFES()->insert(reconnect_event);
        insertedEvent = true;
        reconnectModules.clear();
    } else {
        delete reconnect_event;
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
    if (is_time_based and until_reached) {
        log("MosaikScheduler: reached until time in mosaik." );
        endSimulation();

    }
    if (!event) {
        // if there are no events, wait until something comes from outside
        // TBD: obey simtimelimit, cpu-time-limit
        // This way targetTime will always be "as far in the future as possible",
        // considering
        // how integer overflows work in conjunction with comparisons in C++ (in
        // practice...)
        if (until_reached) {
            log("MosaikScheduler: reached until time in mosaik." );
            endSimulation();
        }
        targetTime = INT64_MAX;

    } else if (initial_message_received){
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
    if (initial_message_received) {
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
    snapshotFile.open ("../snapshot.txt");
    snapshotFile << eventNumber << " " << messageCounter << " " << errorCounter << " " << maxAdvanceCounter
            << " " << disconnectCounter << " " << reconnectCounter << " " << lastEventTime.str();
    snapshotFile.close();
}


void MosaikScheduler::endSimulation() {
    writeSimulationSnapshot();
    if (is_time_based) {
        getSimulation()->getFES()->clear();
    }
    log("MosaikScheduler: end simulation" );
    getSimulation()->callFinish();
    // close socket connection to mosaik
    closesocket(connSocket);
    getSimulation()->getActiveEnvir()->alert("End of simulation");

}

void MosaikScheduler::sendMsgGroupToMosaik() {
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

        if (not until_reached) {
            log("MosaikScheduler: receive new messages from mosaik." + simTime().str() );
            receiveUntil(INT64_MAX);
        }
    } catch(...) {
        log("exception");
    }

}

int MosaikScheduler::getNumberOfMessagesInPMSGGroup() {
    return pmsg_group.msg().size();
}


void MosaikScheduler::sendToMosaik(cMessage *reply) {
    if (reply == nullptr) {
        throw cRuntimeError("MosaikScheduler: Message is nullptr");
    }
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // round up current simTime
    int current_step = ceil(simTime().dbl()*1000);
    double current_step_d = (current_step * 1.0) / 1000;
    MosaikCtrlEvent *reply_event;

    bool scheduleMessage = true;
    if (typeid(*reply) == typeid(MosaikCtrlEvent)) {
        reply_event = dynamic_cast<MosaikCtrlEvent *>(reply);
        if(reply_event->getCtrlType() == ContinueWaiting) {
            scheduleMessage = false;
        }
    }
    if (scheduleMessage) {
        if(pmsg_group.current_mosaik_step() == current_step) {
            log("MosaikScheduler: request to send message for the same step " + std::to_string(current_step) + " back to mosaik." );
        } else {
            pmsg_group.set_current_mosaik_step(current_step);

            // get scheduler module
            MosaikSchedulerModule *schedulerModuleObject = dynamic_cast<MosaikSchedulerModule *>(schedulerModule);
            MosaikCtrlEvent* end_of_step_event = new MosaikCtrlEvent("send message to mosaik");
            end_of_step_event->setCtrlType(ControlType::EndOfStep);
            end_of_step_event->setArrival(schedulerModule->getId(), -1, current_step_d);
            getSimulation()->getFES()->insert(end_of_step_event);
            log("MosaikScheduler: insert event 'send message to mosaik' for time " + std::to_string(current_step_d) );

        }
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
        int creation_time = reply_msg->getCreationTime();

        pmsg->set_sender(sender);
        pmsg->set_receiver(receiver);
        pmsg->set_content(content);
        pmsg->set_simtime(current_step);
        pmsg->set_msgid(msgId);
        pmsg->set_creation_time(creation_time);
        pmsg->set_type(CosimaMsgGroup::CosimaMsg::INFO);

        if(reply_msg->getTransmission_error()) {
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::TRANSMISSION_ERROR);
            std::stringstream errorCounterStr;
            errorCounterStr << errorCounter;
            std::string m_id = "ErrorMessage_" + errorCounterStr.str();
            pmsg->set_msgid(m_id);
            errorCounter += 1;

            log("MosaikScheduler: send notification for transmission error for message with sender " + sender +
                    " and receiver " + receiver + " and id " + m_id + " back to mosaik. " );
        } else if (reply_msg->getDisconnected_event()) {
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::DISCONNECT);
            std::stringstream disconnectCounterStr;
            disconnectCounterStr << disconnectCounter;
            std::string m_id = "DisconnectNotificationMessage_" + disconnectCounterStr.str();
            pmsg->set_msgid(m_id);
            pmsg->set_simtime(current_step + 1);
            pmsg->set_connection_change_successful(reply_msg->getConnection_change_successful());
            disconnectCounter += 1;

            log("MosaikScheduler: send notification for disconnect for client " + sender +
                    " and id " + m_id + " back to mosaik. " );
        } else if (reply_msg->getReconnected_event()) {
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::RECONNECT);
            std::stringstream reconnectCounterStr;
            reconnectCounterStr << reconnectCounter;
            std::string m_id = "ReconnectNotificationMessage_" + reconnectCounterStr.str();
            pmsg->set_msgid(m_id);
            pmsg->set_simtime(current_step + 1);
            pmsg->set_connection_change_successful(reply_msg->getConnection_change_successful());
            reconnectCounter += 1;

            log("MosaikScheduler: send notification for reconnect for client " + sender +
                    " and id " + m_id + " back to mosaik. " );
        }
        else {
            double delay = reply_msg->getDelay();
            delay = delay/1000;

            pmsg->set_delay(delay);
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::INFO);
            log("MosaikScheduler: send message with content '" + content + "' and sender " + sender
                    + " and delay " + std::to_string(delay) + " back to mosaik. " );
        }

    } else if (typeid(*reply) == typeid(MosaikCtrlEvent)) {
        if (reply_event->getCtrlType() == ContinueWaiting) {
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::WAITING);
            pmsg->set_simtime(current_step + 1);


            log("MosaikScheduler: send message with waiting info back to mosaik");
            sendMsgGroupToMosaik();

        } else if (reply_event->getCtrlType() == MaxAdvance) {
            // message is max advance event

            // set type to protobuf message and serialize message
            pmsg->set_type(CosimaMsgGroup::CosimaMsg::MAX_ADVANCE);
            std::stringstream maxAdvanceCounterStr;
            maxAdvanceCounterStr << maxAdvanceCounter;
            std::string m_id = "MaxAdvanceMessage_" + maxAdvanceCounterStr.str();
            pmsg->set_msgid(m_id);
            pmsg->set_simtime(current_step + 1);
            maxAdvanceCounter += 1;
            if (is_time_based) {
                // schedule new max advance event for new step
                MosaikSchedulerModule *schedulerModuleObject = dynamic_cast<MosaikSchedulerModule *>(schedulerModule);
                if (!schedulerModuleObject->max_adv_event->isScheduled()) {
                    schedulerModuleObject->max_adv_event->setArrival(schedulerModule->getId(), -1, current_step_d+0.001);
                    getSimulation()->getFES()->insert(schedulerModuleObject->max_adv_event);
                    log("insert max advance event for " + std::to_string(current_step_d+0.001));
                } else {
                    log("max advance event already scheduled");
                }
            }
            log("MosaikScheduler: send message with max advance info back to mosaik with Message id " + m_id );

        }

    }

}

