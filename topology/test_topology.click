require(library /home/comnetsii/elements/routerport.click);

// rp :: RouterPort(DEV $dev, IN_MAC $in_mac , OUT_MAC $out_mac );
// rp :: RouterPort();

HELLOpacketGenerator()->
    Topology()->
    Print("\nPeriodic HELLO Pacaket")->
    Discard();
