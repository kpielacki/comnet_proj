#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "hello_processing.hh"

//--------------HelloProcessing--------------
CLICK_DECLS

HelloProcessing::HelloProcessing() : r_table(0) {

}

HelloProcessing::~HelloProcessing(){

}

int HelloProcessing::initialize(ErrorHandler *errh){
    return 0;
}

int HelloProcessing::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh, "ROUTINGTABLE", cpkN, cpElement, &r_table, cpEnd) < 0) return -1;
    return 0;
}

void HelloProcessing::push(int port, Packet *packet) {
    assert(packet);

    // look at hello packet
    struct PacketHELLO *hello_packet = (struct PacketHELLO *)packet->data();

    if ( hello_packet->type != 1 ){
        packet->kill();
    }
    else {
        r_table->hello_update(hello_packet->source);
    }

    // r_table->print_table();
    packet->kill();
}


CLICK_ENDDECLS
EXPORT_ELEMENT(HelloProcessing)
