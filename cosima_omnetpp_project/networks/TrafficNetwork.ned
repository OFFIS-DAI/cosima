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

import inet.common.scenario.ScenarioManager;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.EtherLink;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import modules.CosimaSchedulerModule;
import modules.CosimaScenarioManager;


network TrafficNetwork
{
    @display("bgb=5003.927,3943.4114");
    submodules:

        scenarioManager: CosimaScenarioManager {

            @display("p=97.5,317.19998");
        }

        schedulerModule: CosimaSchedulerModule {
            @display("p=200,100");
        }

        configurator: Ipv4NetworkConfigurator {
            @display("p=100,100");
        }

        client0: StandardHost {
            @display("p=3327.8247,146.27802");

        }
        client2: StandardHost {
            @display("p=3656.9502,603.3968");
        }
        client1: StandardHost {
            @display("p=3388.774,463.21368");
        }
        router0: Router {
            @display("p=3163.262,719.2002");
        }
        router1: Router {
            @display("p=2888.9907,536.3527");
        }
        switch0: EtherSwitch {
            @display("p=3382.679,676.53577");
        }
        switch1: EtherSwitch {
            @display("p=3084.028,188.94243");
        }

        visualizer: IntegratedCanvasVisualizer {
            @display("p=98,209");
        }

        router2: Router {
            @display("p=2797.567,359.6001");
        }
        router3: Router {
            @display("p=3010.889,383.97977");
        }
        router4: Router {
            @display("p=2870.7058,213.3221");
        }
        switch2: EtherSwitch {
            @display("p=3199.8315,396.16962");
        }
        client3: StandardHost {
            @display("p=3608.191,835.00366");
        }
        switch3: EtherSwitch {
            @display("p=3382.679,828.9087");
        }
        traffic_device_0: StandardHost {
            @display("p=2596.4346,1066.6105");

        }
        traffic_device_1: StandardHost {
            @display("p=2456.2515,676.53577");

        }
        switch4: EtherSwitch {
            @display("p=3004.7942,902.0477");
        }
        switch5: EtherSwitch {
            @display("p=2785.3772,700.91547");
        }
        traffic_device_2: StandardHost {
            @display("p=3053.5535,1066.6105");

        }
        traffic_device_3: StandardHost {
            @display("p=2968.2246,774.05444");

        }
        traffic_device_4: StandardHost {
            @display("p=3303.445,286.4611");

        }
        router5: Router {
            @display("p=3303.445,1310.4071");
        }
        router6: Router {
            @display("p=3748.374,1371.3563");
        }
        router7: Router {
            @display("p=4193.3027,1194.6038");
        }
        router8: Router {
            @display("p=4114.069,804.52905");
        }
        router9: Router {
            @display("p=3797.1333,530.25775");
        }
        router10: Router {
            @display("p=3230.306,2340.4482");
        }
        client4: StandardHost {
            @display("p=3163.262,2121.031");
        }
        client5: StandardHost {
            @display("p=2888.9907,2334.3533");
        }
        client6: StandardHost {
            @display("p=2888.9907,2614.7195");
        }
        client7: StandardHost {
            @display("p=3199.8315,2614.7195");
        }
        client8: StandardHost {
            @display("p=3486.2925,2614.7195");
        }
        client9: StandardHost {
            @display("p=3656.9502,2389.2075");
        }
        client10: StandardHost {
            @display("p=3528.957,2188.0752");
        }
        client11: StandardHost {
            @display("p=3797.1333,1767.5259");
        }
        client12: StandardHost {
            @display("p=4047.025,1682.1971");
        }
        client13: StandardHost {
            @display("p=4339.581,1499.3496");
        }
        client14: StandardHost {
            @display("p=4558.998,1237.2682");
        }
        client15: StandardHost {
            @display("p=4485.859,774.05444");
        }
        client16: StandardHost {
            @display("p=4412.7197,530.25775");
        }
        client17: StandardHost {
            @display("p=4193.3027,396.16962");
        }
        client18: StandardHost {
            @display("p=3894.6519,207.22717");
        }
        client19: StandardHost {
            @display("p=2968.2246,1499.3496");
        }
        client20: StandardHost {
            @display("p=3230.306,1682.1971");
        }
        router11: Router {
            @display("p=2450.1567,1420.1157");
        }
        router12: Router {
            @display("p=1676.1022,1371.3563");
        }
        router13: Router {
            @display("p=1529.8242,1084.8953");
        }
        router14: Router {
            @display("p=1121.4647,969.0918");
        }
        router15: Router {
            @display("p=828.9087,1304.3123");
        }
        router16: Router {
            @display("p=975.1867,1566.3937");
        }
        router17: Router {
            @display("p=1322.597,1633.4377");
        }
        switch6: EtherSwitch {
            @display("p=1030.041,713.1053");
        }

        client21: StandardHost {
            @display("p=1377.4513,560.73236");
        }
        client22: StandardHost {
            @display("p=987.3766,463.21368");
        }
        client23: StandardHost {
            @display("p=414.45435,835.00366");
        }
        client24: StandardHost {
            @display("p=243.79668,1188.5088");
        }
        client25: StandardHost {
            @display("p=268.17636,1548.1089");
        }
        client26: StandardHost {
            @display("p=243.79668,1980.848");
        }
        client27: StandardHost {
            @display("p=524.16284,1858.9497");
        }
        client28: StandardHost {
            @display("p=877.668,1932.0887");
        }
        client29: StandardHost {
            @display("p=1133.6545,1999.1328");
        }
        client30: StandardHost {
            @display("p=1426.2106,1913.804");
        }
        client31: StandardHost {
            @display("p=1730.9564,2035.7023");
        }
        client32: StandardHost {
            @display("p=1767.5259,1767.5259");
        }
        client33: StandardHost {
            @display("p=2011.3226,1682.1971");
        }
        client34: StandardHost {
            @display("p=1676.1022,798.43414");
        }
        client35: StandardHost {
            @display("p=1865.0446,835.00366");
        }
        client36: StandardHost {
            @display("p=1968.6582,1066.6105");
        }
        router18: Router {
            @display("p=713.1053,2687.8584");
        }
        client37: StandardHost {
            @display("p=920.33246,2474.5364");
        }
        client38: StandardHost {
            @display("p=1066.6105,2681.7634");
        }
        client39: StandardHost {
            @display("p=713.1053,2986.5093");
        }
        client40: StandardHost {
            @display("p=962.9969,2986.5093");
        }
        client41: StandardHost {
            @display("p=420.5493,2986.5093");
        }
        client42: StandardHost {
            @display("p=323.0306,2681.7634");
        }
        client43: StandardHost {
            @display("p=524.16284,2535.4856");
        }
        client44: StandardHost {
            @display("p=219.417,2474.5364");
        }
        client45: StandardHost {
            @display("p=115.80342,2809.7568");
        }
        client46: StandardHost {
            @display("p=115.80342,3090.123");
        }
        client47: StandardHost {
            @display("p=420.5493,3321.7297");
        }
        client48: StandardHost {
            @display("p=713.1053,3321.7297");
        }
        client49: StandardHost {
            @display("p=1066.6105,3321.7297");
        }
        client50: StandardHost {
            @display("p=1255.5529,3090.123");
        }
        client51: StandardHost {
            @display("p=1401.8309,2895.0857");
        }
        client52: StandardHost {
            @display("p=1359.1665,2565.96");
        }
        router19: Router {
            @display("p=4114.069,2992.6042");
        }
        client53: StandardHost {
            @display("p=4047.025,2724.428");
        }
        client54: StandardHost {
            @display("p=4047.025,2724.428");
        }
        client55: StandardHost {
            @display("p=4047.025,2724.428");
        }
        client56: StandardHost {
            @display("p=4406.625,2535.4856");
        }
        client57: StandardHost {
            @display("p=4193.3027,2724.428");
        }
        client58: StandardHost {
            @display("p=3388.774,3236.401");
        }
        client59: StandardHost {
            @display("p=4193.3027,2480.6313");
        }
        client60: StandardHost {
            @display("p=4485.859,2663.4788");
        }
        client61: StandardHost {
            @display("p=3608.191,3480.1975");
        }
        client62: StandardHost {
            @display("p=3608.191,3199.8315");
        }
        client63: StandardHost {
            @display("p=4705.276,2901.1804");
        }
        client64: StandardHost {
            @display("p=3608.191,2956.0347");
        }
        client65: StandardHost {
            @display("p=4260.347,3382.679");
        }
        client66: StandardHost {
            @display("p=4047.025,3480.1975");
        }
        client67: StandardHost {
            @display("p=3797.1333,3260.7805");
        }
        client68: StandardHost {
            @display("p=3797.1333,3004.7942");
        }
        client69: StandardHost {
            @display("p=4558.998,3120.5974");
        }
        client70: StandardHost {
            @display("p=4339.581,2809.7568");
        }
        router20: Router {
            @display("p=2303.8787,1919.8988");
        }
        router21: Router {
            @display("p=1956.4684,2486.726");
        }
        router22: Router {
            @display("p=2078.3667,3474.1028");
        }
        router23: Router {
            @display("p=2383.1125,2748.8076");
        }
        client71: StandardHost {
            @display("p=2669.5737,2114.9363");
        }
        client72: StandardHost {
            @display("p=2669.5737,3382.679");
        }
        client73: StandardHost {
            @display("p=2157.6006,3090.123");
        }
        client74: StandardHost {
            @display("p=2529.3906,3114.5027");
        }
        client75: StandardHost {
            @display("p=1877.2345,3090.123");
        }
        client76: StandardHost {
            @display("p=2675.6685,3602.096");
        }
        client77: StandardHost {
            @display("p=1468.875,3748.374");
        }
        client78: StandardHost {
            @display("p=1730.9564,3260.7805");
        }
        client79: StandardHost {
            @display("p=1621.2479,3468.0078");
        }
        client80: StandardHost {
            @display("p=2596.4346,3827.608");
        }
        client81: StandardHost {
            @display("p=2340.4482,3224.2112");
        }
        client82: StandardHost {
            @display("p=2157.6006,2480.6313");
        }
        client83: StandardHost {
            @display("p=2114.9363,1980.848");
        }
        client84: StandardHost {
            @display("p=2529.3906,1828.4751");
        }
        client85: StandardHost {
            @display("p=2596.4346,2535.4856");
        }
        client86: StandardHost {
            @display("p=2529.3906,3321.7297");
        }
        client87: StandardHost {
            @display("p=2078.3667,3224.2112");
        }
        client88: StandardHost {
            @display("p=2303.8787,3382.679");
        }
        client89: StandardHost {
            @display("p=2450.1567,3528.957");
        }
        client90: StandardHost {
            @display("p=2303.8787,3748.374");
        }
        client91: StandardHost {
            @display("p=1767.5259,2742.7126");
        }
        client92: StandardHost {
            @display("p=1676.1022,2474.5364");
        }
        client93: StandardHost {
            @display("p=1968.6582,2261.214");
        }
        client94: StandardHost {
            @display("p=1688.292,3638.6655");
        }
        client95: StandardHost {
            @display("p=1877.2345,3748.374");
        }
        client96: StandardHost {
            @display("p=2078.3667,3748.374");
        }
        client97: StandardHost {
            @display("p=1877.2345,3382.679");
        }
        client98: StandardHost {
            @display("p=2592.684,2723.9592");
        }
        client99: StandardHost {
            @display("p=2156.663,2784.9082");
        }
    connections:
        switch0.ethg++ <--> Eth100M <--> client2.ethg++;
        switch0.ethg++ <--> Eth100M <--> router0.ethg++;
        router0.ethg++ <--> Eth100M <--> router1.ethg++;
        client0.ethg++ <--> Eth100M <--> switch1.ethg++;
        router2.ethg++ <--> Eth100M <--> router4.ethg++;
        router2.ethg++ <--> Eth100M <--> router1.ethg++;
        router1.ethg++ <--> Eth100M <--> router3.ethg++;
        router3.ethg++ <--> Eth100M <--> router4.ethg++;
        router4.ethg++ <--> Eth100M <--> switch1.ethg++;
        router2.ethg++ <--> Eth100M <--> router3.ethg++;
        switch2.ethg++ <--> Eth100M <--> client1.ethg++;
        switch2.ethg++ <--> Eth100M <--> router3.ethg++;
        switch3.ethg++ <--> Eth100M <--> client3.ethg++;
        router0.ethg++ <--> Eth100M <--> switch3.ethg++;
        router0.ethg++ <--> Eth100M <--> switch4.ethg++;
        router1.ethg++ <--> Eth100M <--> switch5.ethg++;
        traffic_device_1.ethg++ <--> Eth100M <--> switch5.ethg++;
        switch4.ethg++ <--> Eth100M <--> traffic_device_0.ethg++;
        switch1.ethg++ <--> Eth100M <--> traffic_device_4.ethg++;
        switch4.ethg++ <--> Eth100M <--> traffic_device_2.ethg++;
        switch5.ethg++ <--> Eth100M <--> traffic_device_3.ethg++;
        router0.ethg++ <--> Eth100M <--> router5.ethg++;
        router5.ethg++ <--> Eth100M <--> router6.ethg++;
        router6.ethg++ <--> Eth100M <--> router7.ethg++;
        router8.ethg++ <--> Eth100M <--> router7.ethg++;
        router9.pppg++ <--> Eth100M <--> router8.pppg++;
        router9.ethg++ <--> Eth100M <--> router0.ethg++;
        router6.ethg++ <--> Eth100M <--> router10.ethg++;
        router10.ethg++ <--> Eth100M <--> client10.ethg++;
        router10.ethg++ <--> Eth100M <--> client9.ethg++;
        router10.ethg++ <--> Eth100M <--> client8.ethg++;
        router10.ethg++ <--> Eth100M <--> client7.ethg++;
        router10.ethg++ <--> Eth100M <--> client6.ethg++;
        router10.ethg++ <--> Eth100M <--> client5.ethg++;
        router10.ethg++ <--> Eth100M <--> client4.ethg++;
        router9.ethg++ <--> Eth100M <--> client18.ethg++;
        router9.ethg++ <--> Eth100M <--> client17.ethg++;
        router8.ethg++ <--> Eth100M <--> client16.ethg++;
        router8.ethg++ <--> Eth100M <--> client15.ethg++;
        router7.ethg++ <--> Eth100M <--> client14.ethg++;
        router7.ethg++ <--> Eth100M <--> client13.ethg++;
        router6.ethg++ <--> Eth100M <--> client12.ethg++;
        router6.ethg++ <--> Eth100M <--> client11.ethg++;
        client19.ethg++ <--> Eth100M <--> router5.ethg++;
        router5.ethg++ <--> Eth100M <--> client20.ethg++;
        router5.ethg++ <--> Eth100M <--> router11.ethg++;
        router11.ethg++ <--> Eth100M <--> router12.ethg++;
        router12.ethg++ <--> Eth100M <--> router13.ethg++;
        router13.ethg++ <--> Eth100M <--> router14.ethg++;
        router14.ethg++ <--> Eth100M <--> router15.ethg++;
        router15.ethg++ <--> Eth100M <--> router16.ethg++;
        router16.ethg++ <--> Eth100M <--> router17.ethg++;
        router17.ethg++ <--> Eth100M <--> router12.ethg++;
        router14.ethg++ <--> Eth100M <--> switch6.ethg++;
        switch6.ethg++ <--> Eth100M <--> client21.ethg++;
        switch6.ethg++ <--> Eth100M <--> client22.ethg++;
        router15.ethg++ <--> Eth100M <--> client23.ethg++;
        router15.ethg++ <--> Eth100M <--> client24.ethg++;
        router16.ethg++ <--> Eth100M <--> client25.ethg++;
        router16.ethg++ <--> Eth100M <--> client26.ethg++;
        router17.ethg++ <--> Eth100M <--> client28.ethg++;
        router16.ethg++ <--> Eth100M <--> client27.ethg++;
        router17.ethg++ <--> Eth100M <--> client29.ethg++;
        router17.ethg++ <--> Eth100M <--> client30.ethg++;
        router17.ethg++ <--> Eth100M <--> client31.ethg++;
        router12.ethg++ <--> Eth100M <--> client32.ethg++;
        router12.ethg++ <--> Eth100M <--> client33.ethg++;
        client34.ethg++ <--> Eth100M <--> router13.ethg++;
        client35.ethg++ <--> Eth100M <--> router13.ethg++;
        client36.ethg++ <--> Eth100M <--> router13.ethg++;
        router17.ethg++ <--> Eth100M <--> router18.ethg++;
        client37.ethg++ <--> Eth100M <--> router18.ethg++;
        client38.ethg++ <--> Eth100M <--> router18.ethg++;
        client39.ethg++ <--> Eth100M <--> router18.ethg++;
        client40.ethg++ <--> Eth100M <--> router18.ethg++;
        client41.ethg++ <--> Eth100M <--> router18.ethg++;
        client42.ethg++ <--> Eth100M <--> router18.ethg++;
        client43.ethg++ <--> Eth100M <--> router18.ethg++;
        client44.ethg++ <--> Eth100M <--> router18.ethg++;
        client45.ethg++ <--> Eth100M <--> router18.ethg++;
        client46.ethg++ <--> Eth100M <--> router18.ethg++;
        client47.ethg++ <--> Eth100M <--> router18.ethg++;
        client48.ethg++ <--> Eth100M <--> router18.ethg++;
        client49.ethg++ <--> Eth100M <--> router18.ethg++;
        client50.ethg++ <--> Eth100M <--> router18.ethg++;
        client51.ethg++ <--> Eth100M <--> router18.ethg++;
        client52.ethg++ <--> Eth100M <--> router18.ethg++;
        router10.ethg++ <--> Eth100M <--> router19.ethg++;
        client53.ethg++ <--> Eth100M <--> router19.ethg++;
        client54.ethg++ <--> Eth100M <--> router19.ethg++;
        client55.ethg++ <--> Eth100M <--> router19.ethg++;
        client56.ethg++ <--> Eth100M <--> router19.ethg++;
        client57.ethg++ <--> Eth100M <--> router19.ethg++;
        client58.ethg++ <--> Eth100M <--> router19.ethg++;
        client59.ethg++ <--> Eth100M <--> router19.ethg++;
        client60.ethg++ <--> Eth100M <--> router19.ethg++;
        client61.ethg++ <--> Eth100M <--> router19.ethg++;
        client62.ethg++ <--> Eth100M <--> router19.ethg++;
        client63.ethg++ <--> Eth100M <--> router19.ethg++;
        client64.ethg++ <--> Eth100M <--> router19.ethg++;
        client65.ethg++ <--> Eth100M <--> router19.ethg++;
        client66.ethg++ <--> Eth100M <--> router19.ethg++;
        client67.ethg++ <--> Eth100M <--> router19.ethg++;
        client68.ethg++ <--> Eth100M <--> router19.ethg++;
        client69.ethg++ <--> Eth100M <--> router19.ethg++;
        client70.ethg++ <--> Eth100M <--> router19.ethg++;
        router11.ethg++ <--> Eth100M <--> router20.ethg++;
        router20.ethg++ <--> Eth100M <--> router21.ethg++;
        router21.ethg++ <--> Eth100M <--> router22.ethg++;
        router22.ethg++ <--> Eth100M <--> router23.ethg++;
        router20.ethg++ <--> Eth100M <--> router23.ethg++;
        client85.ethg++ <--> Eth100M <--> router23.ethg++;
        router20.ethg++ <--> Eth100M <--> client84.ethg++;
        client84.ethg++ <--> Eth100M <--> client83.ethg++;
        client71.ethg++ <--> Eth100M <--> router20.ethg++;
        client93.ethg++ <--> Eth100M <--> router21.ethg++;
        router21.ethg++ <--> Eth100M <--> client82.ethg++;
        router21.ethg++ <--> Eth100M <--> client92.ethg++;
        router21.ethg++ <--> Eth100M <--> client91.ethg++;
        client87.ethg++ <--> Eth100M <--> router22.ethg++;
        client73.ethg++ <--> Eth100M <--> router22.ethg++;
        client88.ethg++ <--> Eth100M <--> router22.ethg++;
        client81.ethg++ <--> Eth100M <--> router22.ethg++;
        client74.ethg++ <--> Eth100M <--> router22.ethg++;
        client86.ethg++ <--> Eth100M <--> router22.ethg++;
        client72.ethg++ <--> Eth100M <--> router22.ethg++;
        client89.ethg++ <--> Eth100M <--> router22.ethg++;
        client76.ethg++ <--> Eth100M <--> router22.ethg++;
        client80.ethg++ <--> Eth100M <--> router22.ethg++;
        client90.ethg++ <--> Eth100M <--> router22.ethg++;
        client96.ethg++ <--> Eth100M <--> router22.ethg++;
        client95.ethg++ <--> Eth100M <--> router22.ethg++;
        router22.ethg++ <--> Eth100M <--> client94.ethg++;
        router22.ethg++ <--> Eth100M <--> client77.ethg++;
        router22.ethg++ <--> Eth100M <--> client79.ethg++;
        router22.ethg++ <--> Eth100M <--> client97.ethg++;
        router22.ethg++ <--> Eth100M <--> client78.ethg++;
        router22.ethg++ <--> Eth100M <--> client75.ethg++;
        router23.ethg++ <--> Eth100M <--> client99.ethg++;
        router23.ethg++ <--> Eth100M <--> client98.ethg++;
}
