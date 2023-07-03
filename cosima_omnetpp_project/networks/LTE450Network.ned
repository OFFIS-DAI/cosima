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

package networks;

import inet.environment.common.PhysicalEnvironment;
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



network LTE450Network
{
    // width = 10000
    // height = 15000
    @display("bgb=8000,7000;bgi=background/bhv-map1;bgs=5,m");

    types:
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
        LTE450_Mitteldeich: eNodeB {
            @display("p=8.6, 3451.2;is=n");
        }
        LTE450_Dorum: eNodeB {
            @display("p=2711.2, 65.0;is=n");
        }
        LTE450_Bederkesa: eNodeB {
            @display("p=6167.6, 1928.4;is=n");
        }
        LTE450_Frelsdorf: eNodeB {
            @display("p=7274.8, 3623.2;is=n");
        }
        LTE450_Beverstedt: eNodeB {
            @display("p=5824.4, 5655.0;is=n");
        }
        LTE450_Hagen: eNodeB {
            @display("p=3740.4, 6902.8;is=n");
        }


        client0: Ue {
            // LTE450_Beverstedt
            @display("p=5693.554,4783.6885;is=s");
        }
        client1: Ue {
            // LTE450_Beverstedt
            @display("p=5059.4053,5652.197;is=s");
        }
        client2: Ue {
            // LTE450_Bederkesa
            @display("p=6948.0664,1916.2325;is=s");
        }
        client3: Ue {
            // LTE450_Bederkesa
            @display("p=6258.774,1075.296;is=s");
        }
        physicalEnvironment: PhysicalEnvironment {
            @display("p=234.35938,206.78769");
        }
    connections:
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> LTE450_Mitteldeich.ppp;
        pgw.pppg++ <--> Eth10G <--> LTE450_Dorum.ppp;
        pgw.pppg++ <--> Eth10G <--> LTE450_Bederkesa.ppp;
        pgw.pppg++ <--> Eth10G <--> LTE450_Frelsdorf.ppp;
        pgw.pppg++ <--> Eth10G <--> LTE450_Beverstedt.ppp;
        pgw.pppg++ <--> Eth10G <--> LTE450_Hagen.ppp;

}

network LTE450TrafficNetwork extends LTE450Network
{
    submodules:
        traffic_device_0: Ue {
            @display("p=6541.7363,5645.224;is=s");
        }
        traffic_device_1: Ue {
            @display("p=6499.7124,6079.472;is=s");
        }
        traffic_device_2: Ue {
            @display("p=5981.4165,6401.6562;is=s");
        }
        traffic_device_3: Ue {
            @display("p=6415.664,5098.912;is=s");
        }
        traffic_device_4: Ue {
            @display("p=5210.976,6317.6084;is=s");
        }
}

