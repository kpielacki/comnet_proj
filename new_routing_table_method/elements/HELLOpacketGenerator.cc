#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "HELLOpacketGenerator.hh"
#include "proj_packets.hh"

CLICK_DECLS

HELLOpacketGenerator::HELLOpacketGenerator() : _timer(this) {
	seq = 0;
}

HELLOpacketGenerator::~HELLOpacketGenerator(){

}

int HELLOpacketGenerator::initialize(ErrorHandler *errh){
    _timer.initialize(this);
    _timer.schedule_now();
    return 0;
}

void HELLOpacketGenerator::run_timer(Timer *timer) {
	seq++;
    assert(timer == &_timer);

    // make click packet with size of hello packet format
    WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);

    // set data to 0?
    memset(packet->data(),0,packet->length());

    // make format point to data part of click packet where HELLO packet is stored
    struct PacketHELLO *format = (struct PacketHELLO*) packet->data();

    // set type
    format->type = 4;
    // set source 0 temp
    format->source = seq%667;
    // set sequence
    format->sequence = seq;

    output(0).push(packet);
    _timer.reschedule_after_sec(5);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(HELLOpacketGenerator)
