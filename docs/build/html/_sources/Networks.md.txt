# OMNeT++ networks
As a starting point for own scenarios some networks are provided in the project, which contain different communication technologies and topologies. \
The network description files can be found in the [networks folder](https://gitlab.com/mosaik/examples/cosima/-/tree/master/cosima_omnetpp_project/networks). 

## Provided networks

### *WinzentNetwork*
The *[WinzentNetwork](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/networks/WinzentNetwork.ned)* consists of five routers, which form the core network. Four switches, each with one end device, are connected to the core network. The connections are wired Ethernet connections. The data rate of the [EtherLink](https://doc.omnetpp.org/inet/api-old/neddoc/index.html?p=inet.node.ethernet.EtherLink.html) is set to 10Mbps. The distances between the individual network devices are defined manually, but can be easily adjusted by modifying the .ned file. In the network description file five networks are modeled in total, which are modifications of the initial *WinzentNetwork*. 

### *ManyAgentNetwork*
The *[ManyAgentNetwork](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/networks/ManyAgentNetwork.ned)* consists of two ring-shaped core networks of routers. These are connected to 20 end devices via switches. 
As in the *WinzentNetwork*, the communication technology is based on Ethernet cables. 

### *LargeCosimaNetwork*
The [LargeCosimaNetwork](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/networks/LargeCosimaNetwork.ned) consists of multiple rings of routers connected via Ethernet cables. 50 end devices are modelled.

### *MosaikNetwork*
In the [MosaikNetwork](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/networks/MosaikNetwork.ned), an [InternetCloud](https://doc.omnetpp.org/inet/api-current/neddoc/src-inet-node-internetcloud-InternetCloud.ned.html) from the inet library is used. It is used here to model a fixed delay time between the end devices. Ethernet cables with a data rate of 100Mbps are modeled as connections.

### *SingleCellLTE*
The [SingleCellLTE](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/networks/SingleCellLTE.ned) network is based on the basic network of the SimuLTE library. The tutorial can be found under [SimuLTE tutorial](https://simulte.com/tutorial-basic.html). \
The network is, as the name suggests, a LTE network. Four devices are within range of a base station and can thus communicate via the wireless network. 

### *WifiNetwork*
Another wireless network is the [WifiNetwork](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/networks/WifiNetwork.ned). Communication here takes place via Wifi.

## Modelling a new network
All of the previously described, provided networks contain an instance of the *[MosaikSchedulerModule](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/modules/MosaikSchedulerModule.h)*. 
When modeling an additional network, this module must also be inserted into the network to enable the *[MosaikScheduler](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/modules/MosaikScheduler.h)* to operate. For further information also see the [documentation on the MosaikScheduler](MosaikScheduler.md). \
In addition, an instance of the *[MosaikScenarioManager](https://gitlab.com/mosaik/examples/cosima/-/blob/master/cosima_omnetpp_project/modules/MosaikScenarioManager.h)* should be inserted into the network if changes on the infrastructure (for example disconnecting or reconnecting clients) should be possible at simulation time. For further information on infrastructure changes during simulation time see the [documentation on the ICTController](ICTController.md).
