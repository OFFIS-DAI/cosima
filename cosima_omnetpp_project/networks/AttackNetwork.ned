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


import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth10M;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import lte.corenetwork.binder.LteBinder;
import lte.corenetwork.nodes.eNodeB;
import lte.world.radio.LteChannelControl;
import lte.epc.PgwStandardSimplified;
import lte.common.LteNetworkConfigurator;

import modules.MosaikScenarioManager;
import modules.MosaikSchedulerModule;
import modules.attacks.Attacked_Ue;


network AttackNetwork
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
        client0: Attacked_Ue {
            @display("p=783,278");
        }
        client1: Attacked_Ue {
            @display("p=710.1813,364.41876");
        }
        client2: Attacked_Ue {
            @display("p=664.1625,288.55");
        }
        client3: Attacked_Ue {
            @display("p=717.64374,277.35626");
        }
    connections:
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> eNB.ppp;
}
