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

import modules.MosaikScenarioManager;
import modules.MosaikSchedulerModule;

network SingleCell
{
    parameters:
        @display("i=block/network2;bgb=991,558;bgi=background/budapest");
    submodules:
        scenarioManager: MosaikScenarioManager {

            @display("p=97.5,317.19998");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=19582.516,1151.9127");
        }
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=50,75;is=s");
        }
        configurator: LteNetworkConfigurator {
            @display("p=50,125");
        }
        binder: LteBinder {
            @display("p=50,175;is=s");
        }
        router: Router {
            @display("p=321,136;i=device/smallrouter");
        }
        pgw: PgwStandardSimplified {
            nodeType = "PGW";
            @display("p=519,175;is=l");
        }
        eNB: eNodeB {
            @display("p=391,259;is=vl");
        }
        client0: Ue {
            @display("p=783,278");
        }
        client1: Ue {
            @display("p=710.1813,364.41876");
        }
        client2: Ue {
            @display("p=664.1625,288.55");
        }
        client3: Ue {
            @display("p=717.64374,277.35626");
        }
    connections:
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> eNB.ppp;
}
