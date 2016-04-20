#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "Topology.hh"
#include "proj_packets.hh"

CLICK_DECLS

routing_entry * adjust_size(routing_entry *array, int old_size, int new_size){
    routing_entry* newArr = new routing_entry[new_size];
    memcpy( newArr, array, old_size * sizeof(routing_entry) );

    delete [] array;
    array = newArr;
    return array;
}

Topology::Topology() : _timer(this) {
    first_entry = true;
    updated_entry = false;

    entry_num = 0;
    routing_entry *routing_table = new routing_entry[0];

    seq = 0;
}

Topology::~Topology(){

}

int Topology::initialize(ErrorHandler *errh){
    _timer.initialize(this);
    _timer.schedule_now();
    return 0;
}

int Topology::configure(Vector<String> &conf, ErrorHandler *errh) {
  return 0;
}

void Topology::push(int port, Packet *packet) {
    assert(packet);

    // look at hello packet
    struct PacketHELLO *hello_packet = (struct PacketHELLO *)packet->data();

    click_chatter("Received packet %u. Type: %u, Source: %u", hello_packet->sequence, hello_packet->type, hello_packet->source);
    int entry;

    if ( hello_packet->type != 0 ){
        click_chatter("Invalid packet received at topology stage, killing packet");
        packet->kill();
    }
    else {
        updated_entry = false;


        if ( first_entry ){
            click_chatter("Entering first entry");
            routing_table = adjust_size(routing_table, entry_num, entry_num);
            routing_table[0].destination = hello_packet->source;
            routing_table[0].cost = 1;

            first_entry = false;
        }
        else {
            click_chatter("entry num: %u", entry_num);

            for( entry=0; entry <= entry_num; entry++ ){
                click_chatter("Searching Entry: %u, Source: %u, Cost: %u", entry, routing_table[entry].destination, routing_table[entry].cost);
                // reassign hop count if entry exists
                if ( routing_table[entry].destination == hello_packet->source ){
                    click_chatter("Updating entry: %u", entry);
                    routing_table[entry].cost = 1;

                    updated_entry = true;

                }
            }

            if ( !updated_entry ){
                routing_table = adjust_size(routing_table, entry_num+1, entry_num+2);

                entry_num++;
                routing_table[entry_num].destination = hello_packet->source;
                routing_table[entry_num].cost = 1;

                click_chatter("Made new entry: %u", entry_num);
            }
        }
    }

    click_chatter("---Routing Table---");
    for( entry=0; entry <= entry_num; entry++ ){
        click_chatter("Entry: %u, Source: %u, Cost: %u", entry, routing_table[entry].destination, routing_table[entry].cost);
    }
    click_chatter("\n");



    /*
    WritablePacket *ack = Packet::make(0,0,sizeof(struct PacketHeader), 0);
    memset(ack->data(),0,ack->length());
    struct PacketHeader *format = (struct PacketHeader*) ack->data();
    format->type = ACK;
    format->sequence = header->sequence;
    format->source = _my_address;
    format->destination = header->source;
    format->size = sizeof(struct PacketHeader);
    output(0).push(ack);
    struct PacketHeader *header = (struct PacketHeader *)packet->data();
    click_chatter("Received ack %u from %u", header->sequence, header->source);
    */
    packet->kill();
}

void Topology::run_timer(Timer *timer) {
    seq++;
    assert(timer == &_timer);

    // make click packet with size of hello packet format
    WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHELLO), 0);

    // set data to 0?
    memset(packet->data(),0,packet->length());

    // make format point to data part of click packet where HELLO packet is stored
    struct PacketHELLO *format = (struct PacketHELLO*) packet->data();

    // set type
    format->type = 0;
    // set source 0 temp
    format->source = seq%6;
    // set sequence
    format->sequence = seq;

    output(0).push(packet);
    _timer.reschedule_after_sec(5);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Topology)
