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
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.Eth10M;
import inet.node.ethernet.EtherLink;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import modules.CosimaSchedulerModule;
import modules.CosimaScenarioManager;


network DERNetwork
{
    @display("bgb=6962.763,4474.325");
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

        visualizer: IntegratedCanvasVisualizer {
            @display("p=98,209");
        }

        client0: StandardHost {
            @display("p=1831.8795,1604.921;i=abstract/pv;is=n");
        }
        client1: StandardHost {
            @display("p=875.41144,1888.6191;i=abstract/pv;is=n");
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
            @display("p=2868.1572,1777.0105;i=abstract/pv;is=n");
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
            @display("p=2562.6362,1739.5997;i=abstract/pv;is=n");
        }
        client12: StandardHost {
            @display("p=2091.2607,1839.9852;i=abstract/pv;is=n");
        }
        client13: StandardHost {
            @display("p=1045.8734,274.09097;i=abstract/pv;is=n");
        }
        client14: StandardHost {
            @display("p=2139.8948,2593.8118;i=abstract/pv;is=n");
        }
        client15: StandardHost {
            @display("p=445.8114,1969.6758;i=abstract/pv;is=n");
        }
        client16: StandardHost {
            @display("p=147.86487,706.86615;i=abstract/pv;is=n");
        }
        client17: StandardHost {
            @display("p=652.7693,569.8207;i=abstract/pv;is=n");
        }
        client18: StandardHost {
            @display("p=193.28886,1353.022;i=abstract/pv;is=n");
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
            @display("p=1669.7664,1888.6191;is=l");
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
            @display("p=1669.7664,1086.1587;is=l");
        }
        router29: Router {
            @display("p=1386.0681,1313.1172;is=l");
        }
        router30: Router {
            @display("p=1045.6304,1353.6455;is=l");
        }
        router31: Router {
            @display("p=1418.4908,964.5737;is=l");
        }
        router32: Router {
            @display("p=1256.3776,697.0869;is=l");
        }

        switch0: EtherSwitch {
            @display("p=1871.7528,1316.3579;is=l");
        }
        switch1: EtherSwitch {
            @display("p=450.8075,757.3566;is=l");
        }
        switch2: EtherSwitch {
            @display("p=1256.3776,1775.1399;is=l");
        }
        switch3: EtherSwitch {
            @display("p=2098.9597,454.41397;is=l");
        }
        switch4: EtherSwitch {
            @display("p=1420.9453,270.4845;is=l");
        }
        switch5: EtherSwitch {
            @display("p=2868.1572,1502.665;is=l");
        }
        switch6: EtherSwitch {
            @display("p=3444.9062,1313.1172;is=l");
        }
        switch7: EtherSwitch {
            @display("p=492.57483,1234.5547;is=l");
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
            @display("p=2562.6362,1502.665;is=l");
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
            @display("p=648.45294,1604.921;is=l");
        }
        switch19: EtherSwitch {
            @display("p=1831.8795,2269.5852;is=l");
        }


        router1: Router {
            @display("p=3204.854,1608.6621;is=l");
        }
        router2: Router {
            @display("p=3678.7234,1577.4866;is=l");
        }
        router3: Router {
            @display("p=4027.8904,1889.2428;is=l");
        }
        router4: Router {
            @display("p=3223.5593,1889.2428;is=l");
        }
        router5: Router {
            @display("p=3578.9614,2082.5315;is=l");
        }
        switch20: EtherSwitch {
            @display("p=3965.5393,1190.9088;is=l");
        }
        switch21: EtherSwitch {
            @display("p=4451.879,1845.5969;is=l");
        }
        switch22: EtherSwitch {
            @display("p=3671.8647,2447.91;is=l");
        }
        switch23: EtherSwitch {
            @display("p=2868.1572,2132.4126;is=l");
        }
        client20: StandardHost {
            @display("p=4027.8904,935.2687;i=abstract/pv;is=n");
        }
        client21: StandardHost {
            @display("p=4801.046,1777.0105;i=abstract/pv;is=n");
        }
        client22: StandardHost {
            @display("p=3574.5967,2828.876;i=abstract/pv;is=n");
        }
        client23: StandardHost {
            @display("p=2562.6362,2269.5854;i=abstract/pv;is=n");
        }
        switch24: EtherSwitch {
            @display("p=2982.8835,2374.9587;is=l");
        }
        switch25: EtherSwitch {
            @display("p=3299.0044,2407.3816;is=l");
        }
        switch26: EtherSwitch {
            @display("p=4450.0083,2229.057;is=l");
        }
        switch27: EtherSwitch {
            @display("p=4108.9473,1502.665;is=l");
        }
        client24: StandardHost {
            @display("p=4451.879,1353.022;i=abstract/pv;is=n");
        }
        client25: StandardHost {
            @display("p=4798.552,2229.057;i=abstract/pv;is=n");
        }
        client26: StandardHost {
            @display("p=3096.3628,2861.2986;i=abstract/pv;is=n");
        }
        client27: StandardHost {
            @display("p=2715.3967,2682.974;i=abstract/pv;is=n");
        }
        router6: Router {
            @display("p=1418.4908,2358.7476;is=l");
        }
        router7: Router {
            @display("p=907.8341,2504.6494;is=l");
        }
        router8: Router {
            @display("p=1418.4908,3039.623;is=l");
        }
        router9: Router {
            @display("p=761.9322,3039.623;is=l");
        }
        router11: Router {
            @display("p=1134.7926,3428.6948;is=l");
        }
        switch28: EtherSwitch {
            @display("p=1669.7664,2682.974;is=l");
        }


        switch29: EtherSwitch {
            @display("p=1758.9286,3420.589;is=l");
        }


        switch30: EtherSwitch {
            @display("p=1531.9701,3688.076;is=l");
        }


        switch31: EtherSwitch {
            @display("p=632.24164,3833.978;is=l");
        }


        switch32: EtherSwitch {
            @display("p=494.44537,2593.8118;is=l");
        }


        client28: StandardHost {
            @display("p=1929.1475,2990.9893;i=abstract/pv;is=n");
        }
        client29: StandardHost {
            @display("p=2034.5211,3461.1174;i=abstract/pv;is=n");
        }
        client30: StandardHost {
            @display("p=1872.4078,3744.8157;i=abstract/pv;is=n");
        }
        client31: StandardHost {
            @display("p=875.41144,4012.3025;i=abstract/pv;is=n");
        }
        client32: StandardHost {
            @display("p=186.43022,2723.5022;i=abstract/pv;is=n");
        }
        switch33: EtherSwitch {
            @display("p=494.44537,3380.0608;is=l");
        }


        client33: StandardHost {
            @display("p=194.53587,3461.1174;i=abstract/pv;is=n");
        }
        switch34: EtherSwitch {
            @display("p=1321.2229,4012.3025;is=l");
        }


        client34: StandardHost {
            @display("p=1694.0833,4060.9365;i=abstract/pv;is=n");
        }
        client35: StandardHost {
            @display("p=3331.427,3428.6948;i=abstract/pv;is=n");
        }
        client36: StandardHost {
            @display("p=5787.4424,3039.623;i=abstract/pv;is=n");
        }
        client37: StandardHost {
            @display("p=5382.159,4077.1477;i=abstract/pv;is=n");
        }
        client38: StandardHost {
            @display("p=5787.4424,3566.4912;i=abstract/pv;is=n");
        }
        client39: StandardHost {
            @display("p=4879.6084,4263.578;i=abstract/pv;is=n");
        }
        client40: StandardHost {
            @display("p=4231.1553,3947.4573;i=abstract/pv;is=n");
        }
        router12: Router {
            @display("p=4101.465,2447.91;is=l");
        }
        router13: Router {
            @display("p=4668.8613,2593.8118;is=l");
        }
        router15: Router {
            @display("p=4879.6084,3169.3137;is=l");
        }
        router22: Router {
            @display("p=4020.4082,2990.9893;is=l");
        }
        router26: Router {
            @display("p=4450.0083,3380.0608;is=l");
        }
        switch35: EtherSwitch {
            @display("p=3598.9138,3169.3137;is=l");
        }
        switch36: EtherSwitch {
            @display("p=5260.574,3039.623;is=l");
        }
        switch37: EtherSwitch {
            @display("p=4960.665,3688.076;is=l");
        }
        switch38: EtherSwitch {
            @display("p=3761.027,3607.0195;is=l");
        }
        switch39: EtherSwitch {
            @display("p=4231.1553,3566.4912;is=l");
        }
        switch40: EtherSwitch {
            @display("p=4636.4385,3947.4573;is=l");
        }
        switch41: EtherSwitch {
            @display("p=5382.159,3509.7515;is=l");
        }
        switch42: EtherSwitch {
            @display("p=5009.299,2682.974;is=l");
        }
        client41: StandardHost {
            @display("p=3598.9138,3947.4573;i=abstract/pv;is=n");
        }
        client42: StandardHost {
            @display("p=5552.3784,2593.8118;i=abstract/pv;is=n");
        }
        router33: Router {
            @display("p=5382.159,1888.6191;is=l");
        }
        router34: Router {
            @display("p=5260.574,1450.9135;is=l");
        }
        router35: Router {
            @display("p=5868.499,1345.5398;is=l");
        }
        router36: Router {
            @display("p=5868.499,1888.6191;is=l");
        }
        switch43: EtherSwitch {
            @display("p=5260.574,932.15106;is=l");
        }
        switch44: EtherSwitch {
            @display("p=5787.4424,932.15106;is=l");
        }
        switch45: EtherSwitch {
            @display("p=5998.1895,2374.9587;is=l");
        }
        switch46: EtherSwitch {
            @display("p=6241.3594,1078.053;is=l");
        }
        switch47: EtherSwitch {
            @display("p=4798.552,1159.1096;is=l");
        }
        client43: StandardHost {
            @display("p=4450.0083,705.19257;i=abstract/pv;is=n");
        }
        client44: StandardHost {
            @display("p=5114.6724,567.3963;i=abstract/pv;is=n");
        }
        client45: StandardHost {
            @display("p=5787.4424,453.91705;i=abstract/pv;is=n");
        }
        client46: StandardHost {
            @display("p=6533.163,875.41144;i=abstract/pv;is=n");
        }
        client47: StandardHost {
            @display("p=6241.3594,2780.242;i=abstract/pv;is=n");
        }
        switch48: EtherSwitch {
            @display("p=6322.416,2075.0493;is=l");
        }
        switch49: EtherSwitch {
            @display("p=6322.416,1702.189;is=l");
        }
        client48: StandardHost {
            @display("p=6646.6426,2447.91;i=abstract/pv;is=n");
        }
        client49: StandardHost {
            @display("p=6768.2275,1604.921;i=abstract/pv;is=n");
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
        router10.ethg++ <--> Eth10M <--> router1.ethg++;
        router1.ethg++ <--> Eth10M <--> router2.ethg++;
        router2.ethg++ <--> Eth10M <--> router3.ethg++;
        router3.ethg++ <--> Eth10M <--> router5.ethg++;
        router4.pppg++ <--> Eth10M <--> router1.pppg++;
        router4.ethg++ <--> Eth10M <--> router5.ethg++;
        router2.ethg++ <--> Eth10M <--> switch20.ethg++;
        router3.ethg++ <--> Eth10M <--> switch21.ethg++;
        router5.ethg++ <--> Eth10M <--> switch22.ethg++;
        router4.ethg++ <--> Eth10M <--> switch23.ethg++;
        switch20.ethg++ <--> Eth10M <--> client20.ethg++;
        switch21.ethg++ <--> Eth10M <--> client21.ethg++;
        switch22.ethg++ <--> Eth10M <--> client22.ethg++;
        switch23.ethg++ <--> Eth10M <--> client23.ethg++;
        router2.ethg++ <--> Eth10M <--> switch27.ethg++;
        router3.ethg++ <--> Eth10M <--> switch26.ethg++;
        router5.ethg++ <--> Eth10M <--> switch25.ethg++;
        router4.ethg++ <--> Eth10M <--> switch24.ethg++;
        switch27.ethg++ <--> Eth10M <--> client24.ethg++;
        switch26.ethg++ <--> Eth10M <--> client25.ethg++;
        switch25.ethg++ <--> Eth10M <--> client26.ethg++;
        switch24.ethg++ <--> Eth10M <--> client27.ethg++;
        router6.ethg++ <--> Eth10M <--> router23.ethg++;
        router6.ethg++ <--> Eth10M <--> router8.ethg++;
        router6.ethg++ <--> Eth10M <--> router7.ethg++;
        router7.ethg++ <--> Eth10M <--> router9.ethg++;
        router9.ethg++ <--> Eth10M <--> router11.ethg++;
        router11.ethg++ <--> Eth10M <--> router8.ethg++;
        router6.ethg++ <--> Eth10M <--> switch28.ethg++;
        router8.ethg++ <--> Eth10M <--> switch29.ethg++;
        switch29.ethg++ <--> Eth10M <--> client29.ethg++;
        router11.ethg++ <--> Eth10M <--> switch30.ethg++;
        switch30.ethg++ <--> Eth10M <--> client30.ethg++;
        router11.ethg++ <--> Eth10M <--> switch31.ethg++;
        switch31.ethg++ <--> Eth10M <--> client31.ethg++;
        router7.ethg++ <--> Eth10M <--> switch32.ethg++;
        switch32.ethg++ <--> Eth10M <--> client32.ethg++;
        client33.ethg++ <--> Eth10M <--> switch33.ethg++;
        switch33.ethg++ <--> Eth10M <--> router9.ethg++;
        router11.ethg++ <--> Eth10M <--> switch34.ethg++;
        switch34.ethg++ <--> Eth10M <--> client34.ethg++;
        switch28.ethg++ <--> Eth10M <--> client28.ethg++;
        router15.ethg++ <--> Eth10M <--> router26.ethg++;
        router15.ethg++ <--> Eth10M <--> switch36.ethg++;
        router15.ethg++ <--> Eth10M <--> switch41.ethg++;
        switch35.ethg++ <--> Eth10M <--> client35.ethg++;
        switch39.ethg++ <--> Eth10M <--> client40.ethg++;
        router13.ethg++ <--> Eth10M <--> router15.ethg++;
        router13.ethg++ <--> Eth10M <--> switch42.ethg++;
        router12.ethg++ <--> Eth10M <--> router13.ethg++;
        switch40.ethg++ <--> Eth10M <--> client39.ethg++;
        router22.pppg++ <--> Eth10M <--> router12.pppg++;
        router22.ethg++ <--> Eth10M <--> router26.ethg++;
        router22.ethg++ <--> Eth10M <--> switch38.ethg++;
        router22.ethg++ <--> Eth10M <--> switch39.ethg++;
        switch37.ethg++ <--> Eth10M <--> client37.ethg++;
        switch41.ethg++ <--> Eth10M <--> client38.ethg++;
        router26.ethg++ <--> Eth10M <--> switch37.ethg++;
        router26.ethg++ <--> Eth10M <--> switch40.ethg++;
        switch36.ethg++ <--> Eth10M <--> client36.ethg++;
        router5.ethg++ <--> Eth10M <--> router12.ethg++;
        router3.ethg++ <--> Eth10M <--> router12.ethg++;
        switch35.ethg++ <--> Eth10M <--> router22.ethg++;
        client41.ethg++ <--> Eth10M <--> switch38.ethg++;
        switch42.ethg++ <--> Eth10M <--> client42.ethg++;
        router13.ethg++ <--> Eth10M <--> router33.ethg++;
        router33.ethg++ <--> Eth10M <--> router34.ethg++;
        router34.ethg++ <--> Eth10M <--> router35.ethg++;
        router35.ethg++ <--> Eth10M <--> router36.ethg++;
        router36.ethg++ <--> Eth10M <--> router33.ethg++;
        router34.ethg++ <--> Eth10M <--> switch43.ethg++;
        router34.ethg++ <--> Eth10M <--> switch47.ethg++;
        router35.ethg++ <--> Eth10M <--> switch44.ethg++;
        router35.ethg++ <--> Eth10M <--> switch46.ethg++;
        router36.ethg++ <--> Eth10M <--> switch45.ethg++;
        switch47.ethg++ <--> Eth10M <--> client43.ethg++;
        switch43.ethg++ <--> Eth10M <--> client44.ethg++;
        switch44.ethg++ <--> Eth10M <--> client45.ethg++;
        switch46.ethg++ <--> Eth10M <--> client46.ethg++;
        router36.ethg++ <--> Eth10M <--> switch49.ethg++;
        switch49.ethg++ <--> Eth10M <--> client49.ethg++;
        router36.ethg++ <--> Eth10M <--> switch48.ethg++;
        switch48.ethg++ <--> Eth10M <--> client48.ethg++;
        switch45.ethg++ <--> Eth10M <--> client47.ethg++;
}


