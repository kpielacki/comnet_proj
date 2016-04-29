#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "kn_router.hh"
//#include "proj_packets.hh"



//--------------ROUTING TABLE CLASS--------------
TableKN::TableKN(){
    first_entry = true;
    //static routing_entry *routing_table = new routing_entry[0];
}

TableKN::~TableKN(){

}

void TableKN::adjust_size(int new_size){

    routing_entry* newArr = new routing_entry[1+new_size];

    memcpy( newArr, routing_table, new_size * sizeof(routing_entry) );

    if ( routing_table != NULL ) {
        delete [] routing_table;
    }

    routing_table = newArr;
    entry_num = new_size;
    
    // if ( newArr != NULL ) {
    //     delete [] newArr;
    // }

}

int TableKN::entry_num = 0;

routing_entry * TableKN::routing_table = new routing_entry[0];

int TableKN::get_entry_num(){
    return entry_num;
}

routing_entry *TableKN::get_all_entries(){
    return routing_table;
}

void TableKN::update_entry(int entry, uint16_t dest, uint8_t cost, uint16_t next_hop){
    routing_table[entry].destination = dest;
    routing_table[entry].cost = cost;
    routing_table[entry].next_hop = next_hop;
}

void TableKN::add_new_entry(uint16_t dest, uint8_t cost, uint16_t next_hop){
    if ( first_entry ){
        adjust_size(entry_num);
        routing_table[0].destination = dest;
        routing_table[0].cost = cost;
        routing_table[0].next_hop = next_hop;

        first_entry = false;
    }
    else{
        adjust_size(entry_num+1);
        routing_table[entry_num].destination = dest;
        routing_table[entry_num].cost = cost;
        routing_table[entry_num].next_hop = next_hop;
    }
}

void TableKN::print_table(){
    int entry;

    click_chatter("\n-----Routing Table-----");
    for( entry=0; entry <= entry_num; entry++ ){
        click_chatter("Entry: %u, Destination: %u, Cost: %u, Next Hop: %u", entry, routing_table[entry].destination, routing_table[entry].cost, routing_table[entry].next_hop);
    }
    click_chatter("\n");
}

void TableKN::hello_update(uint16_t h_source){
    if ( first_entry ){
        adjust_size(entry_num);
        routing_table[0].destination = h_source;
        routing_table[0].cost = 1;
        routing_table[0].next_hop = h_source;

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
            adjust_size(entry_num+1);
            routing_table[entry_num].destination = h_source;
            routing_table[entry_num].cost = 1;
            routing_table[entry].next_hop = h_source;
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
    // seq++;
    // assert(timer == &_timer);

    // // make click packet with size of hello packet format
    // WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketHELLO), 0);

    // // set data to 0?
    // memset(packet->data(),0,packet->length());

    // // make format point to data part of click packet where HELLO packet is stored
    // struct PacketHELLO *format = (struct PacketHELLO*) packet->data();

    // // set type
    // format->type = 1;
    // // set source 0 temp
    // format->source = seq%6;
    // // set sequence
    // format->sequence = seq;

    // output(0).push(packet);
    // _timer.reschedule_after_sec(5);
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

        //routing_entry* pay_read = new routing_entry[update_packet->length];
        //memcpy( pay_read, &update_packet->payload, update_packet->length * sizeof(routing_entry) );

        int r_entry;
        // interate through all entries in received routing table
        for ( r_entry = 0; r_entry < update_packet->length; r_entry++ ) {
            click_chatter("Checking Received Entry: %u, Source: %u, Dest: %u, Cost: %u", r_entry, update_packet->source, update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost);            
            //click_chatter("\n-----UPDATE ROUTING TABLE-----");
            
            // discard incoming entries if already in table
            bool same_entry;
            same_entry = false;

            // array to hold entry number of matching destinations in routing table
            int matching_dest_entries[2];  // there should never be more than 3 of the same entry in a table            
            int equal_hop_count;
            int better_hop_count;
            equal_hop_count = 0;
            better_hop_count = 0;

            // interate through all entries in own routing table
            int entry;
            bool found_entry = false;
            // retrieve routing table through each iteration since it will be updated each iteration
            routing_entry* retrieved_table = r_table.get_all_entries();
            for( entry=0; entry <= r_table.get_entry_num(); entry++ ){
                click_chatter("\tChecking Own Entry: %u, Destiation: %u, Cost: %u, Next Hop: %u", entry, retrieved_table[entry].destination, retrieved_table[entry].cost, retrieved_table[entry].next_hop);

                // array to hold entry number of matching destinations in routing table
                // int matching_dest_entries[2] = new int[2];  // there should never be more than 3 of the same entry in a table            
                equal_hop_count = 0;
                better_hop_count = 0;

                if ( update_packet->payload[r_entry].destination == retrieved_table[entry].destination ) {
                    // interate through all current entries
                    if ( update_packet->payload[r_entry].cost + 1 < retrieved_table[entry].cost ) {
                        click_chatter("\t Replacing with Better Entry");
                        matching_dest_entries[better_hop_count] = entry;
                        better_hop_count++;
                        found_entry = true;
                    }
                    else if ( update_packet->payload[r_entry].cost + 1 == retrieved_table[entry].cost ) {
                        matching_dest_entries[equal_hop_count] = entry;
                        equal_hop_count++;
                        if ( update_packet->source == retrieved_table[entry].next_hop ) {
                            click_chatter("\t\tFound Same Entry: %u", equal_hop_count);
                            same_entry = true;
                            found_entry = true;
                        }
                    }
                }
            }

            // add equal entry if room available
            if ( !same_entry and equal_hop_count > 0 and equal_hop_count < 3 ) {
                click_chatter("\t\tAdding New Route Entry");
                r_table.add_new_entry(update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);
            }
            else if ( better_hop_count > 0 ) {
                r_table.update_entry(matching_dest_entries[0], update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);
                
                // remove alternative routes if exists
                int i;
                for ( i = 1; i <= better_hop_count; i++ ) {
                    click_chatter("\t\tRemoving Old Worse Entry");
                }
            }
            // add entry to routing table if entry was not found
            click_chatter("\t\tpls");
            if ( !found_entry ){
                click_chatter("\t\tAdding Brand New Entry");
                r_table.add_new_entry(update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);
                found_entry = false;
            }
        }

    r_table.print_table();
    packet->kill();
    }
}

void RoutingKN::run_timer(Timer *timer) {
    // seq++;
    // assert(timer == &_timer);

    // WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketUPDATE), 0);

    // memset(packet->data(),0,packet->length());

    // struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();

    // // set type
    // format->type = 2;
    // // set source 0 temp
    // format->source = 666;
    // // set sequence
    // format->sequence = seq;

    // output(0).push(packet);
    _timer.reschedule_after_sec(5);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(RoutingKN)


CLICK_DECLS

TESTpacketGen::TESTpacketGen() : _timer(this) {
    seq = 0;
}

TESTpacketGen::~TESTpacketGen(){

}

int TESTpacketGen::initialize(ErrorHandler *errh){
    _timer.initialize(this);
    _timer.schedule_now();
    return 0;
}

void TESTpacketGen::run_timer(Timer *timer) {
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
    format->source = seq%2;
    // set sequence
    format->sequence = seq;
    // set sequence
    format->length = 3;
    // payload

    export_table = new routing_entry[2];
    export_table[0].destination = 12;
    export_table[0].cost = 10;
    export_table[1].destination = 69;
    export_table[1].cost =  10;
    export_table[2].destination = 5;
    export_table[2].cost = 10;

    //routing_entry ex = new routing_entry[100];
    //memcpy(ex, &export_table, sizeof(routing_entry[2]));
    //format->payload = *export_table;
    //format->payload = *export_table;
    
    // copy (largest index + 1) * size
    memcpy(format->payload, export_table, format->length*sizeof(routing_entry));

    output(0).push(packet);

    delete [] export_table;
    _timer.reschedule_after_sec(1);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(TESTpacketGen)
