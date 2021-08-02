/*
 * SocketAgentApp.cc
 *
 *  Created on: 11 Apr 2021
 *      Author: malin
 */
#include <string.h>
#include <omnetpp.h>
#include <omnetpp/platdep/sockets.h>
#include "MosaikObserver.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "MosaikMessage_m.h"
#include "TikTokPacket_m.h"
#include <algorithm>

using namespace omnetpp;

class SocketAgentApp : public cSimpleModule {
 private:
  MosaikObserver *observer;

  inet::UdpSocket socketudp;

  int numRecvBytes;

 public:
  SocketAgentApp();
  virtual ~SocketAgentApp();
  char recvBuffer[4000];

 protected:
  void initialize(int stage) override;
  int numInitStages() const override { return (inet::NUM_INIT_STAGES); }
  void handleMessage(cMessage *msg) override;
  void handleSocketEvent(cMessage *msg);
  void handleReply(TikTokPacket *reply);  // const char *reply
};

Define_Module(SocketAgentApp);

SocketAgentApp::SocketAgentApp() {
  numRecvBytes = 0;
  observer = nullptr;
}

SocketAgentApp::~SocketAgentApp() = default;

void SocketAgentApp::initialize(int stage) {
  cSimpleModule::initialize(stage);
  if (stage != inet::INITSTAGE_APPLICATION_LAYER) return;

  // get intern socket scheduler from simulation and cast to cSocketobserver
  observer = check_and_cast<MosaikObserver *>(getSimulation()->getScheduler());
  // register module at scheduler
  observer->setInterfaceModule(this, recvBuffer, 4000, &numRecvBytes);
  // initialize socket in simulation in OMNeT++ and bind to local port
  // local port can be set in .ini file
  socketudp.setOutputGate(gate("socketOut"));
  int localPort = par("localPort");
  socketudp.bind(localPort);
}

void SocketAgentApp::handleMessage(cMessage *msg) {
  // message can be socket event -> call handleSocketEvent()
  // or message from network -> handle message
  std::cout << "SocketAgentApp of " << this->getParentModule()->getName()
            << " received message " << msg << endl;

  if (msg->getArrivalGate() == gate("socketIn")) {
    std::cout << "message is inet packet from network" << endl;

    inet::Packet *packet = dynamic_cast<inet::Packet *>(msg);
    if (packet != nullptr) {
      std::string reply_content = "0";
      TikTokPacket *answer = new TikTokPacket();

      std::cout << "Observer SimTime "
                << observer->getSimulation()->getSimTime() << endl;
      std::cout << "Creation time " << msg->getCreationTime() << endl;
      simtime_t delay =
          observer->getSimulation()->getSimTime() - msg->getCreationTime();
      std::cout << "Delay " << delay << endl;
      double delay_d = (delay).dbl();

      // message is ACL message from other node in simulation over UDP socket
      // now handle reply to Mango
      inet::b offset = inet::b(0);  // start from the beginning
      bool found_acl_part = false;

      while (auto chunk =
                 packet->peekAt(offset)->dupShared()) {  // for each chunk
        if (found_acl_part) {
          // message is from other SocketAgent
          delete packet;
          break;
        }
        auto length = chunk->getChunkLength();

        if (chunk->getClassName() == std::string("MosaikMessage")) {
          reply_content =
              packet->peekAt<MosaikMessage>(offset, length)->getContent();
          std::cout << "reply content " << reply_content << endl;
          // reply.pop_back();
          std::string reply_receiver =
              packet->peekAt<MosaikMessage>(offset, length)->getReceiver();
          std::string reply_sender =
              packet->peekAt<MosaikMessage>(offset, length)->getSender();
          answer->setContent(reply_content.c_str());
          answer->setReceiver(reply_receiver.c_str());
          answer->setDelay(delay_d);
          answer->setSender(reply_sender.c_str());
          found_acl_part = true;


        } else {
          offset += chunk->getChunkLength();
          answer->setDelay(delay_d);
        }
      }
      handleReply(answer);
    }

  } else {
    // message is placed from observer
    handleSocketEvent(msg);
  }
}

void SocketAgentApp::handleSocketEvent(cMessage *msg) {
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
    std::string senderName = tmsg->getSender();
    int msgSize = tmsg->getSize();

    // get corresponding Port for receiver name
    int receiverPort = observer->getPortForModule(receiverName.c_str());

    std::cout << "Message receiver: " << receiverName
              << ", msgSize: " << msgSize << ", receiverPort: " << receiverPort
              << endl;

    inet::Packet *packet = new inet::Packet();
    const auto &payload = inet::makeShared<MosaikMessage>();
    payload->setContent(content);
    payload->setReceiver(receiverName.c_str());
    payload->setSender(senderName.c_str());
    payload->setChunkLength(inet::B(msgSize));
    payload->setCreationTime(simTime());
    packet->insertAtBack(payload);

    // get destination
    inet::L3Address destAddress;
    destAddress = inet::L3AddressResolver().resolve(receiverName.c_str());

    // send packet
    socketudp.sendTo(packet, destAddress, receiverPort);
    std::cout << "send packet " << packet << " to port " << receiverPort
              << endl;
  }
}

void SocketAgentApp::handleReply(TikTokPacket *reply) {
  std::cout << "Set message for observer " << reply << endl;
  observer->sendBytes(reply);
}
