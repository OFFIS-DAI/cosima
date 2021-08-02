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

#ifndef MOSAIKOBSERVERMODULE_H_
#define MOSAIKOBSERVERMODULE_H_

#include <string.h>
#include <omnetpp.h>
#include "MosaikObserver.h"
#include "MosaikCtrlMsg_m.h"

using namespace omnetpp;

class MosaikObserverModule: public cSimpleModule {
private:
 MosaikObserver *observer;

public:
    MosaikObserverModule();
    virtual ~MosaikObserverModule();
    MosaikCtrlMsg* max_adv_event;
    virtual void cancelMaxAdvanceEvent();

protected:
  // The following redefined virtual function holds the algorithm.
  virtual void initialize(int stage) override;
  virtual void handleMessage(cMessage *msg) override;

};

#endif /* MOSAIKOBSERVERMODULE_H_ */
