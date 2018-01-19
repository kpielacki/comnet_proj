require(library /home/comnetsii/elements/routerport.click);

// Element Configuration
// 		Naming Convention:
//			element@Device[Name]Number		No Name for Routers

// ---Router 1---
rt@1:: TableKN(MY_HOST 1001);

pcg@1::ClassifierAndGenerator(ROUTINGTABLE rt@1);
fw@1::Forwarding(ROUTINGTABLE rt@1);
hproc@1::HelloProcessing(ROUTINGTABLE rt@1);
uproc@1::UpdateProcessing(ROUTINGTABLE rt@1);
sw@1::McSwitch(ROUTINGTABLE rt@1);

// ---Router 2---
rt@2:: TableKN(MY_HOST 1002);

pcg@2::ClassifierAndGenerator(ROUTINGTABLE rt@2);
fw@2::Forwarding(ROUTINGTABLE rt@2);
hproc@2::HelloProcessing(ROUTINGTABLE rt@2);
uproc@2::UpdateProcessing(ROUTINGTABLE rt@2);
sw@2::McSwitch(ROUTINGTABLE rt@2);

// // ---Router 3---
// rt@3:: TableKN(MY_HOST 1003);

// pcg@3::ClassifierAndGenerator(ROUTINGTABLE rt@3);
// fw@3::Forwarding(ROUTINGTABLE rt@3);
// hproc@3::HelloProcessing(ROUTINGTABLE rt@3);
// uproc@3::UpdateProcessing(ROUTINGTABLE rt@3);
// sw@3::McSwitch(ROUTINGTABLE rt@3);

// // ---Router 4---
// rt@4:: TableKN(MY_HOST 1004);

// pcg@4::ClassifierAndGenerator(ROUTINGTABLE rt@4);
// fw@4::Forwarding(ROUTINGTABLE rt@4);
// hproc@4::HelloProcessing(ROUTINGTABLE rt@4);
// uproc@4::UpdateProcessing(ROUTINGTABLE rt@4);
// sw@4::McSwitch(ROUTINGTABLE rt@4);

// // ---Router 5---
// rt@5:: TableKN(MY_HOST 1005);

// pcg@5::ClassifierAndGenerator(ROUTINGTABLE rt@5);
// fw@5::Forwarding(ROUTINGTABLE rt@5);
// hproc@5::HelloProcessing(ROUTINGTABLE rt@5);
// uproc@5::UpdateProcessing(ROUTINGTABLE rt@5);
// sw@5::McSwitch(ROUTINGTABLE rt@5);

// // ---Router 6---
// rt@6:: TableKN(MY_HOST 1006);

// pcg@6::ClassifierAndGenerator(ROUTINGTABLE rt@6);
// fw@6::Forwarding(ROUTINGTABLE rt@6);
// hproc@6::HelloProcessing(ROUTINGTABLE rt@6);
// uproc@6::UpdateProcessing(ROUTINGTABLE rt@6);
// sw@6::McSwitch(ROUTINGTABLE rt@6);

// // ---Source ---
// rt@source:: TableKN(MY_HOST 1007);

// pcg@source::ClassifierAndGenerator(ROUTINGTABLE rt@source);
// fw@source::Forwarding(ROUTINGTABLE rt@source);
// hproc@source::HelloProcessing(ROUTINGTABLE rt@source);
// uproc@source::UpdateProcessing(ROUTINGTABLE rt@source);
// sw@source::McSwitch(ROUTINGTABLE rt@source);

// // ---Destination 1---
// rt@dest1:: TableKN(MY_HOST 1008);

// pcg@dest1::ClassifierAndGenerator(ROUTINGTABLE rt@dest1);
// fw@dest1::Forwarding(ROUTINGTABLE rt@dest1);
// hproc@dest1::HelloProcessing(ROUTINGTABLE rt@dest1);
// uproc@dest1::UpdateProcessing(ROUTINGTABLE rt@dest1);
// sw@dest1::McSwitch(ROUTINGTABLE rt@dest1);

// // ---Destination 2---
// rt@dest2:: TableKN(MY_HOST 1009);

// pcg@dest2::ClassifierAndGenerator(ROUTINGTABLE rt@dest2);
// fw@dest2::Forwarding(ROUTINGTABLE rt@dest2);
// hproc@dest2::HelloProcessing(ROUTINGTABLE rt@dest2);
// uproc@dest2::UpdateProcessing(ROUTINGTABLE rt@dest2);
// sw@dest2::McSwitch(ROUTINGTABLE rt@dest2);

// // ---Destination 3---
// rt@dest3:: TableKN(MY_HOST 1010);

// pcg@dest3::ClassifierAndGenerator(ROUTINGTABLE rt@dest3);
// fw@dest3::Forwarding(ROUTINGTABLE rt@dest3);
// hproc@dest3::HelloProcessing(ROUTINGTABLE rt@dest3);
// uproc@dest3::UpdateProcessing(ROUTINGTABLE rt@dest3);
// sw@dest3::McSwitch(ROUTINGTABLE rt@dest3);

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

rp_0@1->pcg@1;
        pcg@1[0]->[0]sw@1;
        pcg@1[1]->[1]sw@1;
        pcg@1[2]->fw@1->[2]sw@1;
        pcg@1[3]->hproc@1;
        pcg@1[4]->uproc@1;
sw@1[0]->rp_0@1;

rp_0@2->pcg@2;
        pcg@2[0]->[0]sw@2;
        pcg@2[1]->[1]sw@2;
        pcg@2[2]->fw@2->[2]sw@2;
        pcg@2[3]->hproc@2;
        pcg@2[4]->uproc@2;
sw@2[0]->rp_0@2;
