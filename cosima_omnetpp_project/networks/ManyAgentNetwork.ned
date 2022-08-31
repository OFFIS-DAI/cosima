package networks;
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

import inet.common.scenario.ScenarioManager;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.EtherLink;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import modules.MosaikSchedulerModule;
import modules.MosaikScenarioManager;




network ManyAgentNetwork
{
    @display("bgb=3833.667,1842.9011");
    submodules:

        scenarioManager: MosaikScenarioManager {

            @display("p=97.5,317.19998");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=200,100");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=100,100");
        }

        visualizer: IntegratedCanvasVisualizer {
            @display("p=98,209");
        }

        client0: StandardHost {
            @display("p=1871.7528,1536.3519;i=abstract/pv;is=n");
        }
        client1: StandardHost {
            @display("p=1092.7574,1777.9847;i=abstract/pv;is=n");
        }
        client2: StandardHost {
            @display("p=429.16873,569.8207;i=abstract/pv;is=n");
        }
        client3: StandardHost {
            @display("p=2254.0376,274.09097;i=abstract/pv;is=n");
        }
        client4: StandardHost {
            @display("p=1316.3579,97.37442;i=abstract/pv;is=n");
        }
        client5: StandardHost {
            @display("p=3285.485,1709.462;i=abstract/pv;is=n");
        }
        client6: StandardHost {
            @display("p=3581.2148,1161.2802;i=abstract/pv;is=n");
        }
        client7: StandardHost {
            @display("p=2564.193,400.31708;i=abstract/pv;is=n");
        }
        client8: StandardHost {
            @display("p=3209.7495,274.09097;i=abstract/pv;is=n");
        }
        client9: StandardHost {
            @display("p=3447.776,811.4535;i=abstract/pv;is=n");
        }
        client10: StandardHost {
            @display("p=3209.7495,1089.1509;i=abstract/pv;is=n");
        }
        client11: StandardHost {
            @display("p=2730.0903,1709.462;i=abstract/pv;is=n");
        }
        client12: StandardHost {
            @display("p=2098.9597,1709.462;i=abstract/pv;is=n");
        }
        client13: StandardHost {
            @display("p=1045.8734,274.09097;i=abstract/pv;is=n");
        }
        client14: StandardHost {
            @display("p=1557.9907,1745.5266;i=abstract/pv;is=n");
        }
        client15: StandardHost {
            @display("p=609.49176,1749.133;i=abstract/pv;is=n");
        }
        client16: StandardHost {
            @display("p=147.86487,706.86615;i=abstract/pv;is=n");
        }
        client17: StandardHost {
            @display("p=652.7693,569.8207;i=abstract/pv;is=n");
        }
        client18: StandardHost {
            @display("p=147.86487,1561.5972;i=abstract/pv;is=n");
        }
        client19: StandardHost {
            @display("p=1987.1594,162.2907;i=abstract/pv;is=n");
        }


        router10: Router {
            @display("p=2874.3486,1237.0157;is=l");
        }
        router14: Router {
            @display("p=1045.8734,966.5313;is=l");
        }
        router16: Router {
            @display("p=497.6915,937.6796;is=l");
        }
        router17: Router {
            @display("p=2506.4897,1237.0157;is=l");
        }
        router18: Router {
            @display("p=2647.1416,966.5313;is=l");
        }
        router19: Router {
            @display("p=2423.5413,703.2597;is=l");
        }
        router20: Router {
            @display("p=2142.2373,1193.7383;is=l");
        }
        router21: Router {
            @display("p=2142.2373,703.2597;is=l");
        }
        router23: Router {
            @display("p=1557.9907,1356.0289;is=l");
        }
        router24: Router {
            @display("p=1936.6691,966.5313;is=l");
        }
        router25: Router {
            @display("p=1557.9907,566.21423;is=l");
        }
        router27: Router {
            @display("p=768.17596,937.6796;is=l");
        }
        router28: Router {
            @display("p=1601.2683,966.5313;is=l");
        }
        router29: Router {
            @display("p=1446.1904,1107.1832;is=l");
        }
        router30: Router {
            @display("p=1204.5576,1085.5444;is=l");
        }
        router31: Router {
            @display("p=1446.1904,807.84705;is=l");
        }
        router32: Router {
            @display("p=1204.5576,807.84705;is=l");
        }

        switch0: EtherSwitch {
            @display("p=1871.7528,1316.3579;is=l");
        }
        switch1: EtherSwitch {
            @display("p=450.8075,757.3566;is=l");
        }
        switch2: EtherSwitch {
            @display("p=1269.4739,1557.9907;is=l");
        }
        switch3: EtherSwitch {
            @display("p=2098.9597,454.41397;is=l");
        }
        switch4: EtherSwitch {
            @display("p=1420.9453,270.4845;is=l");
        }
        switch5: EtherSwitch {
            @display("p=3072.7039,1651.7587;is=l");
        }
        switch6: EtherSwitch {
            @display("p=3469.4146,1316.3579;is=l");
        }
        switch7: EtherSwitch {
            @display("p=429.16873,1435.3711;is=l");
        }
        switch8: EtherSwitch {
            @display("p=2935.6584,703.2597;is=l");
        }
        switch9: EtherSwitch {
            @display("p=710.47266,757.3566;is=l");
        }
        switch10: EtherSwitch {
            @display("p=2311.741,548.18195;is=l");
        }
        switch11: EtherSwitch {
            @display("p=2935.6584,364.25247;is=l");
        }
        switch12: EtherSwitch {
            @display("p=2986.149,937.6796;is=l");
        }
        switch13: EtherSwitch {
            @display("p=2647.1416,1507.5002;is=l");
        }
        switch14: EtherSwitch {
            @display("p=2142.2373,1507.5002;is=l");
        }
        switch15: EtherSwitch {
            @display("p=1759.9525,310.15555;is=l");
        }
        switch16: EtherSwitch {
            @display("p=908.82794,454.41397;is=l");
        }
        switch17: EtherSwitch {
            @display("p=198.3553,937.6796;is=l");
        }
        switch18: EtherSwitch {
            @display("p=822.2729,1579.6295;is=l");
        }
        switch19: EtherSwitch {
            @display("p=1691.4298,1579.6295;is=l");
        }


    connections:

        router32.ethg++ <--> Eth10M <--> router31.ethg++;
        router31.ethg++ <--> Eth10M <--> router28.ethg++;
        router28.ethg++ <--> Eth10M <--> router29.ethg++;
        router30.ethg++ <--> Eth10M <--> router29.ethg++;
        router30.ethg++ <--> Eth10M <--> router14.ethg++;
        router14.ethg++ <--> Eth10M <--> router32.ethg++;
        router31.ethg++ <--> Eth10M <--> router25.ethg++;
        router25.ethg++ <--> Eth10M <--> switch15.ethg++;
        router14.ethg++ <--> Eth10M <--> router27.ethg++;
        router27.ethg++ <--> Eth10M <--> router16.ethg++;
        router27.ethg++ <--> Eth10M <--> switch9.ethg++;
        router16.ethg++ <--> Eth10M <--> switch17.ethg++;
        router29.ethg++ <--> Eth10M <--> router23.ethg++;
        router23.ethg++ <--> Eth10M <--> switch19.ethg++;
        router28.ethg++ <--> Eth10M <--> router24.ethg++;
        router24.ethg++ <--> Eth10M <--> router21.ethg++;
        router21.ethg++ <--> Eth10M <--> router19.ethg++;
        router19.ethg++ <--> Eth10M <--> router18.ethg++;
        router18.ethg++ <--> Eth10M <--> router17.ethg++;
        router17.ethg++ <--> Eth10M <--> router20.ethg++;
        router24.ethg++ <--> Eth10M <--> router20.ethg++;
        router21.ethg++ <--> Eth10M <--> switch10.ethg++;
        router18.ethg++ <--> Eth10M <--> switch12.ethg++;
        router17.ethg++ <--> Eth10M <--> switch13.ethg++;
        router20.ethg++ <--> Eth10M <--> switch14.ethg++;
        router18.ethg++ <--> Eth10M <--> router10.ethg++;
        switch12.ethg++ <--> Eth10M <--> client10.ethg++;
        switch6.ethg++ <--> Eth10M <--> client6.ethg++;
        switch8.ethg++ <--> Eth10M <--> client9.ethg++;
        switch5.ethg++ <--> Eth10M <--> client5.ethg++;
        switch13.ethg++ <--> Eth10M <--> client11.ethg++;
        switch14.ethg++ <--> Eth10M <--> client12.ethg++;
        switch19.ethg++ <--> Eth10M <--> client14.ethg++;
        switch18.ethg++ <--> Eth10M <--> client15.ethg++;
        switch7.ethg++ <--> Eth10M <--> client18.ethg++;
        client16.ethg++ <--> Eth10M <--> switch17.ethg++;
        switch9.ethg++ <--> Eth10M <--> client17.ethg++;
        switch16.ethg++ <--> Eth10M <--> client13.ethg++;
        switch15.ethg++ <--> Eth10M <--> client19.ethg++;
        switch10.ethg++ <--> Eth10M <--> client7.ethg++;
        switch11.ethg++ <--> Eth10M <--> client8.ethg++;
        router25.ethg++ <--> Eth10M <--> switch4.ethg++;
        client4.ethg++ <--> Eth10M <--> switch4.ethg++;
        router21.ethg++ <--> Eth10M <--> switch3.ethg++;
        switch3.ethg++ <--> Eth10M <--> client3.ethg++;
        client1.ethg++ <--> Eth10M <--> switch2.ethg++;
        switch2.ethg++ <--> Eth10M <--> router23.ethg++;
        router20.ethg++ <--> Eth10M <--> switch0.ethg++;
        switch0.ethg++ <--> Eth10M <--> client0.ethg++;
        router16.ethg++ <--> Eth10M <--> switch1.ethg++;
        switch1.ethg++ <--> Eth10M <--> client2.ethg++;
        switch7.ethg++ <--> Eth10M <--> router14.ethg++;
        router19.ethg++ <--> Eth10M <--> switch11.ethg++;
        router10.ethg++ <--> Eth10M <--> switch5.ethg++;
        router10.ethg++ <--> Eth10M <--> switch6.ethg++;
        router19.ethg++ <--> Eth10M <--> switch8.ethg++;
        switch16.ethg++ <--> Eth10M <--> router32.ethg++;
        router30.ethg++ <--> Eth10M <--> switch18.ethg++;
}

