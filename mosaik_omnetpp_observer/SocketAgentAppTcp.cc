/*
 * SocketAgentAppTcp.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */
#include <cmath>

#include "SocketAgentAppTcp.h"

#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include <string.h>
#include <algorithm>
#include "MosaikMessage_m.h"
#include "MosaikObserver.h"
#include "TikTokPacket_m.h"
#include "inet/applications/base/ApplicationPacket_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

Define_Module(SocketAgentAppTcp);

#define MSGKIND_CONNECT 1
#define MSGKIND_SEND 2
#define MSGKIND_CLOSE 3
#define MSGKIND_RENEW 4

SocketAgentAppTcp::SocketAgentAppTcp() {
  numRecvBytes = 0;
  observer = nullptr;
}

SocketAgentAppTcp::~SocketAgentAppTcp() {
   cancelAndDelete(timeoutMsg);
   cancelAndDelete(timeoutMsgAlternative);
}

void SocketAgentAppTcp::initialize(int stage) {
  TcpAppBase::initialize(stage);
  if (stage == inet::INITSTAGE_LOCAL) {
    timeoutMsg = new cMessage("timer");
    timeoutMsgAlternative = new cMessage("second timer");
  } else if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
    // get intern socket scheduler from simulation and cast to MosaikObserver
    observer =
        check_and_cast<MosaikObserver *>(getSimulation()->getScheduler());
    // register module at scheduler
    observer->setInterfaceModule(this, recvBuffer, 4000, &numRecvBytes);
  }
}

void SocketAgentAppTcp::putMessage(cMessage *msg, double mosaikSimTime) {
  // message can be socket event -> call handleSocketEvent()
  // or message from network -> handle message
  std::cout << "SocketAgentAppTcp of " << this->getParentModule()->getName()
            << " received message " << msg << endl;

  if (msg->getKind() == 0) {
    if (msg->getArrivalGate() != gate("socketIn")) {
      // message is placed from observer
      handleSocketEvent(msg, mosaikSimTime);
    }
  }else{
      delete msg;
  }
}

void SocketAgentAppTcp::handleSocketEvent(cMessage *msg, double mosaikSimTime) {
  std::cout << "handle Socket Event" << endl;
  // make packet
  if (typeid(*msg) == typeid(TikTokPacket)) {
    std::cout << "Message is TikTokPacket" << endl;
    TikTokPacket *tmsg = dynamic_cast<TikTokPacket *>(msg);
    std::cout << "successful cast" << endl;
    const char *content = tmsg->getContent();
    std::cout << "print content " << content << endl;

    std::cout << "handle socket event, msg: " << content << endl;

    std::string receiverName = tmsg->getReceiver();
    std::cout << "receiver: " << receiverName << endl;
    std::string senderName = tmsg->getSender();
    std::cout << "sender: " << senderName << endl;
    int msgSize = tmsg->getSize();
    std::cout << "size: " << msgSize << endl;

    // get corresponding Port for receiver name
    int receiverPort = observer->getPortForModule(receiverName.c_str());

    std::cout << "Message receiver: " << receiverName
              << ", msgSize: " << msgSize << ", receiverPort: " << receiverPort
              << endl;

    packet = new inet::Packet();
    const auto &payload = inet::makeShared<MosaikMessage>();
    payload->setContent(content);
    payload->setReceiver(receiverName.c_str());
    payload->setSender(senderName.c_str());
    payload->setChunkLength(inet::B(msgSize));
    std::cout << "set CreationTime to " << observer->getSimulation()->getSimTime() << endl;
    payload->setCreationTime(mosaikSimTime);
    packet->insertAtBack(payload);

    // get destination
    par("connectAddress") = receiverName.c_str();
    connSend = (connSend + 1) % 1000;
    par("connectPort") = receiverPort + connSend;
    if(timeoutMsg->isScheduled()) {
              timeoutMsgAlternative->setKind(MSGKIND_CONNECT);
              scheduleAt(simTime(), timeoutMsgAlternative);
          } else {
              timeoutMsg->setKind(MSGKIND_CONNECT);
              scheduleAt(simTime(), timeoutMsg);
          }

    packet->setTimestamp(mosaikSimTime);

    std::cout << "send packet " << packet << " to port " << receiverPort
              << endl;
    delete msg;
  }
}

void SocketAgentAppTcp::handleReply(TikTokPacket *reply) {
  std::cout << "Set message for observer " << reply << endl;
  observer->sendBytes(reply);
}

void SocketAgentAppTcp::handleTimer(cMessage *msg) {
  switch (msg->getKind()) {
    case MSGKIND_CONNECT:
      connect();
      break;

    case MSGKIND_SEND:
      sendData();
      break;

    case MSGKIND_CLOSE:
      close();
      break;

    case MSGKIND_RENEW:
      renew();
      break;
  }
}

void SocketAgentAppTcp::renew() {
  std::cout << "in renew" << endl;
  serverSocket.renewSocket();
  serverSocket.setOutputGate(gate("socketOut"));
  serverSocket.setCallback(this);
  const char *localAddress = par("localAddress");
  int localPort = par("localPort");
  connRecv = (connRecv + 1) % 1000;
  serverSocket.bind(
      localAddress[0] ? inet::L3Address(localAddress) : inet::L3Address(),
      localPort + connRecv);
  serverSocket.listen();
}

void SocketAgentAppTcp::socketEstablished(inet::TcpSocket *socket) {
    std::cout << "socket established" << endl;
  inet::TcpAppBase::socketEstablished(socket);
  if(timeoutMsg->isScheduled()) {
            timeoutMsgAlternative->setKind(MSGKIND_SEND);
            scheduleAt(simTime(), timeoutMsgAlternative);
        } else {
            timeoutMsg->setKind(MSGKIND_SEND);
            std::cout << "schedule for renew" << endl;
            scheduleAt(simTime(), timeoutMsg);
        }
}

void SocketAgentAppTcp::socketDataArrived(inet::TcpSocket *socket,
                                          inet::Packet *msg, bool urgent) {
  std::cout << "socket data arrived: " << msg << endl;
  inet::Packet *copy = msg->dup();
  simtime_t delay;
  double delay_d = 0;

  packetsRcvd++;
  bytesRcvd += msg->getByteLength();
  emit(inet::packetReceivedSignal, msg);

  if (msg->getArrivalGate() == gate("socketIn")) {
    std::cout << "message is inet packet from network" << endl;

    inet::Packet *recv_packet = dynamic_cast<inet::Packet *>(copy);
    if (recv_packet != nullptr) {
      std::string reply_content = "0";
      TikTokPacket *answer = new TikTokPacket();

      // message is ACL message from other node in simulation over UDP socket
      // now handle reply to Mango
      inet::b offset = inet::b(0);  // start from the beginning
      bool found_acl_part = false;

      while (auto chunk = recv_packet->peekAt(offset)->dupShared()) {  // for each chunk
        if (found_acl_part) {
          delete recv_packet;
          break;
        }
        auto length = chunk->getChunkLength();

        if (chunk->getClassName() == std::string("MosaikMessage")) {
          reply_content =
              recv_packet->peekAt<MosaikMessage>(offset, length)->getContent();
          std::cout << "Observer SimTime " << observer->getSimulation()->getSimTime()
                              << endl;
          std::cout << "Creation time " << recv_packet->peekAt<MosaikMessage>(offset, length)->getCreationTime() << endl;
          simtime_t delay =
          observer->getSimulation()->getSimTime() - recv_packet->peekAt<MosaikMessage>(offset, length)->getCreationTime();
          double delay_d = round((delay).dbl()*1000)/1000;
          std::cout << "Delay " << delay << endl;
          std::cout << "Delay rounded " << delay_d << endl;
          std::cout << "reply content " << reply_content << endl;
          std::string reply_receiver =
              recv_packet->peekAt<MosaikMessage>(offset, length)->getReceiver();
          std::string reply_sender =
              recv_packet->peekAt<MosaikMessage>(offset, length)->getSender();
          answer->setContent(reply_content.c_str());
          answer->setReceiver(reply_receiver.c_str());
          answer->setDelay(delay_d);
          answer->setSender(reply_sender.c_str());
          found_acl_part = true;
          handleReply(answer);

        } else {
          offset += chunk->getChunkLength();
          answer->setDelay(delay_d);
        }
      }
      delete answer;
      delete msg;
      std::cout << "try to cancel event" << endl;
      if(timeoutMsg->isScheduled()) {
          timeoutMsgAlternative->setKind(MSGKIND_RENEW);
          scheduleAt(simTime(), timeoutMsgAlternative);
      } else {
          timeoutMsg->setKind(MSGKIND_RENEW);
          std::cout << "schedule for renew" << endl;
          scheduleAt(simTime(), timeoutMsg);
      }
    }
  }
}

void SocketAgentAppTcp::sendData() {
  if (packet != nullptr) {
    inet::Packet *sending = packet->dup();
    sending->setTimestamp(packet->getTimestamp());
    sendPacket(sending);
    packet = nullptr;
  } else {
    std::cout << "no packet to send" << endl;
  }
}

void SocketAgentAppTcp::socketClosed(inet::TcpSocket *socket) {
  std::cout << "socket closed " << endl;
  TcpAppBase::socketClosed(socket);
  cancelEvent(timeoutMsg);
  if (operationalState == State::STOPPING_OPERATION && !this->socket.isOpen())
    startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void SocketAgentAppTcp::socketFailure(inet::TcpSocket *socket, int code) {
  std::cout << "socket failure" << endl;
  TcpAppBase::socketFailure(socket, code);
  cancelEvent(timeoutMsg);
}

void SocketAgentAppTcp::handleStartOperation(
    inet::LifecycleOperation *operation) {
  const char *localAddress = par("localAddress");
  int localPort = par("localPort");
  connRecv = (connRecv + 1) % 1000;

  serverSocket.setOutputGate(gate("socketOut"));
  serverSocket.setCallback(this);
  serverSocket.bind(
      localAddress[0] ? inet::L3Address(localAddress) : inet::L3Address(),
      localPort + connRecv);
  serverSocket.listen();
}

void SocketAgentAppTcp::handleStopOperation(
  inet::LifecycleOperation *operation) {
  cancelEvent(timeoutMsg);
  if (socket.isOpen()) close();
  delayActiveOperationFinish(par("stopOperationTimeout"));
}

void SocketAgentAppTcp::handleCrashOperation(
  inet::LifecycleOperation *operation) {
  cancelEvent(timeoutMsg);
}

void SocketAgentAppTcp::finish() {
}
