/*
 * CosimaScheduler.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */
#include "CosimaScheduler.h"

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
#include <future>
#include <mutex>
#include "../colors.h"
#include <arpa/inet.h>


#include "../messages/message.pb.h"

#include "inet/common/packet/Packet.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"
#include "../messages/message.pb.h"
#include "../messages/AttackEvent_m.h"
#include "../messages/ControlType_m.h"
#include "../messages/AttackType_m.h"
#include "../messages/CosimaApplicationChunk_m.h"
#include "../messages/CosimaCtrlEvent_m.h"
#include "CosimaScenarioManager.h"
#include "CosimaSchedulerModule.h"
#include "inet/networklayer/common/L3AddressResolver.h"


using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::microseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

Register_Class(CosimaScheduler);

using namespace omnetpp;

auto MAX_BYTE_SIZE_PER_MSG = 100000;

// counter for messages
auto maxAdvanceCounter = 0U;
auto errorCounter = 0U;
auto disconnectCounter = 0U;
auto reconnectCounter = 0U;
auto messageCounter = 0U;

// parameter from coupled simulation
auto stepSize = 1.0;
auto until = 100.0;
auto untilReached = false; // indicates whether simulation end is reached in coupled simulation
auto initialMessageReceived = false;

auto receiveUntilLock = false;

auto eventScheduled = false;

auto currentStepInMessage = 0U;
auto lastStepInMessage = 0U;

std::string loggingLevel;

class Message {
    /*
     * This is a class, that may represent all used kinds of protocol buffer messages used in this project.
     * It is used for lists of messages that might be of any type.
     */
    SynchronisationMessage syncMessage; // SynchronisationMessages are used for time synchronisation between coupled simulation and OMNeT++.
    InfoMessage infoMessage; // InfoMessages contain messages, that need to be simulated in the OMNeT++ network.
    InfrastructureMessage infrastructureMessage; // InfrastructureMessages are used for dynamic changes in the infrastructure of the OMNeT++ network.

    enum {SYNCHRONISATION_MESSAGE, INFO_MESSAGE, INFRASTRUCTURE_MESSAGE} messageType;
public:
    Message(){}

    void setMessage(SynchronisationMessage syncMessage) {
        this->syncMessage = syncMessage;
        this->messageType = SYNCHRONISATION_MESSAGE;
    }
    void setMessage(InfoMessage infoMessage) {
        this->infoMessage = infoMessage;
        this->messageType = INFO_MESSAGE;
    }
    void setMessage(InfrastructureMessage infrastructureMessage) {
        this->infrastructureMessage = infrastructureMessage;
        this->messageType = INFRASTRUCTURE_MESSAGE;
    }
    void printInfo() {
        if(this->messageType == INFO_MESSAGE) {
            this->infoMessage.PrintDebugString();
        }
        else if(this->messageType == SYNCHRONISATION_MESSAGE) {
            this->syncMessage.PrintDebugString();
        }
        else if(this->messageType == INFRASTRUCTURE_MESSAGE) {
            this->infrastructureMessage.PrintDebugString();
        }
    }
    int getByteSize() {
        if(this->messageType == INFO_MESSAGE) {
            return this->infoMessage.ByteSize();
        }
        else if(this->messageType == SYNCHRONISATION_MESSAGE) {
            return this->syncMessage.ByteSize();
        }
        else if(this->messageType == INFRASTRUCTURE_MESSAGE) {
            return this->infrastructureMessage.ByteSize();
        }
        return -1;
    }
    CosimaMsgGroup addMessageToMsgGroup(CosimaMsgGroup msgGroup) {
        if(this->messageType == INFO_MESSAGE) {
            InfoMessage *infoMessage = msgGroup.add_info_messages();
            infoMessage->CopyFrom(this->infoMessage);
        }
        else if(this->messageType == SYNCHRONISATION_MESSAGE) {
            SynchronisationMessage *syncMessage = msgGroup.add_synchronisation_messages();
            syncMessage->CopyFrom(this->syncMessage);
        }
        else if(this->messageType == INFRASTRUCTURE_MESSAGE) {
            InfrastructureMessage *infrastructureMessage = msgGroup.add_infrastructure_messages();
            infrastructureMessage->CopyFrom(this->infrastructureMessage);
        }
        return msgGroup;
    }
};

std::list<Message> messages;

int CosimaScheduler::getNumberOfSavedMessages() {
    int size = messages.size();
    return size;
}

struct Receiver {
    std::vector<char> operator()(SOCKET listenerSocket) {
        std::vector<char> buf(MAX_BYTE_SIZE_PER_MSG);

        // accept connection, and store FD in connSocket
        if (untilReached) {
            return buf;
        }
        sockaddr_in sinRemote;
        int addrSize = sizeof(sinRemote);
        SOCKET sock = accept(listenerSocket, (sockaddr *)&sinRemote,
                (socklen_t *)&addrSize);
        if (sock == INVALID_SOCKET)
            throw cRuntimeError("CosimaScheduler: accept() failed. Invalid Socket!");
        EV << "CosimaScheduler: connected!\n" << endl;
        int bytes = recv(sock, buf.data(), buf.size(), 0);
        return buf;
    }
};

CosimaScheduler::CosimaScheduler() : cScheduler() {
    // variables for socket connection to coupled simulation
    listenerSocket = INVALID_SOCKET;
    numModules = 0;

    // message for scheduling socket events from coupled simulation
    socketEvent = nullptr;


}

CosimaScheduler::~CosimaScheduler() = default;

std::string CosimaScheduler::info() const {
    return ("scheduler with socket option for integration of other simulation framework");
}

void CosimaScheduler::startRun() {
    if (initsocketlibonce() != 0)
        throw cRuntimeError("CosimaScheduler: Cannot initialize socket library");
    setupListener();
}

void CosimaScheduler::setupListener() {
    listenerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerSocket == INVALID_SOCKET)
        throw cRuntimeError("CosimaScheduler: cannot create socket. Socket is invalid");

    int enable = 1;
    if (setsockopt(listenerSocket, SOL_SOCKET, SO_REUSEADDR,
            (const char *)&enable, sizeof(int)) < 0)
        throw cRuntimeError("CosimaScheduler: cannot set socket option. Socket is invalid");

    sockaddr_in sinInterface;
    sinInterface.sin_family = AF_INET;
    sinInterface.sin_addr.s_addr = INADDR_ANY;
    sinInterface.sin_port = htons(PORT);
    if (bind(listenerSocket, (sockaddr *)&sinInterface, sizeof(sockaddr_in)) ==
            SOCKET_ERROR)
        throw cRuntimeError("CosimaScheduler: socket bind() failed. Please check if another instance of the simulation is already running.");

    listen(listenerSocket, SOMAXCONN);


}

void CosimaScheduler::endRun() {
    close(listenerSocket);
    endSimulation();
}

void CosimaScheduler::executionResumed() {
    cScheduler::executionResumed();
}

void CosimaScheduler::printCurrentTime() {
    std::cout << getCurrentTime() << " ";
}

std::string CosimaScheduler::getCurrentTime() {
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int usec = curTime.tv_usec;

    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    char currentTime[84] = "";
    sprintf(currentTime, "%s:%d", buffer, usec);
    return currentTime;
}

void CosimaScheduler::log(std::string info, std::string logLevel) {
    if(logLevel.compare("warning") == 0) {
        printCurrentTime();
        std::cout << FRED(" | WARNING | OMNeT++: ");
        std::cout << info << endl;
    }
    if((logLevel.compare("info")) == 0 and (loggingLevel.compare("info") == 0 or loggingLevel.compare("debug") == 0)) {
        printCurrentTime();
        std::cout << FBLU(" | INFO    | OMNeT++: ");
        std::cout << info << endl;
    }
    if(loggingLevel.compare("debug") == 0) {
        printCurrentTime();
        std::cout << " | DEBUG   | OMNeT++: ";
        std::cout << info << endl;
    }

    auto written = false;
    while (not written) {
        try {
            std::fstream f;

            // opening file using ofstream
            f.open("../log.txt", std::ios::app);
            if (!f) {
                return;
            } else{
                f << getCurrentTime() << " OMNeT++: " << info << "\n";
                f.close();
                written = true;
            }
        } catch(...) {
        }
    }

}

void CosimaScheduler::log(std::string info) {
    log(info, "debug");
}

void CosimaScheduler::setInterfaceModule(cModule *mod, bool isCosimaSchedulerModule) {
    if (mod == nullptr) {
        throw cRuntimeError(
                "CosimaScheduler: setInterfaceModule() not called with a module object");
    }
    std::string module_name = std::string(mod->getName());
    if (isCosimaSchedulerModule) {
        schedulerModule = mod;
    } else {
        // possible to add several modules to array
        addModule(mod);
    }
}

std::list<omnetpp::cModule *> CosimaScheduler::getModuleList () {
    return modules;
}

void CosimaScheduler::setScenarioManager(cModule *manager) {
    log("CosimaScheduler: registered ScenarioManager.");
    scenarioManager = manager;
}

void CosimaScheduler::setAttackNetworkLayer(omnetpp::cModule *networkLayerModule) {
    attackModules.push_back(networkLayerModule);
}


void CosimaScheduler::addModule(cModule *mod) {
    modules.push_back(mod);
    numModules = numModules + 1;
}

int CosimaScheduler::getPortForModule(std::string module_name) {
    if (module_name == "") {
        return -1;
    }
    cModule *submod = getReceiverModule(module_name);
    if (submod != nullptr and submod->hasPar("localPort")) {
        return (submod->par("localPort"));
    }
    return -1;
}

std::string CosimaScheduler::getModuleNameFromPort(int port) {
    for (auto const& module : modules) {
        if(module->par("localPort").intValue() == port) {
            return module->getParentModule()->getName();
        }
    }
    return "";
}

cModule *CosimaScheduler::getReceiverModule(std::string module_name) {
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

cModule *CosimaScheduler::getAttackNetworkLayerModule(std::string module_name) {
    // get corresponding module to server name
    cModule *matchingModule = nullptr;
    for (auto const& module : attackModules) {
        std::string moduleNameCheck = std::string(module->getParentModule()->getParentModule()->getName());
        if (moduleNameCheck.compare(module_name) == 0) {
            matchingModule = module;
        }
    }
    if (matchingModule == nullptr) {
        throw cRuntimeError("No network layer module found, that implements attack functionality. Make sure to use the special AttackUe module.");
    }
    return (matchingModule);
}

int CosimaScheduler::handleMsgFromCoupledSimulation(std::vector<char> data) {
    if (data.size() == 0) {
        return 0;
    }
    log("CosimaScheduler: handles msg!!!");
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    CosimaMsgGroup pmsg_group;
    auto success = pmsg_group.ParseFromString(data.data());

    bool insertedEvent = false;

    CosimaSchedulerModule *schedulerModuleObject = dynamic_cast<CosimaSchedulerModule *>(schedulerModule);
    CosimaScenarioManager *scenarioManagerObject = dynamic_cast<CosimaScenarioManager *>(scenarioManager);

    auto disconnectMsgCounter = 0U;
    CosimaCtrlEvent *disconnectEvent = new CosimaCtrlEvent();
    disconnectEvent->setModuleNamesArraySize(pmsg_group.infrastructure_messages().size());
    std::vector<std::string> disconnectModules;
    disconnectEvent->setCtrlType(Disconnect);

    auto connectMsgCounter = 0U;
    CosimaCtrlEvent *reconnectEvent = new CosimaCtrlEvent();
    reconnectEvent->setModuleNamesArraySize(pmsg_group.infrastructure_messages().size());
    std::vector<std::string> reconnectModules;
    reconnectEvent->setCtrlType(Reconnect);

    auto adjustedCouplingSimTime = 0.0;


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
        log("CosimaScheduler: until event inserted for time " + std::to_string(until) + " at simtime " + simTime().str(), "info");

        stepSize = initialMessage.step_size();
        auto loggingLevelMsg = initialMessage.logging_level();
        std::string loggingLevelStr = loggingLevelMsg;
        loggingLevel = loggingLevelStr;
        log("Setting logging level to: " + loggingLevelStr, "info");
        MAX_BYTE_SIZE_PER_MSG = initialMessage.max_byte_size_per_msg_group();
        log("Setting max byte size per message to " + std::to_string(MAX_BYTE_SIZE_PER_MSG));
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
            // create message with parameters from coupled simulation
            CosimaSchedulerMessage *msg = new CosimaSchedulerMessage();
            msg->setSender(sender.c_str());
            msg->setReceiver(receiver.c_str());
            msg->setSize(size);
            msg->setContent(content.c_str());
            msg->setMsgId(msgId.c_str());
            msg->setCreationTime(msgCreationTime);

            // cast message
            cPacket *pkt = dynamic_cast<cPacket *>(msg);
            socketEvent = dynamic_cast<cMessage *>(pkt);


            adjustedCouplingSimTime = (infoMessage.sim_time() * 1.0) / 1000;

            log("Cosima Scheduler received message with sender: " + sender + " for simTime " + std::to_string(adjustedCouplingSimTime) + " and id " + msgId );

            // schedule message as an event at the sender module
            socketEvent->setArrival(module->getId(), -1, adjustedCouplingSimTime);
            getSimulation()->getFES()->insert(socketEvent);
            insertedEvent = true;
            eventScheduled = true;
            log("CosimaScheduler: message with sender " + sender + " and id " + msgId + " inserted.");
            // schedule max advance event
            schedulerModuleObject->cancelMaxAdvanceEvent();
            schedulerModuleObject->maxAdvEvent->setCtrlType(ControlType::MaxAdvance);
            if (maxAdvance >= simTime().dbl()) {
                schedulerModuleObject->maxAdvEvent->setArrival(schedulerModule->getId(), -1, maxAdvance);
                getSimulation()->getFES()->insert(schedulerModuleObject->maxAdvEvent);
                log("CosimaScheduler: max advanced event inserted for time " + std::to_string(maxAdvance) + " at simtime " + simTime().str());
            } else {
                log("max advance " + std::to_string(maxAdvance) + " at time " + simTime().str() + " not inserted");
            }

        } else {
            throw cRuntimeError("CosimaScheduler: Can't find module in modules array. Please make sure, that the number of agents matchers the number of modules in the scenario.");
        }
    }

    // get synchronization messages
    for(auto i=0; i < pmsg_group.synchronisation_messages().size(); i++) {
        SynchronisationMessage syncMessage = pmsg_group.synchronisation_messages().Get(i);
        if (syncMessage.msg_type() == SynchronisationMessage_MsgType_WAITING) {
            std::string msgId = syncMessage.msg_id();
            log("CosimaScheduler: received waiting message with Id " + msgId);
            // Pretend to have an inserted event to not longer wait for messages
            auto waitingMsgTime = ((syncMessage.sim_time()) * 1.0) / 1000;
            auto maxAdvance = ((syncMessage.max_advance()) * 1.0) / 1000;
            log("waiting msg time " + std::to_string(waitingMsgTime) + " sim time " + simTime().str() + " max advance " + std::to_string(maxAdvance));

            // schedule max advance event
            schedulerModuleObject->cancelMaxAdvanceEvent();
            schedulerModuleObject->maxAdvEvent->setCtrlType(ControlType::MaxAdvance);
            if (maxAdvance >= simTime().dbl()) {
                schedulerModuleObject->maxAdvEvent->setArrival(schedulerModule->getId(), -1, maxAdvance);
                getSimulation()->getFES()->insert(schedulerModuleObject->maxAdvEvent);
                log("CosimaScheduler: max advanced event inserted for time " + std::to_string(maxAdvance) + " at simtime " + simTime().str());
                if(waitingMsgTime >= simTime().dbl() or untilReached) {
                    log("CosimaScheduler: continue simulation.");
                    insertedEvent = true;
                    eventScheduled = true;
                } else {
                    log("CosimaScheduler: continue waiting at time " + simTime().str() );
                    informCoupledSimulationAboutWaiting();
                }
            } else {
                log("max advance " + std::to_string(maxAdvance) + " at time " + simTime().str() + " not inserted");
                log("CosimaScheduler: continue waiting at time " + simTime().str() );
                informCoupledSimulationAboutWaiting();
            }
        }

    }

    // get infrastructure messages
    for(auto i=0; i < pmsg_group.infrastructure_messages().size(); i++) {
        InfrastructureMessage infrastructureMessage = pmsg_group.infrastructure_messages().Get(i);
        adjustedCouplingSimTime = (infrastructureMessage.sim_time() * 1.0) / 1000;
        if (infrastructureMessage.change_module() != "") {
            if (infrastructureMessage.msg_type() == InfrastructureMessage_MsgType_DISCONNECT) {
                auto msgId = infrastructureMessage.msg_id();
                auto adjustedCouplingSimTime = (infrastructureMessage.sim_time() * 1.0) / 1000;
                log("CosimaScheduler: disconnect event inserted for simtime " + std::to_string(adjustedCouplingSimTime) + " for " + infrastructureMessage.change_module() + ".");
                disconnectModules.push_back((infrastructureMessage.change_module()).c_str());

            } else if (infrastructureMessage.msg_type() == InfrastructureMessage_MsgType_RECONNECT) {
                auto msgId = infrastructureMessage.msg_id();
                log("CosimaScheduler: reconnect event inserted for simtime " + std::to_string(adjustedCouplingSimTime) + " for " + infrastructureMessage.change_module() + ".");
                reconnectModules.push_back((infrastructureMessage.change_module()).c_str());
            }
        } else {
            log("Attention: no module name for infrastructure change set!");
        }

    }

    // get traffic messages
    for(auto i=0; i < pmsg_group.traffic_messages().size(); i++) {
        TrafficMessage trafficMessage = pmsg_group.traffic_messages().Get(i);
        CosimaCtrlEvent *trafficEvent = new CosimaCtrlEvent();
        trafficEvent->setCtrlType(Traffic);
        trafficEvent->setSource(trafficMessage.source().c_str());
        trafficEvent->setDestination(trafficMessage.destination().c_str());
        trafficEvent->setStart(trafficMessage.start());
        trafficEvent->setStop(trafficMessage.stop());
        trafficEvent->setInterval(trafficMessage.interval());
        trafficEvent->setPacketLength(trafficMessage.packet_length());
        auto arrivalTime = (trafficMessage.start() * 1.0) / 1000;
        trafficEvent->setArrival(scenarioManagerObject->getId(), -1, arrivalTime);
        log("CosimaScheduler: traffic event inserted for simtime " + std::to_string(arrivalTime) + " for " + trafficMessage.source() + " to " + trafficMessage.destination() + ".");
        getSimulation()->getFES()->insert(trafficEvent);
    }

    for(auto i=0; i < pmsg_group.attack_messages().size(); i++) {
        AttackMessage attackMessage = pmsg_group.attack_messages().Get(i);
        AttackEvent *attackEvent = new AttackEvent();
        if(attackMessage.msg_type() == AttackMessage_MsgType_PACKET_DELAY) {
            attackEvent->setAttackType(PacketDelay);
        }
        else if(attackMessage.msg_type()  == AttackMessage_MsgType_PACKET_DROP) {
            attackEvent->setAttackType(PacketDrop);
        }
        else if(attackMessage.msg_type() == AttackMessage_MsgType_PACKET_FALSIFICATION) {
            attackEvent->setAttackType(PacketFalsification);
        }
        attackEvent->setAttacked_module(attackMessage.attacked_module().c_str());
        attackEvent->setStart(attackMessage.start());
        attackEvent->setStop(attackMessage.stop());
        double attackProbability = attackMessage.attack_probability();
        attackEvent->setProbability(attackProbability);
        attackEvent->setArrival(scenarioManagerObject->getId(), -1, simTime());
        getSimulation()->getFES()->insert(attackEvent);
    }

    if(disconnectModules.size() > 0) {
        disconnectEvent->setModuleNamesArraySize(disconnectModules.size());
        auto counter = 0;
        for(const auto& module: disconnectModules) {
            disconnectEvent->setModuleNames(counter, module.c_str());
            counter++;
        }
        disconnectEvent->setArrival(scenarioManagerObject->getId(), -1, adjustedCouplingSimTime); //TODO: set to time in msg
        getSimulation()->getFES()->insert(disconnectEvent);
        insertedEvent = true;
        eventScheduled = true;

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
        reconnectEvent->setArrival(scenarioManagerObject->getId(), -1, adjustedCouplingSimTime); //TODO: set to time in msg
        getSimulation()->getFES()->insert(reconnectEvent);
        insertedEvent = true;
        eventScheduled = true;

        reconnectModules.clear();
    } else {
        delete reconnectEvent;
    }

    return (pmsg_group.number_of_message_groups());
}

void CosimaScheduler::informCoupledSimulationAboutWaiting() {
    CosimaCtrlEvent *waitingEvent = new CosimaCtrlEvent();
    waitingEvent->setCtrlType(ContinueWaiting);
    sendToCoupledSimulation(waitingEvent);
}

void CosimaScheduler::receive() {
    while (true) {
        auto returnValue = std::async(std::launch::async, Receiver(), listenerSocket).get();
        if (strlen(returnValue.data()) != 0) {
            int numberOfMsgGroups = handleMsgFromCoupledSimulation(returnValue);
            for (int i=0; i < numberOfMsgGroups-1; i++) {
                auto returnValue = std::async(std::launch::async, Receiver(), listenerSocket).get();
                handleMsgFromCoupledSimulation(returnValue);
            }
            return;
        }
    }
}

cEvent *CosimaScheduler::guessNextEvent() {
    return (sim->getFES()->peekFirst());
}

cEvent *CosimaScheduler::takeNextEvent() {
    // calculate target time
    cEvent *event = sim->getFES()->peekFirst();
    if (!event) {
        if (untilReached) {
            log("CosimaScheduler: reached until time in coupled simulation." );
            endSimulation();
        }

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
                                    log("CosimaScheduler: There was an error. Send notification message to coupled simulation. " );
                                    CosimaSchedulerMessage *notification_message = new CosimaSchedulerMessage();
                                    notification_message->setTransmission_error(true);
                                    notification_message->setConnection_change_successful(true);
                                    notification_message->setSender(sourceModule->getName());
                                    sendToCoupledSimulation(notification_message);
                                }

                            } else {
                                offset += chunk->getChunkLength();
                            }
                        }
                    }
                }

            };
        } catch (...) {
            log("CosimaScheduler: couldn't find host");

        }

        // use time of next event
        simtime_t eventSimtime = event->getArrivalTime();
        if(eventSimtime < lastEventTime) {
            log("ATTENTION! CosimaScheduler: try to execute event for simtime " + eventSimtime.str() + ", but last event was at time " + lastEventTime.str() );
            throw cRuntimeError("Wrong simulation order. Please contact the Cosima Developer Team: https://cosima.offis.de/pages/contact");
        }
        lastEventTime = eventSimtime;
    } else {
        log("Waiting for connection");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (not eventScheduled) {
        receive();
    }

    if (initialMessageReceived) {
        // remove event from FES and return it
        cEvent *tmp = sim->getFES()->removeFirst();
        ASSERT(tmp == event);
        return (event);
    }
    return nullptr;
}

void CosimaScheduler::putBackEvent(cEvent *event) {
    sim->getFES()->putBackFirst(event);
}

void CosimaScheduler::writeSimulationSnapshot() {
    // write snapshot to file
    eventnumber_t eventNumber = getSimulation()->getEventNumber();
    std::ofstream snapshotFile;
    snapshotFile.open ("../tests/integration_tests/snapshot.txt");
    snapshotFile << eventNumber << " " << messageCounter << " " << errorCounter << " " << maxAdvanceCounter
            << " " << disconnectCounter << " " << reconnectCounter << " " << lastEventTime.str();
    snapshotFile.close();
}


void CosimaScheduler::endSimulation() {
    writeSimulationSnapshot();
    log("CosimaScheduler: end simulation" );
    getSimulation()->callFinish();
    getSimulation()->getActiveEnvir()->alert("End of simulation");

}

void CosimaScheduler::sendMsgGroupToCoupledSimulation(bool isWaitingMsg) {
    std::list<CosimaMsgGroup> msgGroups;
    CosimaMsgGroup msgGroup;

    msgGroup.set_current_time_step(currentStepInMessage);

    std::list<Message> newMessagesList;
    if(isWaitingMsg) {
        newMessagesList.push_back(messages.back());
    } else {
        std::copy(messages.begin(), messages.end(), std::back_inserter(newMessagesList));
        messages.clear();
        lastStepInMessage = currentStepInMessage;
    }

    auto length = newMessagesList.size();
    if (length == 0) {
        return;
    }

    int index = 0;

    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(4243);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return;
    }
    for (auto message: newMessagesList) {
        auto byteSize = message.getByteSize();

        if (msgGroup.ByteSize() + byteSize >= (MAX_BYTE_SIZE_PER_MSG - 100)) { // 3 bytes for "END" and 3 bytes for size
            // message size is outside boundaries -> add message group to list and make new message group
            CosimaMsgGroup msgGroupCopy;
            msgGroupCopy.CopyFrom(msgGroup);
            msgGroups.push_back(msgGroupCopy);
            msgGroup.Clear();
            msgGroup.set_current_time_step(currentStepInMessage);
        }
        msgGroup = message.addMessageToMsgGroup(msgGroup);
        msgGroup.set_number_of_messages(length);
        if (index == length-1) {
            // is last saved message -> set flag and add to list
            msgGroups.push_back(msgGroup);
        }
        index++;
    }
    for (auto msgGroup: msgGroups) {
        msgGroup.set_number_of_message_groups(msgGroups.size());


        // serialize message
        std::string msg;
        msgGroup.SerializeToString(&msg);
        msg += "END";
        int numOfBytesbefore = strlen(msg.c_str());

        msg.insert(msg.end(), MAX_BYTE_SIZE_PER_MSG - msg.size(), ' ');
        int numOfBytes = strlen(msg.c_str());

        if ((numOfBytes) != MAX_BYTE_SIZE_PER_MSG) {
            throw cRuntimeError("CosimaScheduler: number of bytes is not max byte size per msg");
        }

        // send message over TCP socket
        std::future<ssize_t> asyncSend = std::async(send, sock, msg.c_str(), numOfBytes, 0);
        size_t size = asyncSend.get();
        msg.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    close(sock);
    receive();
}

void CosimaScheduler::setUntilReached(bool untilReachedValue) {
    untilReached = untilReachedValue;
}

bool CosimaScheduler::getUntilReached() {
    return untilReached;
}

void CosimaScheduler::setInitialMessageReceived(bool value) {
    initialMessageReceived = value;
}


void CosimaScheduler::sendToCoupledSimulation(cMessage *reply) {
    if (reply == nullptr) {
        throw cRuntimeError("CosimaScheduler: Message is nullptr");
    }
    // round up current simTime
    auto currentStep = 0U;
    currentStep = ceil(simTime().dbl()*1000);
    auto currentStep_d = (currentStep * 1.0) / 1000;
    CosimaCtrlEvent *replyEvent;

    auto scheduleMessage = true;
    if (typeid(*reply) == typeid(CosimaCtrlEvent)) {
        replyEvent = dynamic_cast<CosimaCtrlEvent *>(reply);
        if(replyEvent->getCtrlType() == ContinueWaiting) {
            scheduleMessage = false;
        }
    }
    if (scheduleMessage) {
        if (currentStep == lastStepInMessage) {
            currentStep += 1;
            currentStep_d = (currentStep * 1.0) / 1000;
        }
        if(currentStepInMessage == currentStep) {
            log("CosimaScheduler: request to send message for the same step " + std::to_string(currentStep) + " back to coupled simulation." );
        } else {
            // get scheduler module
            CosimaSchedulerModule *schedulerModuleObject = dynamic_cast<CosimaSchedulerModule *>(schedulerModule);
            CosimaCtrlEvent* endOfStepEvent = new CosimaCtrlEvent("send message to coupled simulation");
            endOfStepEvent->setCtrlType(ControlType::EndOfStep);
            endOfStepEvent->setArrival(schedulerModule->getId(), -1, currentStep_d);
            getSimulation()->getFES()->insert(endOfStepEvent);
            currentStepInMessage = currentStep;
            log("CosimaScheduler: insert event 'send message to coupled simulation' for time " + std::to_string(currentStep_d) );
        }
    }

    if (typeid(*reply) == typeid(CosimaSchedulerMessage)) {
        CosimaSchedulerMessage *replyMsg = dynamic_cast<CosimaSchedulerMessage *>(reply);

        // get message content
        auto sender = replyMsg->getSender();
        auto receiver = replyMsg->getReceiver();
        auto content = replyMsg->getContent();
        auto msgId = replyMsg->getMsgId();
        auto creationTime = replyMsg->getCreationTime();
        auto isFalsified = replyMsg->getFalsified();

        std::stringstream logContent;

        if(replyMsg->getTransmission_error()) {
            SynchronisationMessage syncMessage;
            // SynchronisationMessage* syncMessage = pmsg_group.add_synchronisation_messages();

            syncMessage.set_msg_type(SynchronisationMessage_MsgType_TRANSMISSION_ERROR);
            syncMessage.set_timeout(replyMsg->getTimeout());
            syncMessage.set_timeout_msg_id(replyMsg->getMsgId());

            std::stringstream errorCounterStr;
            errorCounterStr << errorCounter;
            auto mId = "ErrorMessage_" + errorCounterStr.str();
            syncMessage.set_msg_id(mId);
            errorCounter += 1;

            Message message;
            message.setMessage(syncMessage);
            messages.push_back(message);

            logContent << "CosimaScheduler: send notification for transmission error for message with sender " << sender <<
                    " and receiver " << receiver << " and messageId " << replyMsg->getMsgId() << " as message " <<  mId <<
                    " back to coupled simulation. ";
            log(logContent.str());
            logContent.str("");
        } else if (replyMsg->getDisconnected_event()) {
            InfrastructureMessage infrastructureMessage;

            infrastructureMessage.set_msg_type(InfrastructureMessage_MsgType_DISCONNECT);

            std::stringstream disconnectCounterStr;
            disconnectCounterStr << disconnectCounter;
            auto mId = "DisconnectNotificationMessage_" + disconnectCounterStr.str();
            infrastructureMessage.set_msg_id(mId);
            infrastructureMessage.set_sim_time(currentStep);
            infrastructureMessage.set_connection_change_successful(replyMsg->getConnection_change_successful());
            disconnectCounter += 1;

            Message message;
            message.setMessage(infrastructureMessage);
            messages.push_back(message);

            logContent << "CosimaScheduler: send notification for disconnect for client " << sender <<
                    " and id " << mId << " back to coupled simulation. ";
            log(logContent.str());
            logContent.str("");
        } else if (replyMsg->getReconnected_event()) {
            InfrastructureMessage infrastructureMessage;

            infrastructureMessage.set_msg_type(InfrastructureMessage_MsgType_RECONNECT);
            std::stringstream reconnectCounterStr;
            reconnectCounterStr << reconnectCounter;
            auto mId = "ReconnectNotificationMessage_" + reconnectCounterStr.str();
            infrastructureMessage.set_msg_id(mId);
            infrastructureMessage.set_sim_time(currentStep);
            infrastructureMessage.set_connection_change_successful(replyMsg->getConnection_change_successful());
            reconnectCounter += 1;

            Message message;
            message.setMessage(infrastructureMessage);
            messages.push_back(message);

            logContent << "CosimaScheduler: send notification for reconnect for client " << sender <<
                    " and id " << mId << " back to coupled simulation. ";
            log(logContent.str());
            logContent.str("");
        }
        else {
            InfoMessage infoMessage;

            infoMessage.set_sender(sender);
            infoMessage.set_receiver(receiver);
            infoMessage.set_content(content);
            infoMessage.set_sim_time(currentStep);
            infoMessage.set_msg_id(msgId);
            infoMessage.set_creation_time(creationTime);
            infoMessage.set_is_falsified(isFalsified);

            Message message;
            message.setMessage(infoMessage);
            messages.push_back(message);

            logContent << "CosimaScheduler: send message with sender " << sender << " and current step " << currentStep << " back to coupled simulation.";
            log(logContent.str());
        }

    } else if (typeid(*reply) == typeid(CosimaCtrlEvent)) {
        if (replyEvent->getCtrlType() == ContinueWaiting) {
            SynchronisationMessage syncMessage;

            syncMessage.set_msg_type(SynchronisationMessage_MsgType_WAITING);
            syncMessage.set_sim_time(currentStep);

            Message message;
            message.setMessage(syncMessage);
            messages.push_back(message);

            log("CosimaScheduler: send message with waiting info back to coupled simulation");
            sendMsgGroupToCoupledSimulation(false);

        } else if (replyEvent->getCtrlType() == MaxAdvance) {
            SynchronisationMessage syncMessage;

            syncMessage.set_msg_type(SynchronisationMessage_MsgType_MAX_ADVANCE);

            std::stringstream maxAdvanceCounterStr;
            maxAdvanceCounterStr << maxAdvanceCounter;
            auto mId = "MaxAdvanceMessage_" + maxAdvanceCounterStr.str();
            syncMessage.set_msg_id(mId);
            syncMessage.set_sim_time(currentStep);
            maxAdvanceCounter += 1;

            Message message;
            message.setMessage(syncMessage);
            messages.push_back(message);

            log("CosimaScheduler: send message with max advance info back to coupled simulation with Message id " + mId );

        }

    }
}

