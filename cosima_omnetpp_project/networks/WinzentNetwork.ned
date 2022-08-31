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


channel MyEth10M extends EtherLink
{
    datarate = 10Mbps;
}

network CosimaNetwork
{
    @display("bgb=1694.396,1062.732,#D3D7CF");
    submodules:

        scenarioManager: MosaikScenarioManager {

            @display("p=162.184,435.336");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=162.184,597.52");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=162.184,76.824005");
        }

        client0: StandardHost {
            @display("p=1203.576,121.638;i=abstract/pv;is=n");

        }
        client2: StandardHost {
            @display("p=1500.202,680.74603;i=abstract/pv;is=n");
        }
        client1: StandardHost {
            @display("p=1282.534,401.19202;i=abstract/pv;is=n");
        }
        router0: Router {
            @display("p=1060.598,661.54004;is=l");
        }
        router1: Router {
            @display("p=783.17804,473.74802;is=l");
        }
        switch0: EtherSwitch {
            @display("p=1284.668,680.74603;is=l");
        }
        switch1: EtherSwitch {
            @display("p=979.50604,132.308;is=l");
        }

        visualizer: IntegratedCanvasVisualizer {
            @display("p=162.184,249.67801");
        }

        router2: Router {
            @display("p=691.416,300.894;is=l");
        }
        router3: Router {
            @display("p=904.81604,324.368;is=l");
        }
        router4: Router {
            @display("p=768.24,151.514;is=l");
        }
        switch2: EtherSwitch {
            @display("p=1096.876,339.306;is=l");
        }
        client3: StandardHost {
            @display("p=1515.14,964.568;i=abstract/pv;is=n");
        }
        switch3: EtherSwitch {
            @display("p=1171.566,898.414;is=l");
        }
        client4: StandardHost {
            @display("p=428.93402,324.368;i=abstract/pv;is=n");
        }
    connections:
        switch0.ethg++ <--> Eth10M {  length = 1000km; } <--> client2.ethg++;
        switch0.ethg++ <--> Eth10M {  length = 1000km; } <--> router0.ethg++;
        router0.ethg++ <--> Eth10M {  length = 1000km; } <--> router1.ethg++;
        client0.ethg++ <--> Eth10M {  length = 500km; } <--> switch1.ethg++;
        router2.ethg++ <--> Eth10M {  length = 100km; } <--> router4.ethg++;
        router2.ethg++ <--> Eth10M {  length = 500km; } <--> router1.ethg++;
        router1.ethg++ <--> Eth10M {  length = 500km; } <--> router3.ethg++;
        router3.ethg++ <--> Eth10M {  length = 50km; } <--> router4.ethg++;
        router4.ethg++ <--> Eth10M {  length = 50km; } <--> switch1.ethg++;
        router2.ethg++ <--> Eth10M {  length = 50km; } <--> router3.ethg++;
        switch2.ethg++ <--> Eth10M {  length = 50km; } <--> client1.ethg++;
        switch2.ethg++ <--> Eth10M {  length = 50km; } <--> router3.ethg++;
        switch3.ethg++ <--> Eth10M {  length = 50km; } <--> client3.ethg++;
        router0.ethg++ <--> Eth10M {  length = 50km; } <--> switch3.ethg++;
        client4.ethg++ <--> Eth10M {  length = 50km; } <--> router2.ethg++;
}

network DisconnectedClient2_WinzentNetwork
{
    submodules:

        scenarioManager: MosaikScenarioManager {
            @display("p=216.6825,136.52249");
        }
        schedulerModule: MosaikSchedulerModule {
            @display("p=200,100");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=100,100");
        }

        client0: StandardHost {
            @display("p=721,100");

        }
        client2: StandardHost {
            @display("p=1016.6,659.1");
        }
        client1: StandardHost {
            @display("p=799.5,379.6");
        }
        router0: Router {
            @display("p=577.2,638.3");
        }
        router1: Router {
            @display("p=300.3,452.4");
        }
        switch0: EtherSwitch {
            @display("p=800.8,657.8");
        }
        switch1: EtherSwitch {
            @display("p=495.3,109.2");
        }

        visualizer: IntegratedCanvasVisualizer {
            @display("p=98,209");
        }

        router2: Router {
            @display("p=208,279.5");
        }
        router3: Router {
            @display("p=422.5,301.6");
        }
        router4: Router {
            @display("p=284.69998,128.7");
        }
        switch2: EtherSwitch {
            @display("p=613.6,315.9");
        }
    connections:
        switch0.ethg++ <--> Eth10M {  length = 1000km; } <--> router0.ethg++;
        router0.ethg++ <--> Eth10M {  length = 1000km; } <--> router1.ethg++;
        client0.ethg++ <--> Eth10M {  length = 500km; } <--> switch1.ethg++;
        router2.ethg++ <--> Eth10M {  length = 100km; } <--> router4.ethg++;
        router2.ethg++ <--> Eth10M {  length = 500km; } <--> router1.ethg++;
        router1.ethg++ <--> Eth10M {  length = 500km; } <--> router3.ethg++;
        router3.ethg++ <--> Eth10M {  length = 50km; } <--> router4.ethg++;
        router4.ethg++ <--> Eth10M {  length = 50km; } <--> switch1.ethg++;
        router2.ethg++ <--> Eth10M {  length = 50km; } <--> router3.ethg++;
        switch2.ethg++ <--> Eth10M {  length = 50km; } <--> client1.ethg++;
        switch2.ethg++ <--> Eth10M {  length = 50km; } <--> router3.ethg++;
}

network DisconnectedClient1_WinzentNetwork
{
    submodules:
        scenarioManager: MosaikScenarioManager {
            @display("p=216.6825,136.52249");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=200,100");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=100,100");
        }

        client0: StandardHost {
            @display("p=721,100");

        }
        client2: StandardHost {
            @display("p=1016.6,659.1");
        }
        client1: StandardHost {
            @display("p=799.5,379.6");
        }
        router0: Router {
            @display("p=577.2,638.3");
        }
        router1: Router {
            @display("p=300.3,452.4");
        }
        switch0: EtherSwitch {
            @display("p=800.8,657.8");
        }
        switch1: EtherSwitch {
            @display("p=495.3,109.2");
        }

        visualizer: IntegratedCanvasVisualizer {
            @display("p=98,209");
        }

        router2: Router {
            @display("p=208,279.5");
        }
        router3: Router {
            @display("p=422.5,301.6");
        }
        router4: Router {
            @display("p=284.69998,128.7");
        }
        switch2: EtherSwitch {
            @display("p=613.6,315.9");
        }
    connections:
        switch0.ethg++ <--> Eth10M {  length = 1000km; } <--> client2.ethg++;
        switch0.ethg++ <--> Eth10M {  length = 1000km; } <--> router0.ethg++;
        router0.ethg++ <--> Eth10M {  length = 1000km; } <--> router1.ethg++;
        client0.ethg++ <--> Eth10M {  length = 500km; } <--> switch1.ethg++;
        router2.ethg++ <--> Eth10M {  length = 100km; } <--> router4.ethg++;
        router2.ethg++ <--> Eth10M {  length = 500km; } <--> router1.ethg++;
        router1.ethg++ <--> Eth10M {  length = 500km; } <--> router3.ethg++;
        router3.ethg++ <--> Eth10M {  length = 50km; } <--> router4.ethg++;
        router4.ethg++ <--> Eth10M {  length = 50km; } <--> switch1.ethg++;
        router2.ethg++ <--> Eth10M {  length = 50km; } <--> router3.ethg++;
        switch2.ethg++ <--> Eth10M {  length = 50km; } <--> router3.ethg++;
}

network SimpleWinzentNetwork
{
    submodules:
        scenarioManager: MosaikScenarioManager {
            @display("p=216.6825,136.52249");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=200,100");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=100,100");
        }

        client0: StandardHost {
            @display("p=183,404");

        }
        client2: StandardHost {
            @display("p=712,365");
        }
        client1: StandardHost {
            @display("p=633,564");
        }

        router0: Router {
            @display("p=463,359");
        }
        switch0: EtherSwitch {
            @display("p=582,358");
        }
        switch1: EtherSwitch {
            @display("p=307,341");
        }

        visualizer: IntegratedCanvasVisualizer {
            @display("p=98,209");
        }

        switch2: EtherSwitch {
            @display("p=543,477");
        }
    connections:
        switch0.ethg++ <--> Eth10M {  length = 67km; } <--> client2.ethg++;
        switch0.ethg++ <--> Eth10M {  length = 100km; } <--> router0.ethg++;

        client0.ethg++ <--> Eth10M {  length = 100km; } <--> switch1.ethg++;
        router0.ethg++ <--> Eth10M {  length = 100km; } <--> switch1.ethg++;
        router0.ethg++ <--> Eth10M {  length = 100km; } <--> switch2.ethg++;
        switch2.ethg++ <--> Eth10M {  length = 100km; } <--> client1.ethg++;

}



network MinimalNetwork
{
    submodules:
        scenarioManager: MosaikScenarioManager {
            @display("p=216.6825,136.52249");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=200,100");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=100,100");
        }

        host0: StandardHost {
            @display("p=580,233");

        }
        host1: StandardHost {
            @display("p=98,384");
        }

    connections:
        host0.ethg++ <--> Eth10M <--> host1.ethg++;

}
