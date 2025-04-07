//
// Copyright (C) 2004 Emin Ilker Cetinbas (niw3@yahoo.com)
//
// This library is free software, you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//

package networks;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.node.internetcloud.InternetCloud;
import modules.CosimaSchedulerModule;
import modules.CosimaScenarioManager;


network CloudNetwork
{
    @display("bgb=998.7494,586.0717;bgg=1;bgi=background/streetmap");

    submodules:
        scenarioManager: CosimaScenarioManager {
            @display("p=216.6825,136.52249");
        }
        schedulerModule: CosimaSchedulerModule {
            @display("p=68.8875,129.00749");
        }

        router4: Router {
            @display("p=690.1081,301.70554");
        }
        router2: Router {
            @display("p=287.834,308.6413");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=128.31155,45.08244;is=s");
        }
        client1: StandardHost {
            @display("p=811.4839,215.00854;i=device/pocketpc");
        }
        client0: StandardHost {
            @display("p=301.70554,454.29227;i=device/pocketpc");
        }
        internetCloud: InternetCloud {
            @display("p=488.96347,215.90594");
        }
        client2: StandardHost {
            @display("p=513.52496,467.1825;i=device/pocketpc");
        }
        router1: Router {
            @display("p=488.475,356.9625");
        }
    connections:
        client0.ethg++ <--> Eth100M <--> router2.ethg++;
        client1.ethg++ <--> Eth100M <--> router4.ethg++;
        client2.ethg++ <--> Eth100M <--> router1.ethg++;
        router2.pppg++ <--> Eth100M <--> internetCloud.pppg++;
        router1.pppg++ <--> Eth100M <--> internetCloud.pppg++;
        internetCloud.pppg++ <--> Eth100M <--> router4.pppg++;
}

