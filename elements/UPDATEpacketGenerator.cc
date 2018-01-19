#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "UPDATEpacketGenerator.hh"
// #include "proj_packets.hh"
#include "table_kn.hh"

CLICK_DECLS

UpdatepacketGen::UpdatepacketGen() : _timer(this) {
    seq = 0;
}

UpdatepacketGen::~UpdatepacketGen(){

}

int UpdatepacketGen::initialize(ErrorHandler *errh){
    _timer.initialize(this);
    _timer.schedule_now();
    return 0;
}

void UpdatepacketGen::run_timer(Timer *timer) {
    seq++;
    assert(timer == &_timer);

    // make click packet with size of UPDATE packet format
    WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketUPDATE), 0);

    // set data to 0?
    memset(packet->data(),0,packet->length());

    // make format point to data part of click packet where UPDATE packet is stored
    struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();

    // set type
    format->type = 2;
    // set source 0 temp
    format->source = seq%2;
    // set sequence
    format->sequence = seq;
    // set sequence
    format->length = 3;
    // payload

    routing_entry *export_table;
    if ( seq%20 < 5 ){
        export_table = new routing_entry[2];
        export_table[0].destination = 12;
        export_table[0].cost = 10;
        export_table[1].destination = 69;
        export_table[1].cost =  10;
        export_table[2].destination = 5;
        export_table[2].cost = 10;
    }
    else if (seq%20 < 10 ) {
        export_table = new routing_entry[2];
        export_table[0].destination = 12;
        export_table[0].cost = 5;
        export_table[1].destination = 69;
        export_table[1].cost =  10;
        export_table[2].destination = 5;
        export_table[2].cost = 10;
    }
    else if (seq%20 < 15 ) {
        export_table = new routing_entry[2];
        export_table[0].destination = 12;
        export_table[0].cost = 5;
        export_table[1].destination = 69;
        export_table[1].cost =  2;
        export_table[2].destination = 5;
        export_table[2].cost = 3;
    }
    else {
        export_table = new routing_entry[2];
        export_table[0].destination = 12;
        export_table[0].cost = 5;
        export_table[1].destination = 39;
        export_table[1].cost =  2;
        export_table[2].destination = 55;
        export_table[2].cost = 3;
    }

    //routing_entry ex = new routing_entry[100];
    //memcpy(ex, &export_table, sizeof(routing_entry[2]));
    //format->payload = *export_table;
    //format->payload = *export_table;

    // copy (largest index + 1) * size
    memcpy(format->payload, export_table, format->length*sizeof(routing_entry));

    output(0).push(packet);

    delete [] export_table;
    _timer.reschedule_after_sec(3);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(UpdatepacketGen)
