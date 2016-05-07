require(library /home/comnetsii/elements/routerport.click);

// rp :: RouterPort(DEV $dev, IN_MAC $in_mac , OUT_MAC $out_mac );
// rp :: RouterPort();

// cb::ClassifierBlock();
// tp::Topology();
// rs::RoutingKN();

rt1:: TableKN();
rt2:: TableKN();

hgen1::HELLOpacketGenerator(MY_HOST 666);
hgen2::HELLOpacketGenerator(MY_HOST 123);

hproc1::HelloProcessing(ROUTINGTABLE rt1);
hproc2::HelloProcessing(ROUTINGTABLE rt2);

hgen1->hproc1// ->Discard();
hgen2->hproc2// ->Discard();
// TESTpacketGen()->cb
