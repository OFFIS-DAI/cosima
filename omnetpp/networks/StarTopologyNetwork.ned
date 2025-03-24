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

import inet.node.ethernet.Eth100M;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import modules.CosimaSchedulerModule;
import modules.CosimaScenarioManager;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;


network StarTopologyNetwork
{
    @display("bgb=1104,604");
    submodules:
        scenarioManager: CosimaScenarioManager {
            @display("p=216.6825,136.52249");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=128.31155,45.08244;is=s");
        }
        schedulerModule: CosimaSchedulerModule {
            @display("p=68.8875,129.00749");
        }
        client0: StandardHost {
            @display("p=97,326");
        }
        client1: StandardHost {
            @display("p=698,262");
        }
        client2: StandardHost {
            @display("p=325,45");
        }
        client3: StandardHost {
            @display("p=845,37");
        }
        client4: StandardHost {
            @display("p=1008,37");
        }
        client5: StandardHost {
            @display("p=1054,82");
        }
        client6: StandardHost {
            @display("p=78,389");
        }
        client7: StandardHost {
            @display("p=68,541");
        }
        client8: StandardHost {
            @display("p=328,580");
        }
        client9: StandardHost {
            @display("p=673,567");
        }
        client10: StandardHost {
            @display("p=595,528");
        }
        client11: StandardHost {
            @display("p=896,541");
        }
        client12: StandardHost {
            @display("p=995,521");
        }
        client13: StandardHost {
            @display("p=1054,473");
        }
        client14: StandardHost {
            @display("p=1065,231");
        }
        client15: StandardHost {
            @display("p=1054,153");
        }
        client16: StandardHost {
            @display("p=909,55");
        }
        client17: StandardHost {
            @display("p=969,103");
        }
        client18: StandardHost {
            @display("p=1008,350");
        }
        client19: StandardHost {
            @display("p=144,551");
        }
        client20: StandardHost {
            @display("p=97,441");
        }
        client21: StandardHost {
            @display("p=255,541");
        }
        client22: StandardHost {
            @display("p=761,567");
        }
        client23: StandardHost {
            @display("p=815,528");
        }
        client24: StandardHost {
            @display("p=988,420");
        }
        client25: StandardHost {
            @display("p=969,192");
        }
        client26: StandardHost {
            @display("p=988,270");
        }
        client27: StandardHost {
            @display("p=923,459");
        }
        client28: StandardHost {
            @display("p=917,371");
        }
        client29: StandardHost {
            @display("p=878,262");
        }
        client30: StandardHost {
            @display("p=884,103");
        }
        client31: StandardHost {
            @display("p=767,55");
        }
        client32: StandardHost {
            @display("p=683,55");
        }
        client33: StandardHost {
            @display("p=559,55");
        }
        client34: StandardHost {
            @display("p=485,55");
        }
        client35: StandardHost {
            @display("p=403,55");
        }
        client36: StandardHost {
            @display("p=311,121");
        }
        client37: StandardHost {
            @display("p=231,221");
        }
        client38: StandardHost {
            @display("p=183,262");
        }
        client39: StandardHost {
            @display("p=177,350");
        }
        client40: StandardHost {
            @display("p=192,473");
        }
        client41: StandardHost {
            @display("p=415,541");
        }
        client42: StandardHost {
            @display("p=531,541");
        }
        client43: StandardHost {
            @display("p=673,502");
        }
        client44: StandardHost {
            @display("p=806,426");
        }
        client45: StandardHost {
            @display("p=706,121");
        }
        client46: StandardHost {
            @display("p=806,136");
        }
        client47: StandardHost {
            @display("p=795,231");
        }
        client48: StandardHost {
            @display("p=795,293");
        }
        client49: StandardHost {
            @display("p=376,129");
        }
        client50: StandardHost {
            @display("p=272,293");
        }
        client51: StandardHost {
            @display("p=328,420");
        }
        client52: StandardHost {
            @display("p=364,502");
        }
        client53: StandardHost {
            @display("p=767,350");
        }
        client54: StandardHost {
            @display("p=609,420");
        }
        client55: StandardHost {
            @display("p=683,153");
        }
        client56: StandardHost {
            @display("p=464,434");
        }
        client57: StandardHost {
            @display("p=347,212");
        }
        client58: StandardHost {
            @display("p=289,365");
        }
        client59: StandardHost {
            @display("p=507,114");
        }
        client60: StandardHost {
            @display("p=566,129");
        }
        client61: StandardHost {
            @display("p=425,153");
        }
        client62: StandardHost {
            @display("p=737,182");
        }
        client63: StandardHost {
            @display("p=546,221");
        }
        client64: StandardHost {
            @display("p=559,452");
        }
        client65: StandardHost {
            @display("p=673,319");
        }
        client66: StandardHost {
            @display("p=722,420");
        }
        client67: StandardHost {
            @display("p=595,326");
        }
        client68: StandardHost {
            @display("p=716,231");
        }
        client69: StandardHost {
            @display("p=634,94");
        }
        client70: StandardHost {
            @display("p=386,381");
        }
        client71: StandardHost {
            @display("p=386,280");
        }
        client72: StandardHost {
            @display("p=347,287");
        }
        client73: StandardHost {
            @display("p=507,182");
        }
        router: Router {
            @display("p=492,315");
        }
        client74: StandardHost {
            @display("p=231,221");
        }
        client75: StandardHost {
            @display("p=308,251");
        }
        client76: StandardHost {
            @display("p=311,82");
        }
        client77: StandardHost {
            @display("p=795,82");
        }
        client78: StandardHost {
            @display("p=935,262");
        }
        client79: StandardHost {
            @display("p=935,315");
        }
        client80: StandardHost {
            @display("p=870,410");
        }
        client81: StandardHost {
            @display("p=854,480");
        }
        client82: StandardHost {
            @display("p=845,567");
        }
        client83: StandardHost {
            @display("p=433,389");
        }
        client84: StandardHost {
            @display("p=159,231");
        }
        client85: StandardHost {
            @display("p=183,182");
        }
        client86: StandardHost {
            @display("p=595,580");
        }
        client87: StandardHost {
            @display("p=520,473");
        }
        client88: StandardHost {
            @display("p=376,551");
        }
        client89: StandardHost {
            @display("p=311,512");
        }
        client90: StandardHost {
            @display("p=192,551");
        }
        client91: StandardHost {
            @display("p=136,491");
        }
        client92: StandardHost {
            @display("p=386,459");
        }
        client93: StandardHost {
            @display("p=722,29");
        }
        client94: StandardHost {
            @display("p=648,29");
        }
        client95: StandardHost {
            @display("p=433,29");
        }
        client96: StandardHost {
            @display("p=364,94");
        }
        client97: StandardHost {
            @display("p=255,121");
        }
        client98: StandardHost {
            @display("p=325,168");
        }
        client99: StandardHost {
            @display("p=595,168");
        }
        client100: StandardHost {
            @display("p=128,350");
        }
        client101: StandardHost {
            @display("p=673,389");
        }
        client102: StandardHost {
            @display("p=376,206");
        }
        client103: StandardHost {
            @display("p=442,114");
        }
        client104: StandardHost {
            @display("p=159,410");
        }
        client105: StandardHost {
            @display("p=222,365");
        }
        client106: StandardHost {
            @display("p=289,473");
        }
        client107: StandardHost {
            @display("p=745,502");
        }
        client108: StandardHost {
            @display("p=524,404");
        }
        client109: StandardHost {
            @display("p=854,326");
        }
        client110: StandardHost {
            @display("p=909,198");
        }
        client111: StandardHost {
            @display("p=845,192");
        }
        client112: StandardHost {
            @display("p=616,212");
        }
        client113: StandardHost {
            @display("p=231,293");
        }
        client114: StandardHost {
            @display("p=394,231");
        }
        client115: StandardHost {
            @display("p=58,55");
        }
        client116: StandardHost {
            @display("p=231,426");
        }
        client117: StandardHost {
            @display("p=47,473");
        }
        client118: StandardHost {
            @display("p=29,350");
        }
        client119: StandardHost {
            @display("p=47,206");
        }
        client120: StandardHost {
            @display("p=278,206");
        }
        client121: StandardHost {
            @display("p=183,37");
        }
        client122: StandardHost {
            @display("p=609,37");
        }
        client123: StandardHost {
            @display("p=1047,315");
        }
        client124: StandardHost {
            @display("p=1054,404");
        }
        client125: StandardHost {
            @display("p=969,567");
        }
        client126: StandardHost {
            @display("p=117,168");
        }
        client127: StandardHost {
            @display("p=468,502");
        }
        client128: StandardHost {
            @display("p=468,567");
        }
        client129: StandardHost {
            @display("p=58,262");
        }
        client130: StandardHost {
            @display("p=144,114");
        }
        client131: StandardHost {
            @display("p=255,55");
        }
        client132: StandardHost {
            @display("p=128,221");
        }
    connections:
        router.ethg++ <--> Eth100M <--> client0.ethg++;
        router.ethg++ <--> Eth100M <--> client1.ethg++;
        router.ethg++ <--> Eth100M <--> client2.ethg++;
        router.ethg++ <--> Eth100M <--> client3.ethg++;
        router.ethg++ <--> Eth100M <--> client4.ethg++;
        router.ethg++ <--> Eth100M <--> client5.ethg++;
        router.ethg++ <--> Eth100M <--> client6.ethg++;
        router.ethg++ <--> Eth100M <--> client7.ethg++;
        router.ethg++ <--> Eth100M <--> client8.ethg++;
        router.ethg++ <--> Eth100M <--> client9.ethg++;
        router.ethg++ <--> Eth100M <--> client10.ethg++;
        router.ethg++ <--> Eth100M <--> client11.ethg++;
        router.ethg++ <--> Eth100M <--> client12.ethg++;
        router.ethg++ <--> Eth100M <--> client13.ethg++;
        router.ethg++ <--> Eth100M <--> client14.ethg++;
        router.ethg++ <--> Eth100M <--> client15.ethg++;
        router.ethg++ <--> Eth100M <--> client16.ethg++;
        router.ethg++ <--> Eth100M <--> client17.ethg++;
        router.ethg++ <--> Eth100M <--> client18.ethg++;
        router.ethg++ <--> Eth100M <--> client19.ethg++;
        router.ethg++ <--> Eth100M <--> client20.ethg++;
        router.ethg++ <--> Eth100M <--> client21.ethg++;
        router.ethg++ <--> Eth100M <--> client22.ethg++;
        router.ethg++ <--> Eth100M <--> client23.ethg++;
        router.ethg++ <--> Eth100M <--> client24.ethg++;
        router.ethg++ <--> Eth100M <--> client25.ethg++;
        router.ethg++ <--> Eth100M <--> client26.ethg++;
        router.ethg++ <--> Eth100M <--> client27.ethg++;
        router.ethg++ <--> Eth100M <--> client28.ethg++;
        router.ethg++ <--> Eth100M <--> client29.ethg++;
        router.ethg++ <--> Eth100M <--> client30.ethg++;
        router.ethg++ <--> Eth100M <--> client31.ethg++;
        router.ethg++ <--> Eth100M <--> client32.ethg++;
        router.ethg++ <--> Eth100M <--> client33.ethg++;
        router.ethg++ <--> Eth100M <--> client34.ethg++;
        router.ethg++ <--> Eth100M <--> client35.ethg++;
        router.ethg++ <--> Eth100M <--> client36.ethg++;
        router.ethg++ <--> Eth100M <--> client37.ethg++;
        router.ethg++ <--> Eth100M <--> client38.ethg++;
        router.ethg++ <--> Eth100M <--> client39.ethg++;
        router.ethg++ <--> Eth100M <--> client40.ethg++;
        router.ethg++ <--> Eth100M <--> client41.ethg++;
        router.ethg++ <--> Eth100M <--> client42.ethg++;
        router.ethg++ <--> Eth100M <--> client43.ethg++;
        router.ethg++ <--> Eth100M <--> client44.ethg++;
        router.ethg++ <--> Eth100M <--> client45.ethg++;
        router.ethg++ <--> Eth100M <--> client46.ethg++;
        router.ethg++ <--> Eth100M <--> client47.ethg++;
        router.ethg++ <--> Eth100M <--> client48.ethg++;
        router.ethg++ <--> Eth100M <--> client49.ethg++;
        router.ethg++ <--> Eth100M <--> client50.ethg++;
        router.ethg++ <--> Eth100M <--> client51.ethg++;
        router.ethg++ <--> Eth100M <--> client52.ethg++;
        router.ethg++ <--> Eth100M <--> client53.ethg++;
        router.ethg++ <--> Eth100M <--> client54.ethg++;
        router.ethg++ <--> Eth100M <--> client55.ethg++;
        router.ethg++ <--> Eth100M <--> client56.ethg++;
        router.ethg++ <--> Eth100M <--> client57.ethg++;
        router.ethg++ <--> Eth100M <--> client58.ethg++;
        router.ethg++ <--> Eth100M <--> client59.ethg++;
        router.ethg++ <--> Eth100M <--> client60.ethg++;
        router.ethg++ <--> Eth100M <--> client61.ethg++;
        router.ethg++ <--> Eth100M <--> client62.ethg++;
        router.ethg++ <--> Eth100M <--> client63.ethg++;
        router.ethg++ <--> Eth100M <--> client64.ethg++;
        router.ethg++ <--> Eth100M <--> client65.ethg++;
        router.ethg++ <--> Eth100M <--> client66.ethg++;
        router.ethg++ <--> Eth100M <--> client67.ethg++;
        router.ethg++ <--> Eth100M <--> client68.ethg++;
        router.ethg++ <--> Eth100M <--> client69.ethg++;
        router.ethg++ <--> Eth100M <--> client70.ethg++;
        router.ethg++ <--> Eth100M <--> client71.ethg++;
        router.ethg++ <--> Eth100M <--> client72.ethg++;
        router.ethg++ <--> Eth100M <--> client73.ethg++;
        router.ethg++ <--> Eth100M <--> client74.ethg++;
        router.ethg++ <--> Eth100M <--> client75.ethg++;
        router.ethg++ <--> Eth100M <--> client76.ethg++;
        router.ethg++ <--> Eth100M <--> client77.ethg++;
        router.ethg++ <--> Eth100M <--> client78.ethg++;
        router.ethg++ <--> Eth100M <--> client79.ethg++;
        router.ethg++ <--> Eth100M <--> client80.ethg++;
        router.ethg++ <--> Eth100M <--> client81.ethg++;
        router.ethg++ <--> Eth100M <--> client82.ethg++;
        router.ethg++ <--> Eth100M <--> client83.ethg++;
        router.ethg++ <--> Eth100M <--> client84.ethg++;
        router.ethg++ <--> Eth100M <--> client85.ethg++;
        router.ethg++ <--> Eth100M <--> client86.ethg++;
        router.ethg++ <--> Eth100M <--> client87.ethg++;
        router.ethg++ <--> Eth100M <--> client88.ethg++;
        router.ethg++ <--> Eth100M <--> client89.ethg++;
        router.ethg++ <--> Eth100M <--> client90.ethg++;
        router.ethg++ <--> Eth100M <--> client91.ethg++;
        router.ethg++ <--> Eth100M <--> client92.ethg++;
        router.ethg++ <--> Eth100M <--> client93.ethg++;
        router.ethg++ <--> Eth100M <--> client94.ethg++;
        router.ethg++ <--> Eth100M <--> client95.ethg++;
        router.ethg++ <--> Eth100M <--> client96.ethg++;
        router.ethg++ <--> Eth100M <--> client97.ethg++;
        router.ethg++ <--> Eth100M <--> client98.ethg++;
        router.ethg++ <--> Eth100M <--> client99.ethg++;
        router.ethg++ <--> Eth100M <--> client100.ethg++;
        router.ethg++ <--> Eth100M <--> client101.ethg++;
        router.ethg++ <--> Eth100M <--> client102.ethg++;
        router.ethg++ <--> Eth100M <--> client103.ethg++;
        router.ethg++ <--> Eth100M <--> client104.ethg++;
        router.ethg++ <--> Eth100M <--> client105.ethg++;
        router.ethg++ <--> Eth100M <--> client106.ethg++;
        router.ethg++ <--> Eth100M <--> client107.ethg++;
        router.ethg++ <--> Eth100M <--> client108.ethg++;
        router.ethg++ <--> Eth100M <--> client109.ethg++;
        router.ethg++ <--> Eth100M <--> client110.ethg++;
        router.ethg++ <--> Eth100M <--> client111.ethg++;
        router.ethg++ <--> Eth100M <--> client112.ethg++;
        router.ethg++ <--> Eth100M <--> client113.ethg++;
        router.ethg++ <--> Eth100M <--> client114.ethg++;
        router.ethg++ <--> Eth100M <--> client115.ethg++;
        router.ethg++ <--> Eth100M <--> client116.ethg++;
        router.ethg++ <--> Eth100M <--> client117.ethg++;
        router.ethg++ <--> Eth100M <--> client118.ethg++;
        router.ethg++ <--> Eth100M <--> client119.ethg++;
        router.ethg++ <--> Eth100M <--> client120.ethg++;
        router.ethg++ <--> Eth100M <--> client121.ethg++;
        router.ethg++ <--> Eth100M <--> client122.ethg++;
        router.ethg++ <--> Eth100M <--> client123.ethg++;
        router.ethg++ <--> Eth100M <--> client124.ethg++;
        router.ethg++ <--> Eth100M <--> client125.ethg++;
        router.ethg++ <--> Eth100M <--> client126.ethg++;
        router.ethg++ <--> Eth100M <--> client127.ethg++;
        router.ethg++ <--> Eth100M <--> client128.ethg++;
        router.ethg++ <--> Eth100M <--> client129.ethg++;
        router.ethg++ <--> Eth100M <--> client130.ethg++;
        router.ethg++ <--> Eth100M <--> client131.ethg++;
        router.ethg++ <--> Eth100M <--> client132.ethg++;

}
