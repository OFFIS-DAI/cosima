// network generated from Simbench network with code 1-LV-semiurb4--2-sw (Szenario 2: zukuenftige Netze mit starker Zunahme an DEAs sowie Verbreitung von Wärmepumpen oder Elektrofahrzeuge)
package networks;
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
        eNB: eNodeB {
            @display("p=513.706,427.22;is=vl");
        }
        flexibility_agent_0: Ue {
            @display("p=730.442,615.822");
        }
        flexibility_agent_1: Ue {
            @display("p=611.654,537.672");
        }
        flexibility_agent_2: Ue {
            @display("p=634.578,437.64");
        }
        flexibility_agent_3: Ue {
            @display("p=776.29,439.724");
        }
        flexibility_agent_4: Ue {
            @display("p=776.29,439.724");
        }
        flexibility_agent_5: Ue {
            @display("p=522.042,691.888");
        }
        flexibility_agent_6: Ue {
            @display("p=522.042,691.888");
        }
        flexibility_agent_7: Ue {
            @display("p=358.448,746.072");
        }
        flexibility_agent_8: Ue {
            @display("p=359.49,634.578");
        }
        flexibility_agent_9: Ue {
            @display("p=270.92,633.536");
        }
        flexibility_agent_10: Ue {
            @display("p=270.92,633.536");
        }
        flexibility_agent_11: Ue {
            @display("p=238.618,745.03");
        }
        flexibility_agent_12: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_13: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_14: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_15: Ue {
            @display("p=774.206,627.284");
        }
        flexibility_agent_16: Ue {
            @display("p=677.3,427.22");
        }
        flexibility_agent_17: Ue {
            @display("p=677.3,427.22");
        }
        flexibility_agent_18: Ue {
            @display("p=480.362,535.588");
        }
        flexibility_agent_19: Ue {
            @display("p=480.362,535.588");
        }
        flexibility_agent_20: Ue {
            @display("p=445.976,690.846");
        }
        flexibility_agent_21: Ue {
            @display("p=732.526,438.682");
        }
        flexibility_agent_22: Ue {
            @display("p=732.526,438.682");
        }
        flexibility_agent_23: Ue {
            @display("p=556.428,536.63");
        }
        flexibility_agent_24: Ue {
            @display("p=281.34,745.03");
        }
        flexibility_agent_25: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_26: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_27: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_28: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_29: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_30: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_31: Ue {
            @display("p=655.418,537.672");
        }
        flexibility_agent_32: Ue {
            @display("p=172.972,632.494");
        }
        flexibility_agent_33: Ue {
            @display("p=819.012,439.724");
        }
        flexibility_agent_34: Ue {
            @display("p=819.012,439.724");
        }
        flexibility_agent_35: Ue {
            @display("p=819.012,439.724");
        }
        flexibility_agent_36: Ue {
            @display("p=819.012,439.724");
        }
        flexibility_agent_37: Ue {
            @display("p=325.104,745.03");
        }
        flexibility_agent_38: Ue {
            @display("p=315.726,634.578");
        }
        flexibility_agent_39: Ue {
            @display("p=478.278,690.846");
        }
        flexibility_agent_40: Ue {
            @display("p=228.198,633.536");
        }
        flexibility_agent_41: Ue {
            @display("p=194.854,743.988");
        }
        flexibility_agent_42: Ue {
            @display("p=566.848,691.888");
        }
        flexibility_agent_43: Ue {
            @display("p=653.334,692.93");
        }
        flexibility_agent_44: Ue {
            @display("p=653.334,692.93");
        }
        flexibility_agent_45: Ue {
            @display("p=609.57,692.93");
        }
        flexibility_agent_46: Ue {
            @display("p=609.57,692.93");
        }
        flexibility_agent_47: Ue {
            @display("p=130.25,632.494;i=Agent");
        }
        flexibility_agent_48: Ue {
            @display("p=513.706,548.092");
        }
        flexibility_agent_49: Ue {
            @display("p=513.706,548.092");
        }
        flexibility_agent_50: Ue {
            @display("p=820.054,317.81");
        }
        flexibility_agent_51: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_52: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_53: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_54: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_55: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_56: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_57: Ue {
            @display("p=635.62,238.618");
        }
        flexibility_agent_58: Ue {
            @display("p=581.436,238.618");
        }
        flexibility_agent_59: Ue {
            @display("p=581.436,238.618");
        }
        flexibility_agent_60: Ue {
            @display("p=635.62,328.23");
        }
        flexibility_agent_61: Ue {
            @display("p=580.394,327.188");
        }
        flexibility_agent_62: Ue {
            @display("p=536.63,327.188");
        }
        flexibility_agent_63: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_64: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_65: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_66: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_67: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_68: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_69: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_70: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_71: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_72: Ue {
            @display("p=653.334,692.93");
        }
        flexibility_agent_73: Ue {
            @display("p=653.334,692.93");
        }
        flexibility_agent_74: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_75: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_76: Ue {
            @display("p=733.568,316.768");
        }
        flexibility_agent_77: Ue {
            @display("p=270.92,633.536");
        }
        flexibility_agent_78: Ue {
            @display("p=270.92,633.536");
        }
        flexibility_agent_79: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_80: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_81: Ue {
            @display("p=817.97003,627.284");
        }
        flexibility_agent_82: Ue {
            @display("p=522.042,691.888");
        }
        flexibility_agent_83: Ue {
            @display("p=522.042,691.888");
        }
        flexibility_agent_84: Ue {
            @display("p=609.57,692.93");
        }
        flexibility_agent_85: Ue {
            @display("p=609.57,692.93");
        }
        flexibility_agent_86: Ue {
            @display("p=677.3,427.22");
        }
        flexibility_agent_87: Ue {
            @display("p=677.3,427.22");
        }
        flexibility_agent_88: Ue {
            @display("p=513.706,548.092");
        }
        flexibility_agent_89: Ue {
            @display("p=513.706,548.092");
        }
        flexibility_agent_90: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_91: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_92: Ue {
            @display("p=436.598,547.05");
        }
        flexibility_agent_93: Ue {
            @display("p=732.526,438.682");
        }
        flexibility_agent_94: Ue {
            @display("p=732.526,438.682");
        }
        flexibility_agent_95: Ue {
            @display("p=776.29,439.724");
        }
        flexibility_agent_96: Ue {
            @display("p=776.29,439.724");
        }
        flexibility_agent_97: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_98: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_99: Ue {
            @display("p=777.33203,317.81");
        }
        flexibility_agent_100: Ue {
            @display("p=581.436,238.618");
        }
        flexibility_agent_101: Ue {
            @display("p=581.436,238.618");
        }
        flexibility_agent_102: Ue {
            @display("p=480.362,535.588");
        }
        flexibility_agent_103: Ue {
            @display("p=480.362,535.588");
        }

        storage_agent_0: Ue {
            @display("p=653.334,692.93");
        }
        storage_agent_1: Ue {
            @display("p=777.33203,317.81");
        }
        storage_agent_2: Ue {
            @display("p=228.198,633.536");
        }
        storage_agent_3: Ue {
            @display("p=733.568,316.768");
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

    connections:
        server.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        pgw.pppg++ <--> Eth10G <--> eNB.ppp;
        router.pppg++ <--> Eth10G <--> market_agent.pppg++;
        router.pppg++ <--> Eth10G <--> aggregator_agent.pppg++;
        router.pppg++ <--> Eth10G <--> control_system.pppg++;
        router.pppg++ <--> Eth10G <--> forecasting_agent.pppg++;
}
