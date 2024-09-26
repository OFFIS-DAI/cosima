// network generated from Simbench network with code 1-LV-semiurb4--2-sw (Szenario 2: zukuenftige Netze mit starker Zunahme an DEAs sowie Verbreitung von Wärmepumpen oder Elektrofahrzeuge)
package networks;
import inet.common.scenario.ScenarioManager;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.Ue;
import simu5g.nodes.eNodeB;
import simu5g.nodes.PgwStandard;
import simu5g.world.radio.LteChannelControl;
import modules.CosimaSchedulerModule;
import modules.CosimaScenarioManager;

network SimbenchNetwork
{
    parameters:
        @display("i=block/network2;bgb=1000,1000;bgi=background/streetmap,s");
    submodules:
        scenarioManager: CosimaScenarioManager {
            @display("p=216.6825,136.52249");
        }
        schedulerModule: CosimaSchedulerModule {
            @display("p=68.8875,129.00749");
        }
        channelControl: LteChannelControl {
            @display("p=502.244,38.554");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=373.036,35.428");
        }
        configurator: Ipv4NetworkConfigurator {
            @display("p=260.5,35.428");
        }
        binder: Binder {
            @display("p=179.224,35.428");
        }
        carrierAggregation: CarrierAggregation {
            @display("p=84.402,35.428");
        }
        server: StandardHost {
            @display("p=131.292,285.508");
        }
        router: Router {
            @display("p=270.92,281.34");
        }
        pgw: PgwStandard {
            @display("p=228.198,408.464");
        }
        eNB1: eNodeB {
            @display("p=566,408;is=vl");
        }
        eNB2: eNodeB {
            @display("p=436,777;is=vl");
        }
        flexibility_agent_0: Ue {
            @display("p=730.442,615.822;i=Agent");
        }
        flexibility_agent_1: Ue {
            @display("p=611.654,537.672;i=Agent");
        }
        flexibility_agent_2: Ue {
            @display("p=634.578,437.64;i=Agent");
        }
        flexibility_agent_3: Ue {
            @display("p=776.29,439.724;i=Agent");
        }
        flexibility_agent_4: Ue {
            @display("p=776.29,439.724;i=Agent");
        }
        flexibility_agent_5: Ue {
            @display("p=522.042,691.888;i=Agent");
        }
        flexibility_agent_6: Ue {
            @display("p=522.042,691.888;i=Agent");
        }
        flexibility_agent_7: Ue {
            @display("p=358.448,746.072;i=Agent");
        }
        flexibility_agent_8: Ue {
            @display("p=359.49,634.578;i=Agent");
        }
        flexibility_agent_9: Ue {
            @display("p=270.92,633.536;i=Agent");
        }
        flexibility_agent_10: Ue {
            @display("p=270.92,633.536;i=Agent");
        }
        flexibility_agent_11: Ue {
            @display("p=238.618,745.03;i=Agent");
        }
        flexibility_agent_12: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_13: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_14: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_15: Ue {
            @display("p=774.206,627.284;i=Agent");
        }
        flexibility_agent_16: Ue {
            @display("p=677.3,427.22;i=Agent");
        }
        flexibility_agent_17: Ue {
            @display("p=677.3,427.22;i=Agent");
        }
        flexibility_agent_18: Ue {
            @display("p=480.362,535.588;i=Agent");
        }
        flexibility_agent_19: Ue {
            @display("p=480.362,535.588;i=Agent");
        }
        flexibility_agent_20: Ue {
            @display("p=445.976,690.846;i=Agent");
        }
        flexibility_agent_21: Ue {
            @display("p=732.526,438.682;i=Agent");
        }
        flexibility_agent_22: Ue {
            @display("p=732.526,438.682;i=Agent");
        }
        flexibility_agent_23: Ue {
            @display("p=556.428,536.63;i=Agent");
        }
        flexibility_agent_24: Ue {
            @display("p=281.34,745.03;i=Agent");
        }
        flexibility_agent_25: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_26: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_27: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_28: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_29: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_30: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_31: Ue {
            @display("p=655.418,537.672;i=Agent");
        }
        flexibility_agent_32: Ue {
            @display("p=172.972,632.494;i=Agent");
        }
        flexibility_agent_33: Ue {
            @display("p=819.012,439.724;i=Agent");
        }
        flexibility_agent_34: Ue {
            @display("p=819.012,439.724;i=Agent");
        }
        flexibility_agent_35: Ue {
            @display("p=819.012,439.724;i=Agent");
        }
        flexibility_agent_36: Ue {
            @display("p=819.012,439.724;i=Agent");
        }
        flexibility_agent_37: Ue {
            @display("p=325.104,745.03;i=Agent");
        }
        flexibility_agent_38: Ue {
            @display("p=315.726,634.578;i=Agent");
        }
        flexibility_agent_39: Ue {
            @display("p=478.278,690.846;i=Agent");
        }
        flexibility_agent_40: Ue {
            @display("p=228.198,633.536;i=Agent");
        }
        flexibility_agent_41: Ue {
            @display("p=194.854,743.988;i=Agent");
        }
        flexibility_agent_42: Ue {
            @display("p=566.848,691.888;i=Agent");
        }
        flexibility_agent_43: Ue {
            @display("p=653.334,692.93;i=Agent");
        }
        flexibility_agent_44: Ue {
            @display("p=653.334,692.93;i=Agent");
        }
        flexibility_agent_45: Ue {
            @display("p=609.57,692.93;i=Agent");
        }
        flexibility_agent_46: Ue {
            @display("p=609.57,692.93;i=Agent");
        }
        flexibility_agent_47: Ue {
            @display("p=130.25,632.494;i=Agent");
        }
        flexibility_agent_48: Ue {
            @display("p=513.706,548.092;i=Agent");
        }
        flexibility_agent_49: Ue {
            @display("p=513.706,548.092;i=Agent");
        }
        flexibility_agent_50: Ue {
            @display("p=820.054,317.81;i=Agent");
        }
        flexibility_agent_51: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_52: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_53: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_54: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_55: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_56: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_57: Ue {
            @display("p=635.62,238.618;i=Agent");
        }
        flexibility_agent_58: Ue {
            @display("p=581.436,238.618;i=Agent");
        }
        flexibility_agent_59: Ue {
            @display("p=581.436,238.618;i=Agent");
        }
        flexibility_agent_60: Ue {
            @display("p=635.62,328.23;i=Agent");
        }
        flexibility_agent_61: Ue {
            @display("p=580.394,327.188;i=Agent");
        }
        flexibility_agent_62: Ue {
            @display("p=536.63,327.188;i=Agent");
        }
        flexibility_agent_63: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_64: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_65: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_66: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_67: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_68: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_69: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_70: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_71: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_72: Ue {
            @display("p=653.334,692.93;i=Agent");
        }
        flexibility_agent_73: Ue {
            @display("p=653.334,692.93;i=Agent");
        }
        flexibility_agent_74: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_75: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_76: Ue {
            @display("p=733.568,316.768;i=Agent");
        }
        flexibility_agent_77: Ue {
            @display("p=270.92,633.536;i=Agent");
        }
        flexibility_agent_78: Ue {
            @display("p=270.92,633.536;i=Agent");
        }
        flexibility_agent_79: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_80: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_81: Ue {
            @display("p=817.97003,627.284;i=Agent");
        }
        flexibility_agent_82: Ue {
            @display("p=522.042,691.888;i=Agent");
        }
        flexibility_agent_83: Ue {
            @display("p=522.042,691.888;i=Agent");
        }
        flexibility_agent_84: Ue {
            @display("p=609.57,692.93;i=Agent");
        }
        flexibility_agent_85: Ue {
            @display("p=609.57,692.93;i=Agent");
        }
        flexibility_agent_86: Ue {
            @display("p=677.3,427.22;i=Agent");
        }
        flexibility_agent_87: Ue {
            @display("p=677.3,427.22;i=Agent");
        }
        flexibility_agent_88: Ue {
            @display("p=513.706,548.092;i=Agent");
        }
        flexibility_agent_89: Ue {
            @display("p=513.706,548.092;i=Agent");
        }
        flexibility_agent_90: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_91: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_92: Ue {
            @display("p=436.598,547.05;i=Agent");
        }
        flexibility_agent_93: Ue {
            @display("p=732.526,438.682;i=Agent");
        }
        flexibility_agent_94: Ue {
            @display("p=732.526,438.682;i=Agent");
        }
        flexibility_agent_95: Ue {
            @display("p=776.29,439.724;i=Agent");
        }
        flexibility_agent_96: Ue {
            @display("p=776.29,439.724;i=Agent");
        }
        flexibility_agent_97: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_98: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_99: Ue {
            @display("p=777.33203,317.81;i=Agent");
        }
        flexibility_agent_100: Ue {
            @display("p=581.436,238.618;i=Agent");
        }
        flexibility_agent_101: Ue {
            @display("p=581.436,238.618;i=Agent");
        }
        flexibility_agent_102: Ue {
            @display("p=480.362,535.588;i=Agent");
        }
        flexibility_agent_103: Ue {
            @display("p=480.362,535.588;i=Agent");
        }
        market_agent: StandardHost {
            @display("p=205.274,211.526");
        }
        forecasting_agent: StandardHost {
            @display("p=205.274,211.526");
        }
        aggregator_agent: StandardHost {
            @display("p=355.322,225.072");
        }

        control_system: StandardHost {
            @display("p=315.25598,146.584");
        }

        attacker_0: Ue {
            @display("p=762,506;i=,#EF2929");
        }
        attacker_1: Ue {
            @display("p=373,482;i=,#EF2929");
        }
        attacker_2: Ue {
            @display("p=874,389;i=,#EF2929");
        }
        attacker_3: Ue {
            @display("p=748,254;i=,#EF2929");
        }
        attacker_4: Ue {
            @display("p=777,721;i=,#EF2929");
        }
        attacker_5: Ue {
            @display("p=940,420;i=,#EF2929");
        }
        attacker_6: Ue {
            @display("p=503,627;i=,#EF2929");
        }
        attacker_7: Ue {
            @display("p=893,721;i=,#EF2929");
        }
        attacker_8: Ue {
            @display("p=595,615;i=,#EF2929");
        }
        attacker_9: Ue {
            @display("p=551,762;i=,#EF2929");
        }
        attacker_10: Ue {
            @display("p=445,359;i=,#EF2929");
        }
        attacker_11: Ue {
            @display("p=451,470;i=,#EF2929");
        }
        attacker_12: Ue {
            @display("p=922,506;i=,#EF2929");
        }
        attacker_13: Ue {
            @display("p=846,548;i=,#EF2929");
        }
        attacker_14: Ue {
            @display("p=684,777;i=,#EF2929");
        }
        attacker_15: Ue {
            @display("p=98,482;i=,#EF2929");
        }
        attacker_16: Ue {
            @display("p=68,568;i=,#EF2929");
        }
        attacker_17: Ue {
            @display("p=130,700;i=,#EF2929");
        }
        attacker_18: Ue {
            @display("p=864,870;i=,#EF2929");
        }
        attacker_19: Ue {
            @display("p=774,848;i=,#EF2929");
        }
        attacker_20: Ue {
            @display("p=172,475;i=,#EF2929");
        }
        attacker_21: Ue {
            @display("p=911,793;i=,#EF2929");
        }
        attacker_22: Ue {
            @display("p=358,348;i=,#EF2929");
        }
        attacker_23: Ue {
            @display("p=882,242;i=,#EF2929");
        }
        attacker_24: Ue {
            @display("p=929,359;i=,#EF2929");
        }
        attacker_25: Ue {
            @display("p=465,282;i=,#EF2929");
        }
        attacker_26: Ue {
            @display("p=846,682;i=,#EF2929");
        }
        attacker_27: Ue {
            @display("p=358,808;i=,#EF2929");
        }
        attacker_28: Ue {
            @display("p=503,746;i=,#EF2929");
        }
        attacker_29: Ue {
            @display("p=179,548;i=,#EF2929");
        }
        attacker_30: Ue {
            @display("p=281,682;i=,#EF2929");
        }
        attacker_31: Ue {
            @display("p=846,799;i=,#EF2929");
        }
        attacker_32: Ue {
            @display("p=684,848;i=,#EF2929");
        }
        attacker_33: Ue {
            @display("p=270,527;i=,#EF2929");
        }
        attacker_34: Ue {
            @display("p=503,827;i=,#EF2929");
        }
        attacker_35: Ue {
            @display("p=536,482;i=,#EF2929");
        }
        attacker_36: Ue {
            @display("p=820,254;i=,#EF2929");
        }
        attacker_37: Ue {
            @display("p=911,296;i=,#EF2929");
        }
        attacker_38: Ue {
            @display("p=730,377;i=,#EF2929");
        }
        attacker_39: Ue {
            @display("p=791,377;i=,#EF2929");
        }
        attacker_40: Ue {
            @display("p=281,451;i=,#EF2929");
        }
        attacker_41: Ue {
            @display("p=663,597;i=,#EF2929");
        }
        attacker_42: Ue {
            @display("p=325,566;i=,#EF2929");
        }
        attacker_43: Ue {
            @display("p=762,777;i=,#EF2929");
        }
        attacker_44: Ue {
            @display("p=580,839;i=,#EF2929");
        }
        attacker_45: Ue {
            @display("p=911,615;i=,#EF2929");
        }
        attacker_46: Ue {
            @display("p=774,566;i=,#EF2929");
        }
        attacker_47: Ue {
            @display("p=422,848;i=,#EF2929");
        }
        attacker_48: Ue {
            @display("p=345,870;i=,#EF2929");
        }
        attacker_49: Ue {
            @display("p=611,768;i=,#EF2929");
        }
    connections:
        server.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> eNB1.ppp;
        pgw.pppg++ <--> Eth10G <--> eNB2.ppp;
        router.pppg++ <--> Eth10G <--> market_agent.pppg++;
        router.pppg++ <--> Eth10G <--> aggregator_agent.pppg++;
        router.pppg++ <--> Eth10G <--> control_system.pppg++;
        router.pppg++ <--> Eth10G <--> forecasting_agent.pppg++;
        //# X2 connections
        eNB1.x2++ <--> Eth10G <--> eNB2.x2++;
}
