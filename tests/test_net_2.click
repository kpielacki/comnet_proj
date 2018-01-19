require(library /home/comnetsii/elements/routerport.click);

// rp_1@0::RouterPort(DEV veth1, IN_MAC 7a:cd:ae:7d:e6:e6, OUT_MAC 0a:03:e0:12:1a:7f);
// rp_2@0::RouterPort(DEV veth2, IN_MAC 0a:03:e0:12:1a:7f, OUT_MAC 7a:cd:ae:7d:e6:e6);

// Element Configuration:

// ---Router 1---
rt_1:: TableKN(MY_HOST 900);
pcg_1::ClassifierAndGenerator(ROUTINGTABLE rt_1);
fw_1::Forwarding(ROUTINGTABLE rt_1);
hproc_1::HelloProcessing(ROUTINGTABLE rt_1);
uproc_1::UpdateProcessing(ROUTINGTABLE rt_1);
sw_1::McSwitch(ROUTINGTABLE rt_1);

// ---Router 2---
rt_2:: TableKN(MY_HOST 950);
pcg_2::ClassifierAndGenerator(ROUTINGTABLE rt_2);
fw_2::Forwarding(ROUTINGTABLE rt_2);
hproc_2::HelloProcessing(ROUTINGTABLE rt_2);
uproc_2::UpdateProcessing(ROUTINGTABLE rt_2);
sw_2::McSwitch(ROUTINGTABLE rt_2);



// Network Configuration:
// 	Naming Convention:
//		rp_PortNum@DeviceNum
//	Ethernet Settings:
//	vethN	Router N for 1<=N<7
//	veth7	Source
//	veth8	Destination 1
//	veth9	Destination 2
//	veth10	Destination 3

// Port 0 Router 1 to Router 2
rp_0@1::RouterPort(DEV veth1, IN_MAC $v1_mac, OUT_MAC $v2_mac);
rp_0@2::RouterPort(DEV veth2, IN_MAC $v2_mac, OUT_MAC $v1_mac);

rp_0@1->pcg_1;
        pcg_1[0]->[0]sw_1;
        pcg_1[1]->[1]sw_1;
        pcg_1[2]->fw_1->[2]sw_1;
        pcg_1[3]->hproc_1;
        pcg_1[4]->uproc_1;
sw_1[0]->rp_0@1;

rp_0@2->pcg_2;
        pcg_2[0]->[0]sw_2;
        pcg_2[1]->[1]sw_2;
        pcg_2[2]->fw_2->[2]sw_2;
        pcg_2[3]->hproc_2;
        pcg_2[4]->uproc_2;
sw_2[0]->rp_0@2;
