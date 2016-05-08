require(library /home/comnetsii/elements/routerport.click);

// ---Router 1---

// rp_1@0::RouterPort(DEV veth2, IN_MAC 0a:03:e0:12:1a:7f, OUT_MAC 7a:cd:ae:7d:e6:e6);
// rp_1@0::RouterPort(DEV veth2, IN_MAC 7a:cd:ae:7d:e6:e6, OUT_MAC 0a:03:e0:12:1a:7f);

rp_1@0::RouterPort(DEV veth1, IN_MAC 7a:cd:ae:7d:e6:e6, OUT_MAC 0a:03:e0:12:1a:7f);
rp_2@0::RouterPort(DEV veth2, IN_MAC 0a:03:e0:12:1a:7f, OUT_MAC 7a:cd:ae:7d:e6:e6);

rt_1:: TableKN();
// ugen_1::UpdatepacketGen();
pcg_1::ClassifierAndGenerator(MY_HOST 900, ROUTINGTABLE rt_1);
fw_1::Forwarding(ROUTINGTABLE rt_1);
hproc_1::HelloProcessing(ROUTINGTABLE rt_1);
uproc_1::UpdateProcessing(ROUTINGTABLE rt_1);
sw_1::McSwitch(ROUTINGTABLE rt_1);

rp_1@0->pcg_1;
	pcg_1[0]->[0]sw_1;
	pcg_1[1]->[1]sw_1;
	pcg_1[2]->fw_1->[2]sw_1;
	pcg_1[3]->hproc_1;
	pcg_1[4]->uproc_1;
sw_1[0]->rp_1@0;

// ---Router 2---
rt_2:: TableKN();
pcg_2::ClassifierAndGenerator(MY_HOST 950, ROUTINGTABLE rt_2);
fw_2::Forwarding(ROUTINGTABLE rt_2);
hproc_2::HelloProcessing(ROUTINGTABLE rt_2);
uproc_2::UpdateProcessing(ROUTINGTABLE rt_2);
sw_2::McSwitch(ROUTINGTABLE rt_2);

rp_2@0->pcg_2;
        pcg_2[0]->[0]sw_2;
        pcg_2[1]->[1]sw_2;
        pcg_2[2]->fw_2->[2]sw_2;
        pcg_2[3]->hproc_2;
        pcg_2[4]->uproc_2;
sw_2[0]->rp_2@0;

