#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "kn_router.hh"
#include "proj_packets.hh"



//--------------ROUTING TABLE CLASS--------------
TableKN::TableKN(){
    first_entry = true;
    routing_entry *routing_table = new routing_entry[0];
}

TableKN::~TableKN(){

}

void TableKN::adjust_size(int new_size){
    routing_entry* newArr = new routing_entry[new_size];

    click_chatter("afdsdf");
    click_chatter("%u", new_size);

    memcpy( newArr, routing_table, new_size * sizeof(routing_entry) );
    click_chatter("afdsdf2");


    delete [] routing_table;
    routing_table = newArr;
    entry_num = new_size;
}

int TableKN::entry_num = 0;

int TableKN::get_entry_num(){
    return entry_num;
}

void TableKN::print_table(){
    int entry;

    click_chatter("\n-----Routing Table-----");
    for( entry=0; entry <= entry_num; entry++ ){
        click_chatter("Entry: %u, Source: %u, Cost: %u", entry, routing_table[entry].destination, routing_table[entry].cost);
    }
    click_chatter("\n");
}

void TableKN::hello_update(uint16_t h_source){
    if ( first_entry ){
        adjust_size(entry_num);
        routing_table[0].destination = h_source;
        routing_table[0].cost = 1;

        first_entry = false;
    }
    else {
        bool updated_entry = false;

        // Try to find entries for HELLO source and change cost to 1
        int entry;
        for( entry=0; entry <= entry_num; entry++ ){

            // reassign hop count if entry exists
            if ( routing_table[entry].destination == h_source ){
                routing_table[entry].cost = 1;
                updated_entry = true;
            }
        }

        if ( !updated_entry ){

            click_chatter("up, entry_num = %u", entry_num);
            adjust_size(entry_num+1);
            click_chatter("up after, entry_num = %u", entry_num);
            routing_table[entry_num].destination = h_source;
            routing_table[entry_num].cost = 1;
        }
    }
}

TableKN r_table;

//--------------TOPOLOGY--------------
CLICK_DECLS

Topology::Topology() : _timer(this) {

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

    if ( hello_packet->type != 1 ){
        click_chatter("Invalid packet received at topology stage, killing packet");
        packet->kill();
    }
    else {
        r_table.hello_update(hello_packet->source);
    }

    r_table.print_table();
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
    format->type = 1;
    // set source 0 temp
    format->source = seq%6;
    // set sequence
    format->sequence = seq;

    output(0).push(packet);
    _timer.reschedule_after_sec(5);
}

//--------------ROUTING--------------

CLICK_ENDDECLS
EXPORT_ELEMENT(Topology)

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

    if ( update_packet->type != 2 ){
        click_chatter("Invalid packet received at routing stage, killing packet");
        packet->kill();
    }
    else {
        click_chatter("Received packet Type: %u, Source: %u, Sequence: %u, Length: %u, Payload: %u", update_packet->type, update_packet->source, update_packet->sequence, update_packet->length, update_packet->payload);
        //click_chatter("Routing Table: %u", Topology().get_entry_num());

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
    format->type = 2;
    // set source 0 temp
    format->source = 666;
    // set sequence
    format->sequence = seq;

    output(0).push(packet);
    _timer.reschedule_after_sec(5);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RoutingKN)
