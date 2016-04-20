require(library /home/comnetsii/elements/routerport.click);

// rp :: RouterPort(DEV $dev, IN_MAC $in_mac , OUT_MAC $out_mac );
// rp :: RouterPort();

UPDATEpacketGenerator()->
    RoutingKN()->
    Print("\nPeriodic UPDATE Packet")->
    Discard();
