#include "AttackIpv4.h"
#include "../../../messages/AttackEvent_m.h"
#include "../../../messages/MosaikApplicationChunk_m.h"


Define_Module(AttackIpv4);


void AttackIpv4::initialize(int stage) {
    Ipv4::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {

        // get intern socket scheduler from simulation and cast to MosaikScheduler
        scheduler =
                check_and_cast<MosaikScheduler *>(getSimulation()->getScheduler());

        // register module at scheduler
        scheduler->setAttackNetworkLayer(this);
    }
}

void AttackIpv4::handleMessageWhenUp(cMessage *msg) {
    bool attackSuccess = false;

    if (typeid(*msg) == typeid(AttackEvent)) {
        AttackEvent *event = dynamic_cast<AttackEvent *>(msg);
        auto attackType = event->getAttackType();
        auto attackProbablitiy = event->getProbability();
        auto attackEndTime = event->getStop();
        if (attackType == AttackType::PacketDrop) {
            droppingAttackIsActive = true;
            droppingAttackProbability = attackProbablitiy;
            droppingAttackEndTime = attackEndTime/1000;
        } else if (attackType == AttackType::PacketDelay) {
            delayAttackIsActive = true;
            delayAttackProbability = attackProbablitiy;
            delayAttackEndTime = attackEndTime/1000;
        } else if (attackType == AttackType::PacketFalsification) {
            falsificationAttackIsActive = true;
            falsificationAttackProbability = attackProbablitiy;
            falsificationAttackEndTime = attackEndTime/1000;
        }
    } else {
        if (droppingAttackIsActive) {
            if (droppingAttackEndTime < simTime().dbl()) {
                double randomNumber = rand()/double(RAND_MAX);
                if (randomNumber <= droppingAttackProbability) {
                    scheduler->log("Drop message. ", "info");
                    numDropped = numDropped + 1;
                    delete msg;
                    attackSuccess = true;
                }
            } else {
                droppingAttackIsActive = false;
                droppingAttackEndTime = 0;
                droppingAttackProbability = 0;
            }
        }
        if (delayAttackIsActive) {
            if (delayAttackEndTime < simTime().dbl()) {
                double randomNumber = rand()/double(RAND_MAX);
                if (randomNumber <= delayAttackProbability) {
                    scheduler->log("Delay message. ", "info");
                    numDelays = numDelays + 1;
                    auto newMsg = msg->dup();
                    delete msg;
                    scheduleAt(simTime()+10, newMsg);
                    attackSuccess = true;
                }
            } else {
                delayAttackIsActive = false;
                delayAttackEndTime = 0;
                delayAttackProbability = 0;
            }
        }
        if (falsificationAttackIsActive) {
            if (falsificationAttackEndTime < simTime().dbl()) {
                double randomNumber = rand()/double(RAND_MAX);
                if (randomNumber <= falsificationAttackProbability) {
                    auto copy = msg->dup();
                    inet::Packet *recvPacket = dynamic_cast<inet::Packet *>(copy);
                    inet::b offset = inet::b(0);
                    auto foundApplicationChunk = false;
                    while (auto chunk = recvPacket->peekAt(offset)->dupShared()) {  // for each chunk
                        if (foundApplicationChunk) {
                            break;
                        }
                        auto length = chunk->getChunkLength();

                        if (chunk->getClassName() == std::string("MosaikApplicationChunk")) {
                            auto mosaikApplicationChunk = recvPacket->peekAt<MosaikApplicationChunk>(offset, length, inet::Chunk::PeekFlag::PF_ALLOW_SERIALIZATION).get();
                            scheduler->log("Falsification of the message. ", "info");
                            numFalsifictions = numFalsifictions + 1;
                            const auto &payload = inet::makeShared<MosaikApplicationChunk>();
                            payload->setContent(mosaikApplicationChunk->getContent());
                            payload->setReceiver(mosaikApplicationChunk->getReceiver());
                            payload->setSender(mosaikApplicationChunk->getSender());
                            payload->setChunkLength(mosaikApplicationChunk->getChunkLength());
                            payload->setCreationTime(mosaikApplicationChunk->getCreationTime());
                            payload->setMsgId(mosaikApplicationChunk->getMsgId());
                            payload->setCreationTimeMosaik(mosaikApplicationChunk->getCreationTimeMosaik());
                            payload->setIsTrafficMessage(mosaikApplicationChunk->isTrafficMessage());
                            payload->setIsFalsified(true);

                            recvPacket->eraseAtBack(length);
                            recvPacket->insertAtBack(payload);

                            Ipv4::handleMessageWhenUp(recvPacket);

                            attackSuccess = true;

                            foundApplicationChunk = true;
                        } else {
                            offset += chunk->getChunkLength();
                            if (offset >= recvPacket->getTotalLength()) {
                                delete copy;
                                break;
                            }

                        }

                    }
                }
            }
        }
        if (not attackSuccess) {
            Ipv4::handleMessageWhenUp(msg);
        } else {
            auto notification_message = new MosaikSchedulerMessage();
            notification_message->setTransmission_error(true);
            scheduler->sendToMosaik(notification_message);
        }
    }

}

void AttackIpv4::finish() {
    std::string nameStr = this->getParentModule()->getParentModule()->getName();
    scheduler->log("AttackIpv4 attack statistics of " + nameStr + ": number of delays: " + std::to_string(numDelays) +
            ", number of dropped messages: " + std::to_string(numDropped) +
            ", number of falsified messages: " + std::to_string(numFalsifictions), "info");
}
