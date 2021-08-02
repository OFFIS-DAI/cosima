//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
#include <string.h>
#include <omnetpp.h>
#include "MosaikObserver.h"
#include "MosaikObserverModule.h"
#include "TikTokPacket_m.h"

Define_Module(MosaikObserverModule);

MosaikObserverModule::MosaikObserverModule() {
    observer = nullptr;
    max_adv_event = new MosaikCtrlMsg("hello max advanced");
}

MosaikObserverModule::~MosaikObserverModule() {
    cancelAndDelete(max_adv_event);
}


void MosaikObserverModule::initialize(int stage){
//        cSimpleModule::initialize(stage);
      observer = check_and_cast<MosaikObserver *>(getSimulation()->getScheduler());
      // register module at scheduler
      observer->setInterfaceModule(this, nullptr, 0, nullptr);
      std::cout << "MosaikObserverModule: enter init method" << endl;
}

void MosaikObserverModule::handleMessage(cMessage *msg){
    std::cout << "MosaikObserverModule: "<< msg << endl;

    std::cout << "MosaikObserverModule: Set message for observer " << msg << endl;
    observer->sendBytes(msg);
}

void MosaikObserverModule::cancelMaxAdvanceEvent() {
    cancelEvent(max_adv_event);
}
