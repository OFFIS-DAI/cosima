package networks;
// 
//                           SimuLTE
// 
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself, 
// and cannot be removed from it.
// 

import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth10M;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import lte.corenetwork.binder.LteBinder;
import lte.corenetwork.nodes.Ue;
import lte.corenetwork.nodes.eNodeB;
import lte.world.radio.LteChannelControl;
import lte.epc.PgwStandardSimplified;
import lte.common.LteNetworkConfigurator;
import inet.node.inet.StandardHost;

import modules.MosaikScenarioManager;
import modules.MosaikSchedulerModule;

network LargeLTENetwork
{
    parameters:
        @display("i=block/network2;bgb=464,328;bgi=background/terrain;bgu=m");
    submodules:
        scenarioManager: MosaikScenarioManager {
            @display("p=65,146;i=abstract/logo_icon_small");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=65,111;i=abstract/logo_icon_small");
        }
        channelControl: LteChannelControl {
            @display("p=47,34;is=s");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=110,33;is=s");
        }
        configurator: LteNetworkConfigurator {
            @display("p=47,66;is=s");
        }
        binder: LteBinder {
            @display("p=110,66;is=s");
        }
        router: Router {
            @display("p=301,59;i=device/smallrouter");
        }
        pgw: PgwStandardSimplified {
            nodeType = "PGW";
            @display("p=269,88");
        }
        eNB: eNodeB {
            @display("p=352,123;is=n");
        }
        client0: Ue {
            @display("p=353,184;i=abstract/agent");
        }


        eNB1: eNodeB {
            @display("p=223,133;is=n");
        }
        client1: StandardHost {
            @display("p=307.69235,87.573975;i=abstract/agent");
        }


        client2: StandardHost {
            @display("p=332.5444,79.28995;i=abstract/agent");
        }


        client3: StandardHost {
            @display("p=286.98227,87.573975;i=abstract/agent");
        }


        client4: Ue {
            @display("p=353,184;i=abstract/agent");
        }


        client5: Ue {
            @display("p=353,184;i=abstract/agent");
        }


        client6: Ue {
            @display("p=340,172;i=abstract/agent");
        }


        client7: Ue {
            @display("p=326,202;i=abstract/agent");
        }


        client8: Ue {
            @display("p=372,140;i=abstract/agent");
        }


        client9: Ue {
            @display("p=379,184;i=abstract/agent");
        }


        client10: Ue {
            @display("p=294,106;i=abstract/agent");
        }


        client11: Ue {
            @display("p=294,184;i=abstract/agent");
        }


        client12: Ue {
            @display("p=287,160;i=abstract/agent");
        }


        client13: Ue {
            @display("p=294,133;i=abstract/agent");
        }


        client14: Ue {
            @display("p=340,99;i=abstract/agent");
        }


        client15: Ue {
            @display("p=372,106;i=abstract/agent");
        }


        client16: Ue {
            @display("p=333,145;i=abstract/agent");
        }


        client17: Ue {
            @display("p=402,153;i=abstract/agent");
        }


        client18: Ue {
            @display("p=353,209;i=abstract/agent");
        }


        client19: Ue {
            @display("p=385,191;i=abstract/agent");
        }


        client20: Ue {
            @display("p=385,123;i=abstract/agent");
        }


        client21: Ue {
            @display("p=326,123;i=abstract/agent");
        }


        client22: Ue {
            @display("p=326,184;i=abstract/agent");
        }


        client23: Ue {
            @display("p=311,160;i=abstract/agent");
        }


        client24: Ue {
            @display("p=379,160;i=abstract/agent");
        }


        client25: Ue {
            @display("p=353,153;i=abstract/agent");
        }


        client26: Ue {
            @display("p=223,191;i=abstract/agent");
        }


        client27: Ue {
            @display("p=223,191;i=abstract/agent");
        }


        client28: Ue {
            @display("p=223,191;i=abstract/agent");
        }


        client29: Ue {
            @display("p=223,191;i=abstract/agent");
        }


        client30: Ue {
            @display("p=223,191;i=abstract/agent");
        }


        client31: Ue {
            @display("p=223,191;i=abstract/agent");
        }


        client32: Ue {
            @display("p=223,191;i=abstract/agent");
        }


        client33: Ue {
            @display("p=210,209;i=abstract/agent");
        }


        client34: Ue {
            @display("p=240,177;i=abstract/agent");
        }


        client35: Ue {
            @display("p=200,184;i=abstract/agent");
        }


        client36: Ue {
            @display("p=173,113;i=abstract/agent");
        }


        client37: Ue {
            @display("p=247,106;i=abstract/agent");
        }


        client38: Ue {
            @display("p=216,99;i=abstract/agent");
        }


        client39: Ue {
            @display("p=193,120;i=abstract/agent");
        }


        client40: Ue {
            @display("p=159,133;i=abstract/agent");
        }


        client41: Ue {
            @display("p=166,172;i=abstract/agent");
        }


        client42: Ue {
            @display("p=210,160;i=abstract/agent");
        }


        client43: Ue {
            @display("p=240,202;i=abstract/agent");
        }


        client44: Ue {
            @display("p=254,133;i=abstract/agent");
        }


        client45: Ue {
            @display("p=247,153;i=abstract/agent");
        }


        client46: Ue {
            @display("p=178,202;i=abstract/agent");
        }


        client47: Ue {
            @display("p=193,133;i=abstract/agent");
        }


        client48: Ue {
            @display("p=193,160;i=abstract/agent");
        }


        client49: Ue {
            @display("p=259,179;i=abstract/agent");
        }


    connections:
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> eNB.ppp;
        pgw.pppg++ <--> Eth10G <--> eNB1.ppp;

        eNB.x2++ <--> Eth10G <--> eNB1.x2++;

        router.ethg++ <--> Eth10M <--> client1.ethg++;
        router.ethg++ <--> Eth10M <--> client2.ethg++;
        router.ethg++ <--> Eth10M <--> client3.ethg++;


}

