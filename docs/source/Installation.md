# Installation

To use the project, an installation of mosaik3.0, OMNeT++ and protobuf is required.

If you use ubuntu: 
You can use the [Installation Shell Script](../../install-requirements.sh) via
```bash
chmod +x install-requirements.sh
sudo ./install-requirements.sh
```
for the installation tasks.
Otherwise, follow the following instructions.

## Installation of python packages

Use the package manager [pip](https://pip.pypa.io/en/stable/) to install the requirements.txt file.

```bash
pip install -r requirements.txt
```

## Installation and configuration of OMNeT++

* Install OMNeT++ version 5.6.2 from their website [OMNeT++](https://omnetpp.org/).
    - call `.\configure` with `PREFER_CLANG=yes` and `CXXFLAGS=-std=c++14` for C++ version 14 
    - import the OMNeT++ files of this project [OMNeT++ files](../../cosima_omnetpp_project) as an Existing Project in
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

## Installation of protobuf

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
the C++ files in the [OMNeT++ folder](../../cosima_omnetpp_project).

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


## Optional: PyTests

To set up testing in python follow these steps:

* in PyCharm under Settings -> Python Integrated Tools -> Testing set the Default Test Runner to PyTest
* tests can be found in the [test folder](../../tests). The structure of the test folder should correspond to the structure of
  the project
* test files start with "test_"
* to run a test execute either a single file or the folder

For further information see [info](https://semaphoreci.com/community/tutorials/testing-python-applications-with-pytest).

## Optional: Google-Test and Google-Mock
In the [test folder in OMNeT++](../../cosima_omnetpp_project/tests) the unit tests for OMNeT++ can be found. 
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
* include  [test folder in OMNeT++](../../cosima_omnetpp_project/tests) in build
* under "Project Properties -> OMNeT++ -> Makemake -> Options -> Link -> Additional objects to link with" add the 
  following dependencies: 
  * -lgmock -lgtest -lpthread
* rebuild project 
* when executing, now the test files will be executed 
