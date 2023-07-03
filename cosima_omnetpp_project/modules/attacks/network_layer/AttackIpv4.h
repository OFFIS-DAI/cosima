//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2014 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __ATTACKIPV4_H
#define __ATTACKIPV4_H

#pragma once

#include "inet/networklayer/ipv4/Ipv4.h"

#include "../../MosaikScheduler.h"

using namespace inet;

using namespace omnetpp;

class AttackIpv4 : public Ipv4 {
public:
    AttackIpv4() {
    }
    MosaikScheduler *scheduler;

    virtual void handleMessageWhenUp(cMessage *msg) override;


    /**
     * Method from cSimpleModule class, to initialize the simple module.
     * Overridden function.
     */
    virtual void initialize(int stage) override;

    virtual void finish() override;

private:
    // Dropping attack initialization
    int numDrops = 0;
    double droppingAttackProbability = 0;
    bool droppingAttackIsActive = false;
    double droppingAttackEndTime = 0.0;

    // Delay attack initialization
    int numDelays = 0;
    double delayAttackProbability = 0;
    bool delayAttackIsActive = false;
    double delayAttackEndTime = 0.0;

    // Falsification attack initialization
    int numFalsifictions = 0;
    double falsificationAttackProbability = 0;
    bool falsificationAttackIsActive = false;
    double falsificationAttackEndTime = 0.0;

};
#endif

