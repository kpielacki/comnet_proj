#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "RoutingKN.hh"
#include "proj_packets.hh"

CLICK_DECLS

RoutingKN::RoutingKN() : _timer(this) {
    seq = 0;
}

RoutingKN::~RoutingKN(){

}

int RoutingKN::initialize(ErrorHandler *errh){
    _timer.initialize(this);
    _timer.schedule_now();
    return 0;
}

int RoutingKN::configure(Vector<String> &conf, ErrorHandler *errh) {
  return 0;
}

void RoutingKN::push(int port, Packet *packet) {
    assert(packet);
    
    // look at update packet
    struct PacketUPDATE *update_packet = (struct PacketUPDATE *)packet->data();

    if ( update_packet->type != 1 ){
        click_chatter("Invalid packet received at routing stage, killing packet");
        packet->kill();
    }
    else {
        click_chatter("Received packet Type: %u, Source: %u, Sequence: %u, Length: %u, Payload: %u", update_packet->type, update_packet->source, update_packet->sequence, update_packet->length, update_packet->payload);

    packet->kill();
    }
}

void RoutingKN::run_timer(Timer *timer) {
    seq++;
    assert(timer == &_timer);

    WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketUPDATE), 0);

    memset(packet->data(),0,packet->length());

    struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();

    // set type
    format->type = 1;
    // set source 0 temp
    format->source = 666;
    // set sequence
    format->sequence = seq;

    output(0).push(packet);
    _timer.reschedule_after_sec(5);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RoutingKN)
