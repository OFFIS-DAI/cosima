# cosima: Integration of the (communication) simulator OMNeT++ into the co-simulation framework mosaik

mosaik is a Smart Grid co-simulation framework developed by OFFIS e.V. in Oldenburg. For further information on mosaik
see [mosaik](https://mosaik.offis.de). OMNeT++ is a simulation framework for (communication) network simulation. For
further information on OMNeT++ see [OMNeT++](https://omnetpp.org/). In this project we integrated the communication
simulator OMNeT++ into the co-simulation framework mosaik. This enables the simulation of realistic communication 
technologies (such as 5G) and the analysis of dynamic communication characteristics in Smart Grid scenarios.  

This README contains the basic instructions on how to [install](#installation) cosima and how to [run the simulation](#run-simulation).
We recommend reading the detailled documentation on [readthedocs](https://cosima.readthedocs.io).

## Installation

To use the project, an installation of mosaik (at least version 3.0), OMNeT++ and protobuf are required.

If you use Ubuntu: 
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
In order to install OMNeT++ in version 5.6.2, go to their 
[website](https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.6.2/omnetpp-5.6.2-src-linux.tgz) and follow the 
instructions for your operating system.
In case that you are using Ubuntu (which is recommended), you can also execute the following commands: 

**Go to home directory**
```bash
cd ~
```
**Create working directory and enter directory**
```bash
mkdir -p omnetpp
cd omnetpp
```
**Fetch OMNeT++ source (in version 5.6.2)**
```bash
wget https://github.com/omnetpp/omnetpp/releases/download/omnetpp-5.6.2/omnetpp-5.6.2-src-linux.tgz
tar -xf omnetpp-5.6.2-src-linux.tgz
```
**Export path**
```bash
export PATH=$PATH:/usr/omnetpp/omnetpp-5.6.2/bin
```
**Configure and compile**
```bash
cd omnetpp-5.6.2
./configure PREFER_CLANG=yes
make
```
**Installation and configuration of INET**\
Install INET 4.2.2 from the OMNeT++ website [OMNeT++ INET](https://omnetpp.org/). 
The corresponding release can be found under this [link](https://github.com/inet-framework/inet/releases/download/v4.2.2/inet-4.2.2-src.tgz).

Again, under Ubuntu you can navigate to your preferred working directory and execute the following commands:
```bash
wget https://github.com/inet-framework/inet/releases/download/v4.2.2/inet-4.2.2-src.tgz
tar -xzf inet-4.2.2-src.tgz
rm inet-4.2.2-src.tgz
mv inet4 inet
cd inet
make makefiles
 ```
After your installation, open the OMNeT++ IDE via
```bash
omnetpp
```
In the IDE, you can import the [OMNeT++ files](cosima_omnetpp_project) of this project as an Existing 
Project under\
_File->Import->General->Existing Projects_\
into Workspace.\
Then choose the INET installation directory as a project reference under\
_Project Properties -> Project References_\
in this project.

Now build your project:\
_Project->Build Project_


**(Optional) if you want to use LTE networks: Install SimuLTE 1.2.0** 
Install [SimuLTE in the correct version](https://github.com/inet-framework/simulte/releases/download/v1.2.0/simulte-1.2.0-src.tgz) 
and set the SimuLTE installation directory under\
_Project Properties -> Project References_\
in this project and re-build your project.

After installing all libraries check (and maybe adjust) the Makefile of the project.
Under

_Project Properties -> OMNeT++ -> MakeMake_\
adjust the makemakefile of the source folder ("src:makemake")
- under Target: set "Executable"
- under Scope: set "Deep Compile", "Recursive make" 
- under Compile: add path to INET installation and set "Add include Paths exported from referenced Projects"
- under Link: set both ticks. User interface libraries to link with: "all"

You should get a MakeMake option like 
```bash
--deep -I"C:\Users\user\Omnet-Projekt\inet" --meta:recurse --meta:use-exported-include-paths --meta:export-library --meta:use-exported-libs --meta:feature-ldflags
```

### Installation of protobuf

Install the protobuf compiler version 3.6.1 (on Ubuntu) via

```bash
sudo apt-get install libprotobuf-dev protobuf-compiler 
```

The used [protobuf message](cosima_core/messages/message.proto) is already compiled. If you want to compile the .proto file or another one
use

```bash
protoc cosima_core/messages/message.proto --cpp_out=. --python_out=.
```

The output are the classes [message_pb2.py](cosima_core/messages/message_pb2.py) for python and message.pb.cc and message.pb.h for C++. Put
the C++ files in the [OMNeT++ folder](cosima_omnetpp_project).

Now the generated files can be used in python via

```bash
from message_pb2 import InfoMessage
```

and in C++ via

```bash
#include "message.pb.h"
```

Now add the protobuf installation to your project in OMNeT++ under\
_Project Properties -> OMNeT++ -> MakeMake -> Options -> Link -> more -> additional objects to link with_\
add "-lprotobuf".

## Optional: PyTests

To set up testing in python follow these steps:

* in PyCharm under Settings -> Python Integrated Tools -> Testing set the Default Test Runner to PyTest
* tests can be found in the [test folder](tests). The structure of the test folder should correspond to the structure of
  the project
* test files start with "test_"
* to run a test execute either a single file or the folder

For further information see [info](https://semaphoreci.com/community/tutorials/testing-python-applications-with-pytest).

## Optional: Google-Test and Google-Mock
The process of testing is performed with the unit-testing Framework [Google Test](https://github.com/google/googletest/blob/main/googletest/README.md) with Google Mock. 
It is generally used to test different functionalities of the cosima project like the AgentApps, the MosaikScenarioManager or the Scheduler Module. 
Because the tests do not include the functionalities of the Networks itself, there is no need to execute them, when implementing a new Network. 
For this reason, the tests are excluded from being executed in OMNeT++ by default.

### Installing Google Test
The following commands can be used to install the Google Test framework under a Linux distribution.
Google Test must be installed, to run any of the tests but is not necessary to execute the project itself.
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

### Include the Tests and execute them
Executing the tests is possible by doing the following steps...

1. Including the tests folder
To perform Tests for the cosima project first you have to include them in the OMNeT++ Framework. 
To do so, right-click on the folder [tests](cosima_omnetpp_project/tests) and then go to Resource Configuration -> Exclude from Build. 
In the newly opened window you now can uncheck the boxes for the configuration, that you want to perform the tests on. 
By default, both the configuration for the debug and release mode should be checked.

![excluding of folders](docs/source/images/exclude.PNG)

2. Update dependencies
Next up we have to make sure, that the compiler uses the Google Test libraries, when executinmg the project. 
Under the Project Properties of our cosima_omnetpp_project, we have to click on OMNeT++ -> Makemake -> Options -> Link -> Additional objects to link with. 
Here you have to add the following dependencies (-lgmock -lgtest -lpthread). 
Now all that is left, is to rebuild the project and then executing it. The Tests should now be performed with the rest of the project.

![updating dependencies](docs/source/images/makemake.PNG)


## Run simulation
In order to run a simulation, you have to execute a scenario from the [scenarios folder](cosima_core/scenarios).

There exist different ways to run a simulation. In the [scenario configuration file](scenario_config.py), it is possible to choose from 'ide', 'qtenv' and 'cmd' as start mode.
* Ide: start the simulation in OMNeT++ by running [mosaik.ini](cosima_omnetpp_project/mosaik.ini) with your preferred network and 
  start the co-simulation in mosaik by running the [scenario](cosima_core/scenarios/communication_scenario.py) 
* Qtenv: start mosaik by running the [scenario](cosima_core/scenarios/communication_scenario.py). From within python, OMNeT++ will be started and a window will pop up in which
  the network can be chosen and the simulation can be started.
* Cmd: start mosaik by running the [scenario](cosima_core/scenarios/communication_scenario.py). OMNeT++ will be started automatically as a console application. 
**(note: This only works properly if the project is compiled with clang)**

**Simulation results**
* The exchanged messages are stored in folder results with timestamp of the simulation start as name of the csv-file.
For this, you may need to create a [results folder](cosima_core/results).