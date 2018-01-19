require(library /home/comnetsii/elements/routerport.click);

// Element Configuration
// 		Naming Convention:
//			element@Device[Name]Number		No Name for Routers

// ---Router 1---
rt@1:: TableKN(MY_HOST 1001, IF_SENDER false);

pcg@1::ClassifierAndGenerator(ROUTINGTABLE rt@1);
fw@1::Forwarding(ROUTINGTABLE rt@1);
hproc@1::HelloProcessing(ROUTINGTABLE rt@1);
uproc@1::UpdateProcessing(ROUTINGTABLE rt@1);
sw@1::McSwitch(ROUTINGTABLE rt@1);

// ---Router 2---
rt@2:: TableKN(MY_HOST 1002, IF_SENDER false);

pcg@2::ClassifierAndGenerator(ROUTINGTABLE rt@2);
fw@2::Forwarding(ROUTINGTABLE rt@2);
hproc@2::HelloProcessing(ROUTINGTABLE rt@2);
uproc@2::UpdateProcessing(ROUTINGTABLE rt@2);
sw@2::McSwitch(ROUTINGTABLE rt@2);

// ---Router 3---
rt@3:: TableKN(MY_HOST 1003, IF_SENDER false);

pcg@3::ClassifierAndGenerator(ROUTINGTABLE rt@3);
fw@3::Forwarding(ROUTINGTABLE rt@3);
hproc@3::HelloProcessing(ROUTINGTABLE rt@3);
uproc@3::UpdateProcessing(ROUTINGTABLE rt@3);
sw@3::McSwitch(ROUTINGTABLE rt@3);

// ---Router 4---
rt@4:: TableKN(MY_HOST 1004, IF_SENDER false);

pcg@4::ClassifierAndGenerator(ROUTINGTABLE rt@4);
fw@4::Forwarding(ROUTINGTABLE rt@4);
hproc@4::HelloProcessing(ROUTINGTABLE rt@4);
uproc@4::UpdateProcessing(ROUTINGTABLE rt@4);
sw@4::McSwitch(ROUTINGTABLE rt@4);

// ---Router 5---
rt@5:: TableKN(MY_HOST 1005, IF_SENDER false);

pcg@5::ClassifierAndGenerator(ROUTINGTABLE rt@5);
fw@5::Forwarding(ROUTINGTABLE rt@5);
hproc@5::HelloProcessing(ROUTINGTABLE rt@5);
uproc@5::UpdateProcessing(ROUTINGTABLE rt@5);
sw@5::McSwitch(ROUTINGTABLE rt@5);

// ---Router 6---
rt@6:: TableKN(MY_HOST 1006, IF_SENDER false);

pcg@6::ClassifierAndGenerator(ROUTINGTABLE rt@6);
fw@6::Forwarding(ROUTINGTABLE rt@6);
hproc@6::HelloProcessing(ROUTINGTABLE rt@6);
uproc@6::UpdateProcessing(ROUTINGTABLE rt@6);
sw@6::McSwitch(ROUTINGTABLE rt@6);

// ---Source ---
rt@source:: TableKN(MY_HOST 601, IF_SENDER false);

pcg@source::ClassifierAndGenerator(ROUTINGTABLE rt@source);
fw@source::Forwarding(ROUTINGTABLE rt@source);
hproc@source::HelloProcessing(ROUTINGTABLE rt@source);
uproc@source::UpdateProcessing(ROUTINGTABLE rt@source);
sw@source::McSwitch(ROUTINGTABLE rt@source);

// ---Destination 1---
rt@dest1:: TableKN(MY_HOST 60001, IF_SENDER false);

pcg@dest1::ClassifierAndGenerator(ROUTINGTABLE rt@dest1);
fw@dest1::Forwarding(ROUTINGTABLE rt@dest1);
hproc@dest1::HelloProcessing(ROUTINGTABLE rt@dest1);
uproc@dest1::UpdateProcessing(ROUTINGTABLE rt@dest1);
sw@dest1::McSwitch(ROUTINGTABLE rt@dest1);

// ---Destination 2---
rt@dest2:: TableKN(MY_HOST 60002, IF_SENDER false);

pcg@dest2::ClassifierAndGenerator(ROUTINGTABLE rt@dest2);
fw@dest2::Forwarding(ROUTINGTABLE rt@dest2);
hproc@dest2::HelloProcessing(ROUTINGTABLE rt@dest2);
uproc@dest2::UpdateProcessing(ROUTINGTABLE rt@dest2);
sw@dest2::McSwitch(ROUTINGTABLE rt@dest2);

// ---Destination 3---
rt@dest3:: TableKN(MY_HOST 60003, IF_SENDER false);

pcg@dest3::ClassifierAndGenerator(ROUTINGTABLE rt@dest3);
fw@dest3::Forwarding(ROUTINGTABLE rt@dest3);
hproc@dest3::HelloProcessing(ROUTINGTABLE rt@dest3);
uproc@dest3::UpdateProcessing(ROUTINGTABLE rt@dest3);
sw@dest3::McSwitch(ROUTINGTABLE rt@dest3);

// Network Configuration:
// 		Naming Convention:
// 			rp_PortNum@DeviceNum
// 			Ethernet Settings:
// 			vethN   Router N for 1<=N<7
// 			veth7   Source
// 			veth8   Destination 1
// 			veth9   Destination 2
// 			veth10  Destination 3


// Port 0 Router 1 to Router 2
rp_0@1::RouterPort(DEV veth1, IN_MAC $v1_mac, OUT_MAC $v2_mac);
rp_0@2::RouterPort(DEV veth2, IN_MAC $v2_mac, OUT_MAC $v1_mac);

rp_0@1->[0]pcg@1;
        pcg@1[0]->[0]sw@1;
        pcg@1[1]->[1]sw@1;
        pcg@1[2]->fw@1->[2]sw@1;
        pcg@1[3]->hproc@1;
        pcg@1[4]->uproc@1;
sw@1[0]->rp_0@1;

rp_0@2->[0]pcg@2;
        pcg@2[0]->[0]sw@2;
        pcg@2[1]->[1]sw@2;
        pcg@2[2]->fw@2->[2]sw@2;
        pcg@2[3]->hproc@2;
        pcg@2[4]->uproc@2;
sw@2[0]->rp_0@2;

// Port 1 Router 1 to Router 4
rp_1@1::RouterPort(DEV veth5, IN_MAC $v5_mac, OUT_MAC $v6_mac);
rp_1@4::RouterPort(DEV veth6, IN_MAC $v6_mac, OUT_MAC $v5_mac);

rp_1@1->[1]pcg@1;
        pcg@1[0]->[0]sw@1;
        pcg@1[1]->[1]sw@1;
        pcg@1[2]->fw@1->[2]sw@1;
        pcg@1[3]->hproc@1;
        pcg@1[4]->uproc@1;
sw@1[1]->rp_1@1;

rp_1@4->[1]pcg@4;
        pcg@4[0]->[0]sw@4;
        pcg@4[1]->[1]sw@4;
        pcg@4[2]->fw@4->[2]sw@4;
        pcg@4[3]->hproc@4;
        pcg@4[4]->uproc@4;
sw@4[1]->rp_1@4;

// Port 1 Router 2 to Router 3
rp_1@2::RouterPort(DEV veth3, IN_MAC $v2_mac, OUT_MAC $v3_mac);
rp_1@3::RouterPort(DEV veth4, IN_MAC $v3_mac, OUT_MAC $v2_mac);

rp_1@2->[1]pcg@2;
        pcg@2[0]->[0]sw@2;
        pcg@2[1]->[1]sw@2;
        pcg@2[2]->fw@2->[2]sw@2;
        pcg@2[3]->hproc@2;
        pcg@2[4]->uproc@2;
sw@2[1]->rp_1@2;

rp_1@3->[1]pcg@3;
        pcg@3[0]->[0]sw@3;
        pcg@3[1]->[1]sw@3;
        pcg@3[2]->fw@3->[2]sw@3;
        pcg@3[3]->hproc@3;
        pcg@3[4]->uproc@3;
sw@3[1]->rp_1@3;

// Port 0 Router 3 to Router 4
rp_0@3::RouterPort(DEV veth8, IN_MAC $v8_mac, OUT_MAC $v7_mac);
rp_0@4::RouterPort(DEV veth7, IN_MAC $v7_mac, OUT_MAC $v8_mac);

rp_0@3->[0]pcg@3;
        pcg@3[0]->[0]sw@3;
        pcg@3[1]->[1]sw@3;
        pcg@3[2]->fw@3->[2]sw@3;
        pcg@3[3]->hproc@3;
        pcg@3[4]->uproc@3;
sw@3[0]->rp_0@3;

rp_0@4->[0]pcg@4;
        pcg@4[0]->[0]sw@4;
        pcg@4[1]->[1]sw@4;
        pcg@4[2]->fw@4->[2]sw@4;
        pcg@4[3]->hproc@4;
        pcg@4[4]->uproc@4;
sw@4[0]->rp_0@4;

// Port 2 Router 4 to Router 5
rp_2@4::RouterPort(DEV veth9, IN_MAC $v9_mac, OUT_MAC $v10_mac);
rp_2@5::RouterPort(DEV veth10, IN_MAC $v10_mac, OUT_MAC $v9_mac);

rp_2@4->[2]pcg@4;
        pcg@4[0]->[0]sw@4;
        pcg@4[1]->[1]sw@4;
        pcg@4[2]->fw@4->[2]sw@4;
        pcg@4[3]->hproc@4;
        pcg@4[4]->uproc@4;
sw@4[2]->rp_2@4;

// use port zero at input of pcg and output of sw
rp_2@5->[0]pcg@5;
        pcg@5[0]->[0]sw@5;
        pcg@5[1]->[1]sw@5;
        pcg@5[2]->fw@5->[2]sw@5;
        pcg@5[3]->hproc@5;
        pcg@5[4]->uproc@5;
sw@5[0]->rp_2@5;


// Port 1 Router 5 to Router 6
rp_1@5::RouterPort(DEV veth11, IN_MAC $v11_mac, OUT_MAC $v12_mac);
rp_1@6::RouterPort(DEV veth12, IN_MAC $v12_mac, OUT_MAC $v11_mac);

rp_1@5->[1]pcg@5;
        pcg@5[0]->[0]sw@5;
        pcg@5[1]->[1]sw@5;
        pcg@5[2]->fw@5->[2]sw@5;
        pcg@5[3]->hproc@5;
        pcg@5[4]->uproc@5;
sw@5[1]->rp_1@5;

// use port zero at input of pcg and output of sw
rp_1@6->[0]pcg@6;
        pcg@6[0]->[0]sw@6;
        pcg@6[1]->[1]sw@6;
        pcg@6[2]->fw@6->[2]sw@6;
        pcg@6[3]->hproc@6;
        pcg@6[4]->uproc@6;
sw@6[0]->rp_1@6;

// Client Configuration:
// Port 2 Source to Router 1
rp_2@source::RouterPort(DEV veth13, IN_MAC $v13_mac, OUT_MAC $v14_mac);
rp_2@1::RouterPort(DEV veth14, IN_MAC $v14_mac, OUT_MAC $v13_mac);

// use port zero at input of pcg and output of sw
rp_2@source->[0]pcg@source;
        pcg@source[0]->[0]sw@source;
        pcg@source[1]->[1]sw@source;
        pcg@source[2]->fw@source->[2]sw@source;
        pcg@source[3]->hproc@source;
        pcg@source[4]->uproc@source;
sw@source[0]->rp_2@source;

rp_2@1->[2]pcg@1;
        pcg@1[0]->[0]sw@1;
        pcg@1[1]->[1]sw@1;
        pcg@1[2]->fw@1->[2]sw@1;
        pcg@1[3]->hproc@1;
        pcg@1[4]->uproc@1;
sw@1[2]->rp_2@1;

// Port 2 Router 3 to Destination 1
rp_2@3::RouterPort(DEV veth15, IN_MAC $v15_mac, OUT_MAC $v16_mac);
rp_2@dest1::RouterPort(DEV veth16, IN_MAC $v16_mac, OUT_MAC $v15_mac);

rp_2@3->[2]pcg@3;
        pcg@3[0]->[0]sw@3;
        pcg@3[1]->[1]sw@3;
        pcg@3[2]->fw@3->[2]sw@3;
        pcg@3[3]->hproc@3;
        pcg@3[4]->uproc@3;
sw@3[2]->rp_2@3;

// use port zero at input of pcg and output of sw
rp_2@dest1->[0]pcg@dest1;
        pcg@dest1[0]->[0]sw@dest1;
        pcg@dest1[1]->[1]sw@dest1;
        pcg@dest1[2]->fw@dest1->[2]sw@dest1;
        pcg@dest1[3]->hproc@dest1;
        pcg@dest1[4]->uproc@dest1;
sw@dest1[0]->rp_2@dest1;

// Port 0 Router 6 to Destination 2
rp_0@6::RouterPort(DEV veth17, IN_MAC $v17_mac, OUT_MAC $v18_mac);
rp_0@dest2::RouterPort(DEV veth18, IN_MAC $v18_mac, OUT_MAC $v17_mac);

// use port one at input of pcg and output of sw
rp_0@6->[1]pcg@6;
        pcg@6[0]->[0]sw@6;
        pcg@6[1]->[1]sw@6;
        pcg@6[2]->fw@6->[2]sw@6;
        pcg@6[3]->hproc@6;
        pcg@6[4]->uproc@6;
sw@6[1]->rp_0@6;

// use port zero at input of pcg and output of sw
rp_0@dest2->[0]pcg@dest2;
        pcg@dest2[0]->[0]sw@dest2;
        pcg@dest2[1]->[1]sw@dest2;
        pcg@dest2[2]->fw@dest2->[2]sw@dest2;
        pcg@dest2[3]->hproc@dest2;
        pcg@dest2[4]->uproc@dest2;
sw@dest2[0]->rp_0@dest2;

// Port 2 Router 6 to Destination 3
rp_2@6::RouterPort(DEV veth19, IN_MAC $v19_mac, OUT_MAC $v20_mac);
rp_2@dest3::RouterPort(DEV veth20, IN_MAC $v20_mac, OUT_MAC $v19_mac);

rp_2@6->[2]pcg@6;
        pcg@6[0]->[0]sw@6;
        pcg@6[1]->[1]sw@6;
        pcg@6[2]->fw@6->[2]sw@6;
        pcg@6[3]->hproc@6;
        pcg@6[4]->uproc@6;
sw@6[2]->rp_2@6;

// use port zero at input of pcg and output of sw
rp_2@dest3->[0]pcg@dest3;
        pcg@dest3[0]->[0]sw@dest3;
        pcg@dest3[1]->[1]sw@dest3;
        pcg@dest3[2]->fw@dest3->[2]sw@dest3;
        pcg@dest3[3]->hproc@dest3;
        pcg@dest3[4]->uproc@dest3;
sw@dest3[0]->rp_2@dest3;
