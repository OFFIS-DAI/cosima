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
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.INetworkNode;
import inet.physicallayer.contract.packetlevel.IRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import modules.CosimaSchedulerModule;
import modules.CosimaScenarioManager;

network WifiNetwork
{
    parameters:
        @display("bgb=1005.0767,499.13132;bgg=100,1,grey95");
        @figure[title](type=label; pos=0,-1; anchor=sw; color=darkblue);

    submodules:
        scenarioManager: CosimaScenarioManager {
            @display("p=78.1975,51.71125");
        }
        schedulerModule: CosimaSchedulerModule {
            @display("p=200,100");
        }

        visualizer: <default(firstAvailableOrEmpty("IntegratedCanvasVisualizer"))> like IIntegratedVisualizer if typename != "" {
            @display("p=948.8606,54.512634");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=822.8001,52.809116");
        }
        radioMedium: <default("UnitDiskRadioMedium")> like IRadioMedium {
            @display("p=684.815,51.105595");
        }
        client0: <default("WirelessHost")> like INetworkNode {
            @display("p=50,325");
        }
        client1: <default("WirelessHost")> like INetworkNode {
            @display("p=650,325");
        }
        client2: <default("WirelessHost")> like INetworkNode {
            @display("p=334.23126,428.825");
        }
}
