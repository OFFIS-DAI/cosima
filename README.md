# cosima: Integration of the (communication) simulator OMNeT++ into the co-simulation framework mosaik

mosaik is a Smart Grid co-simulation framework developed by OFFIS e.V. in Oldenburg. For further information on mosaik
see [mosaik](https://mosaik.offis.de). OMNeT++ is a simulation framework for (communication) network simulation. For
further information on OMNeT++ see [OMNeT++](https://omnetpp.org/). In this project we integrated the communication
simulator OMNeT++ into the co-simulation framework mosaik. This enables the simulation of realistic communication 
technologies (such as 5G) and the analysis of dynamic communication characteristics in Smart Grid scenarios.  

This README contains the basic instructions on how to [install](#installation) cosima and how to [run the simulation](#run-simulation).
We recommend reading the detailled documentation on [readthedocs](https://cosima.readthedocs.io).

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
from message_pb2 import InfoMessage
```

and in C++ via

```bash
#include "message.pb.h"
```

Now add the protobuf installation to your project in OMNeT++ under Project Properties -> OMNeT++ -> MakeMake -> Options
-> Link -> more -> additional objects to link with add "-lprotobuf".


## Run simulation
There exist different ways to run a simulation. In the [scenario configuration file](scenario_config.py), it is possible to choose from 'ide', 'qtenv' and 'cmd' as start mode.
* Ide: start the simulation in OMNeT++ by running [mosaik.ini](cosima_omnetpp_project/mosaik.ini) with your preferred network and 
  start the co-simulation in mosaik by running the [scenario](cosima_core/scenarios/communication_scenario.py) 
* Qtenv: start mosaik by running the [scenario](cosima_core/scenarios/communication_scenario.py). From within python, OMNeT++ will be started and a window will pop up in which
  the network can be chosen and the simulation can be started.
* Cmd: start mosaik by running the [scenario](cosima_core/scenarios/communication_scenario.py). OMNeT++ will be started automatically as a console application. 
**(note: This only works properly if the project is compiled with clang)**

Simulation results
* The exchanged messages are stored in folder results with timestamp of the simulation start as name of the csv-file.
For this, you may need to create a folder 'results' under mosaik-integration (mosaik-integration/results).
