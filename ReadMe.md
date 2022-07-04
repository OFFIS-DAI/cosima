# Integration of the (communication) simulator OMNeT++ into the co-simulation framework mosaik

mosaik is a Smart Grid co-simulation framework developed by OFFIS e.V. in Oldenburg. For further information on mosaik
see [mosaik](https://mosaik.offis.de). OMNeT++ is a simulation framework for (communication) network simulation. For
further information on OMNeT++ see [OMNeT++](https://omnetpp.org/). In this project we integrated the communication
simulator OMNeT++ into the co-simulation framework mosaik. This enables the simulation of realistic communication 
technologies (such as 5G) and the analysis of dynamic communication characteristics in Smart Grid scenarios.  

This ReadMe is structured as follows: 
* [Installation](#installation)
  * [Installation of python packages](#installation-of-python-packages)
  * [Installation and configuration of OMNeT++](#installation-and-configuration-of-omnet)
  * [Installation of protobuf](#installation-of-protobuf)
* [Scenario](#scenario)
* [Setup](#setup)
* [Run Simulation](#run-simulation)
* [Modelling new networks in OMNeT++](#modelling-a-new-network-in-omnet)
* [Optional: PyTests](#optional-pytests)
* [Optional: Google-Test and Google-Mock](#optional-google-test-and-google-mock)
* [Optional: clang-tidy and clang-format](#optional-clang-tidy-and-clang-format)

A detailed documentation can be found in the [documentation folder](documentation).  
This detailed documentation contains information about:
* General scenario configuration and synchronization
  * [Scenarios](documentation/Scenarios.md)
  * [Synchronization](documentation/Synchronization.md)
  * [MessageTypes](documentation/Message%20Types.md)
* Components in mosaik
  * [ComSim](documentation/CommSim.md)
  * [ICT-Controller](documentation/ICTController.md)
* Components in OMNeT++
  * [Networks](documentation/Networks.md)
  * [MosaikScheduler](documentation/MosaikScheduler.md)
  * [AgentApps](documentation/Agent%20Apps.md)


## Installation

To use the project, an installation of mosaik3.0, OMNeT++ and protobuf is required.

If you use ubuntu: 
You can use the [Installation Shell Script](install-requirements.sh) via
```bash
chmod +x install-requirements.sh
sudo ./install-requirements.sh
```
for the installation tasks.
Otherwise, follow the following instructions.

### Installation of python packages

Use the package manager [pip](https://pip.pypa.io/en/stable/) to install the requirements.txt file.

```bash
pip install -r requirements.txt
```

### Installation and configuration of OMNeT++

* Install OMNeT++ version 5.6.2 from their website [OMNeT++](https://omnetpp.org/).
    - call `.\configure` with `PREFER_CLANG=yes` and `CXXFLAGS=-std=c++14` for C++ version 14 
    - import the OMNeT++ files of this project [OMNeT++ files](cosima_omnetpp_project) as an Existing Project in
      OMNeT++
    - build the project
* Install INET 4.2.2 from the OMNeT++ website [OMNeT++ INET](https://omnetpp.org/).
    - set the INET installation under Project Properties -> Project References in this project
* (optional if you want to use LTE networks) install SimuLTE 1.2.0
    - set the SimuLTE installation under Project Properties -> Project References in this project
* adjust Makefile
    - under Project Properties -> OMNeT++ -> MakeMake adjust the Makemakefile of the source folder ("src:makemake")
      -- under Target: set "Executable"
      -- under Scope: set "Deep Compile", "Recursive make"
      -- under Compile: add path to INET installation and set "Add include Paths exported from referenced Projects"
      -- under Link: set both ticks. User interface libraries to link with: "all"
    - you should get a MakeMake option like --deep -I"C:\Users\user\Omnet-Projekt\inet" --meta:recurse --meta:
      use-exported-include-paths --meta:export-library --meta:use-exported-libs --meta:feature-ldflags

### Installation of protobuf

Install the protobuf compiler version 3.6.1 (on Ubuntu) via

```bash
sudo apt-get install libprotobuf-dev protobuf-compiler 
```

The used [protobuf message](message.proto) is already compiled. If you want to compile the .proto file or another one
use

```bash
protoc messages/message.proto --cpp_out=. --python_out=.
```

The output are the classes [message_pb2.py](message_pb2.py) for python and message.pb.cc and message.pb.h for C++. Put
the C++ files in the [OMNeT++ folder](cosima_omnetpp_project).

Now the generated files can be used in python via

```bash
from message_pb2 import NegotiationMsg
```

and in C++ via

```bash
#include "message.pb.h"
```

Now add the protobuf installation to your project in OMNeT++ under Project Properties -> OMNeT++ -> MakeMake -> Options
-> Link -> more -> additional objects to link with add "-lprotobuf".

## Scenario

Two example scenarios are included in the project. These are structured very simply and are intended to illustrate the
functionality of the integration. On the mosaik side, agent simulators,
a [communication simulator](cosima_core/simulators/comm_simulator.py) and a
[collector](cosima_core/simulators/collector.py) are implemented. In OMNeT++ you find
the [MosaikScheduler](cosima_omnetpp_project/modules/MosaikScheduler.h) which is responsible for message exchange with mosaik via
TCP socket. The [AgentApp](cosima_omnetpp_project/modules/AgentAppUdp.cc) and
[AgentAppTcp](cosima_omnetpp_project/modules/AgentAppTcp.cc) represent the implementation of the application layer
(and transport layer) of the end devices, which represent the agents from mosaik on the OMNeT++ side. Example networks
can also be found in the project folder. The executable file in OMNeT++
is [mosaik.ini](cosima_omnetpp_project/mosaik.ini). The integration is shown schematically in: ![image](./documentation/images/architecture.png)
When an agent in mosaik sends a message to another agent, it does so through the agent simulator entities. Thus,
client0 sends a message to client1 at time t1. However, this message is first received in the same step in
mosaik by the CommSim, which sends the message as a Protobuf object to OMNeT++ over a TCP connection. When the message
is sent, the simTime in mosaik and the value max_advance is passed from mosaik to OMNeT++. This value specifies how far
OMNeT++ may simulate until potentially new information could be available in mosaik. In OMNeT++ the message is received
by the MosaikScheduler, which extracts the message content and inserts it as an event into the FES at the given simTime
from mosaik. In addition, the value of max_advance is also inserted as an event. Now OMNeT++ simulates the message
dispatch from client0 to client1 over the INET network. The resulting delay time is sent back to the MosaikScheduler and
thus to mosaik. In mosaik the message is given to client1 after the determined end-to-end delay in OMNeT++.

In the following, the simulations are described, which can be performed by adapting the [config file](cosima_core/config.py).
From the given networks it is possible to simulate the scenario with 2 - 50 agents. If a correspondingly larger network 
is modeled, larger agent numbers are also possible.  
Furthermore, it is possible to simulate non-parallel and parallel message
sending behaviour. If parallel sending is simulated, two agents send messages simultaneously and time-shifted with 1 step difference.
It is also possible to add a PV plant to the simulation. PV plants can be connected to agents and read their current power values from
a given csv-file. Every 15 minutes, a new value is read and sent to the corresponding agent. The agent replies with an acknowledgement to 
this value.
Moreover, some changes in the infrastructure can be simulated. It is possible to set times for disconnects and reconnects for clients, routers
and switches.
In addition, long calculation times can be passed for the agents.
Multiple networks are implemented, for example to simulate TCP or UDP connections.

## Setup
Create a folder 'results' under mosaik-integration (mosaik-integration/results).
Before starting a simulation, choose simulation parameters:
- the used step size for the simulation (f.e. ms),
- the end time of the simulation (therefore the simulation duration),
- the number of agents,
- paths to store the simulation results and to load the content of the agent messages,
- the port to connect to OMNeT++,
- parallel or not,
- verbose or not to display information of the simulation run,
- the start mode,
- the simulated network, 
- agents to be connected to pv plants,
- infrastructure changes, 
- calculating times
- time-based or not (default is not time-based, as this is more performant).


## Run simulation
There exist different ways to run a simulation. In the [config](config.py), it is possible to choose from 'ide', 'qtenv' and 'cmd' as start mode.
* Ide: start the simulation in OMNeT++ by running [mosaik.ini](cosima_omnetpp_project/mosaik.ini) with your preferred network and 
  start the co-simulation in mosaik by running the [scenario](comm_scenario.py) 
* Qtenv: start mosaik by running the [scenario](comm_scenario.py). From within python, OMNeT++ will be started and a window will pop up in which
  the network can be chosen and the simulation can be started.
* Cmd: start mosaik by running the [scenario](comm_scenario.py). OMNeT++ will be started automatically as a console application. 
**(note: This only works properly if the project is compiled with clang)**

Simulation results
* The exchanged messages are stored in folder results with timestamp of the simulation start as name of the csv-file.

## Modelling a new network in OMNeT++
All of the provided [networks](cosima_omnetpp_project/networks) contain an instance of the 
*[MosaikSchedulerModule](cosima_omnetpp_project/modules/MosaikSchedulerModule.h)*. 
When modeling an additional network, this module must also be inserted into the network to enable the 
*[MosaikScheduler](cosima_omnetpp_project/modules/MosaikScheduler.h)* to operate. \
In addition, an instance of the *[MosaikScenarioManager](cosima_omnetpp_project/modules/MosaikScenarioManager.h)* 
should be inserted into the network if changes on the infrastructure (for example disconnecting or reconnecting clients) 
should be possible at simulation time. \
The network description file (.ned file) of a network in OMNeT++ should contain the following: 
```bash
import MosaikSchedulerModule;
import MosaikScenarioManager;

  scenarioManager: MosaikScenarioManager {
        }

  schedulerModule: MosaikSchedulerModule {
        }
```
If a new .ini-file in OMNeT++ is created, the scheduler class must be registered in the .ini-file of the OMNeT++-project.\
``
[General] 
``\
``
scheduler-class = "MosaikScheduler"
`` 

## Optional: PyTests

To set up testing in python follow these steps:

* in PyCharm under Settings -> Python Integrated Tools -> Testing set the Default Test Runner to PyTest
* tests can be found in the [test folder](cosima_core/tests). The structure of the test folder should correspond to the structure of
  the project
* test files start with "test_"
* to run a test execute either a single file or the folder

For further information see [info](https://semaphoreci.com/community/tutorials/testing-python-applications-with-pytest).

## Optional: Google-Test and Google-Mock
In the [test folder in OMNeT++](cosima_omnetpp_project/tests) the unit tests for OMNeT++ can be found. 
The framework Google Test with Google Mock is used for implementation 
(see [googletest](https://github.com/google/googletest/blob/main/googletest/README.md)).
For installing Google Test:
```bash
git clone https://github.com/google/googletest.git -b release-1.11.0
cd googletest
mkdir build
cd build
cmake ..
make
sudo make install 
```
or
```bash
sudo apt-get install googletest 
cd /usr/src/googletest   
sudo cmake CMakeLists.tx
sudo make
sudo make install
```
For executing the tests the following steps must be executed:
* include  [test folder in OMNeT++](cosima_omnetpp_project/tests) in build
* under "Project Properties -> OMNeT++ -> Makemake -> Options -> Link -> Additional objects to link with" add the 
  following dependencies: 
  * -lgmock -lgtest -lpthread
* rebuild project 
* when executing, now the test files will be executed 

## Optional: clang-tidy and clang-format

clang-tidy and clang-formal are tools to clean up C++ code. To install, configure and use them do the following:

* choose the right version for your system on [installation page](https://apt.llvm.org/)
* run sudo apt-add-repository "<version>"
* run wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -# Fingerprint: 6084 F3CF 814B 57C1 CF12
  EFD5 15CF 4D18 AF4F 7421
* run sudo apt-get update
* run sudo apt-get install clang-tidy-3.9
* navigate to the OMNeT++ folder
* generate a compilation database with clang++ -MJ compile_commands.o.json -Wall -std=c++11 -o <Dateipfad>.o
  -c <Dateipfad>.cpp
* generate a file with sed -e '1s/^/[\n/' -e '$s/,$/\n]/' *.o.json > compile_commands.json
* print all checks with clang-tidy-3.9 -checks=* -p ./ <Dateiname>.cc
* run sudo apt install clang-format-3.9
* run clang-format-3.9 -style=google <filename>.cc
