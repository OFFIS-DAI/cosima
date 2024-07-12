package networks;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import simu5g.common.binder.Binder;
import simu5g.common.carrierAggregation.CarrierAggregation;
import simu5g.nodes.Upf;
import simu5g.nodes.NR.gNodeB;
import simu5g.nodes.NR.NRUe;
import simu5g.nodes.backgroundCell.BackgroundCell;
import simu5g.world.radio.LteChannelControl;
import modules.CosimaSchedulerModule;
import modules.CosimaScenarioManager;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;


network SimbenchNetwork5G {
parameters: @display("i=block/network2;bgb=517.3986958301393,730.220225494355");
submodules:
    
     scenarioManager: CosimaScenarioManager {
            @display("p=216.6825,136.52249");
     }
    schedulerModule: CosimaSchedulerModule {
            @display("p=68.8875,129.00749");
    }

	channelControl: LteChannelControl{@display("p=100,100");}
	routingRecorder: RoutingTableRecorder{@display("p=100,100");}
	configurator: Ipv4NetworkConfigurator{@display("p=100,100");}
	binder: Binder{@display("p=100,100");}
	carrierAggregation: CarrierAggregation{@display("p=100,100");}
	server: StandardHost{@display("p=100,100");}
	router: Router{@display("p=100,100");}
	upf: Upf{@display("p=100,100");}
	iUpf: Upf{@display("p=100,100");}
	gNB: gNodeB{@display("p=258,365");}
	bgCell: BackgroundCell{@display("p=248,355");}
	household_agent_0: NRUe {@display("p=349,374");}
	household_agent_1: NRUe {@display("p=523,420");}
	household_agent_2: NRUe {@display("p=523,420");}
	household_agent_3: NRUe {@display("p=195,594");}
	household_agent_4: NRUe {@display("p=195,594");}
	household_agent_5: NRUe {@display("p=195,594");}
	household_agent_6: NRUe {@display("p=88,383");}
	household_agent_7: NRUe {@display("p=88,383");}
	household_agent_8: NRUe {@display("p=491,420");}
	household_agent_9: NRUe {@display("p=491,420");}
	household_agent_10: NRUe {@display("p=350,297");}
	household_agent_11: NRUe {@display("p=350,297");}
	household_agent_12: NRUe {@display("p=350,297");}
	household_agent_13: NRUe {@display("p=379,740");}
	household_agent_14: NRUe {@display("p=379,740");}
	household_agent_15: NRUe {@display("p=379,740");}
	household_agent_16: NRUe {@display("p=490,508");}
	household_agent_17: NRUe {@display("p=426,297");}
	household_agent_18: NRUe {@display("p=426,297");}
	household_agent_19: NRUe {@display("p=346,739");}
	storage_agent_0: NRUe {@display("p=1072,11");}
	storage_agent_1: NRUe {@display("p=762,517");}
	storage_agent_2: NRUe {@display("p=10,533");}
	storage_agent_3: NRUe {@display("p=983,143");}
	storage_agent_4: NRUe {@display("p=906,286");}
	storage_agent_5: NRUe {@display("p=297,115");}
	storage_agent_6: NRUe {@display("p=926,508");}
	storage_agent_7: NRUe {@display("p=411,703");}
	storage_agent_8: NRUe {@display("p=564,704");}
	storage_agent_9: NRUe {@display("p=837,706");}
	storage_agent_10: NRUe {@display("p=684,705");}
	storage_agent_11: NRUe {@display("p=590,184");}
	storage_agent_12: NRUe {@display("p=763,363");}
	storage_agent_13: NRUe {@display("p=1028,11");}
	storage_agent_14: NRUe {@display("p=369,592");}
	generation_agent_0: NRUe {@display("p=384,116");}
	generation_agent_1: NRUe {@display("p=640,716");}
	generation_agent_2: NRUe {@display("p=666,284");}
	generation_agent_3: NRUe {@display("p=566,460");}
	generation_agent_4: NRUe {@display("p=455,714");}
	generation_agent_5: NRUe {@display("p=260,535");}
	generation_agent_6: NRUe {@display("p=315,536");}
	generation_agent_7: NRUe {@display("p=335,713");}
	generation_agent_8: NRUe {@display("p=1111,520");}
	generation_agent_9: NRUe {@display("p=869,707");}
	generation_agent_10: NRUe {@display("p=984,10");}
	generation_agent_11: NRUe {@display("p=565,571");}
	generation_agent_12: NRUe {@display("p=86,534");}
	generation_agent_13: NRUe {@display("p=653,516");}
	generation_agent_14: NRUe {@display("p=340,115");}
	control_center_agent: StandardHost {@display("p=100,100");}
	market_agent: StandardHost {@display("p=100,100");}
	aggregator_agent: NRUe {@display("p=258,365");}
	pdc_agent: NRUe {@display("p=258,365");}
	grid_operator_agent: StandardHost {@display("p=100,100");}
	substation_agent: NRUe {@display("p=258,365");}
connections:
	server.pppg++ <--> Eth10G <--> router.pppg++;
	router.pppg++ <--> Eth10G <--> upf.filterGate;
	upf.pppg++ <--> Eth10G <--> iUpf.pppg++;
	iUpf.pppg++ <--> Eth10G <--> gNB.ppp;
	router.pppg++ <--> Eth10G <--> control_center_agent.pppg++;
	router.pppg++ <--> Eth10G <--> market_agent.pppg++;
	router.pppg++ <--> Eth10G <--> grid_operator_agent.pppg++;
}