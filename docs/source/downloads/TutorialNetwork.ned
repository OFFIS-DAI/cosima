package networks;

import modules.MosaikSchedulerModule;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.StandardHost;
import inet.node.ethernet.Eth10M;

network TutorialNetwork
{
    @display("bgb=500,500");

    submodules:
        schedulerModule: MosaikSchedulerModule {
            @display("p=60,40");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=180,40");
        }
        client0: StandardHost {
            @display("p=120,240");
        }
        client1: StandardHost {
            @display("p=380,240");
        }

    connections:
        client0.ethg++ <--> Eth10M <--> client1.ethg++;
}