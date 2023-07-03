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

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.AdhocHost;
import inet.physicallayer.unitdisk.UnitDiskRadioMedium;
import inet.visualizer.contract.IIntegratedVisualizer;
import modules.MosaikSchedulerModule;
import modules.MosaikScenarioManager;

network AdHocNetwork
{
    parameters:
        @display("bgb=650,450");
    submodules:
        scenarioManager: MosaikScenarioManager {

            @display("p=110,44");
        }

        schedulerModule: MosaikSchedulerModule {
            @display("p=59,44");
        }

        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer if hasVisualizer() {
            parameters:
                @display("p=100,300;is=s");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=100,100;is=s");
        }
        radioMedium: UnitDiskRadioMedium {
            parameters:
                @display("p=100,200;is=s");
        }
        client0: AdhocHost {
            parameters:
                @display("r=,,#707070;p=300,200");
        }
        client1: AdhocHost {
            parameters:
                @display("r=,,#707070;p=353,200");
        }
}
