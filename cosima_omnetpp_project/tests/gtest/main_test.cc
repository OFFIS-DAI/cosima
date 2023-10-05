/*
 * main_test.cc
 *
 *  Created on: Feb 6, 2022
 *      Author: malin
 *  Basic code concept from https://github.com/roVer-HM/crownet/tree/master/crownet/tests/gtest/src.
 */
#include "main_test.h"

using namespace omnetpp;

int main(int argc, char **argv) {
  ::testing::InitGoogleMock(&argc, argv);

  // the following line MUST be at the top of main()
  cStaticFlag dummy;
  // initializations
  CodeFragments::executeAll(CodeFragments::STARTUP);
  SimTime::setScaleExp(-12);

  // create a simulation manager and an environment for the simulation
  cEnvir *env = new GtestEnv(argc, argv, new EmptyConfig());
  cSimulation *sim = new cSimulation("simulation", env);
  cSimulation::setActiveSimulation(sim);

  CosimaScheduler *scheduler = new CosimaScheduler();
  sim->setScheduler(scheduler);
  cEventHeap *fes = new cEventHeap();
  sim->setFES(fes);

  int ret = RUN_ALL_TESTS();

  // deallocate registration lists, loaded NED files, etc.
  CodeFragments::executeAll(CodeFragments::SHUTDOWN);
  cSimulation::setActiveSimulation(nullptr);
  delete sim;  // deletes env as well
  return ret;
}



