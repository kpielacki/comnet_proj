#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "HELLOpacketGenerator.hh"
// #include "proj_packets.hh"
#include "table_kn.hh"

CLICK_DECLS

HELLOpacketGenerator::HELLOpacketGenerator() : _timer(this) {
	seq = 0;
    _my_host = 0;
}

HELLOpacketGenerator::~HELLOpacketGenerator(){

}

int HELLOpacketGenerator::initialize(ErrorHandler *errh){
    _timer.initialize(this);
    _timer.schedule_now();
    return 0;
}

int HELLOpacketGenerator::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
                  "MY_HOST", cpkP+cpkM, cpUnsigned, &_my_host,
                  cpEnd) < 0) {
    return -1;
  }
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
    format->type = 1;
    // set source 0 temp
    format->source = _my_host+seq;
    // set sequence
    format->sequence = seq;

    output(0).push(packet);
    _timer.reschedule_after_sec(5);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(HELLOpacketGenerator)
