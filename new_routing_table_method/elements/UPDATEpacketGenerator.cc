#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "UPDATEpacketGenerator.hh"
#include "proj_packets.hh"

CLICK_DECLS

UPDATEpacketGenerator::UPDATEpacketGenerator() : _timer(this) {
	seq = 0;
}

UPDATEpacketGenerator::~UPDATEpacketGenerator(){

}

int UPDATEpacketGenerator::initialize(ErrorHandler *errh){
    _timer.initialize(this);
    _timer.schedule_now();
    return 0;
}

void UPDATEpacketGenerator::run_timer(Timer *timer) {
	seq++;
    assert(timer == &_timer);

    // make click packet with size of UPDATE packet format
    WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketUPDATE), 0);

    // set data to 0?
    memset(packet->data(),0,packet->length());

    // make format point to data part of click packet where UPDATE packet is stored
    struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();

    // set type
    format->type = 2;
    // set source 0 temp
    format->source = seq%4;
    // set sequence
    format->sequence = seq;
    // set sequence
    format->length = 1;
    // payload
    format->payload = 123;


    output(0).push(packet);
    _timer.reschedule_after_sec(1);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(UPDATEpacketGenerator)
