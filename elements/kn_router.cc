#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "kn_router.hh"
#include <click/packet_anno.hh> //Add this one
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

// pass ordered list of routing entries to remove
void TableKN::remove_table_entries(int remove_entries[], int remove_cnt){
    int remove_iter;
    remove_iter = 0;

    // temp
    routing_entry* temp = new routing_entry[1+entry_num - remove_cnt];

    int i;
    for ( i = 0; i <= entry_num; i++ ){
        if ( i==remove_entries[remove_iter] and remove_iter<remove_cnt ) {
            remove_iter++;
        }
        else{
            temp[i - remove_iter] = routing_table[i];
        }
    }

    //memcpy( newArr, routing_table, new_size * sizeof(routing_entry) );

    if ( routing_table != NULL ) {
        delete [] routing_table;
    }

    routing_table = temp;
    entry_num = entry_num - remove_cnt;
}

void TableKN::print_table(){
    int entry;

    click_chatter("-----Routing Table-----");
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
        int remove_entries[2];
        int del_count;
        del_count = 0;

        for( entry=0; entry <= entry_num; entry++ ){

            // reassign hop count if entry exists
            if ( routing_table[entry].destination == h_source and routing_table[entry].next_hop == h_source){
                routing_table[entry].cost = 1;
                updated_entry = true;
            }
            else if ( routing_table[entry].destination == h_source and routing_table[entry].next_hop != h_source and routing_table[entry].cost > 1){
                remove_entries[del_count] = entry;
                del_count++;
            }
        }
        // remove alternative routes of higher cost
        if ( del_count > 0 ) {
            remove_table_entries(remove_entries, del_count);
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

    int entry;

    if ( hello_packet->type != 1 ){
        //packet->kill();
    }
    else {
        r_table.hello_update(hello_packet->source);
    }

    //r_table.print_table();
    //packet->kill();
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
        //packet->kill();
    }
    else {
        //routing_entry* pay_read = new routing_entry[update_packet->length];
        //memcpy( pay_read, &update_packet->payload, update_packet->length * sizeof(routing_entry) );

        int r_entry;
        // interate through all entries in received routing table
        for ( r_entry = 0; r_entry < update_packet->length; r_entry++ ) {

            // discard incoming entries if already in table
            bool same_entry;
            same_entry = false;

            // routing table index of first encountered better entry replacement
            int overwrite_entry;
            // array to hold entires to remove from routing table that were alterative routes but the update packet contained a better route
            int del_entries[1];  // there should never be more than 3 of the same entry in a table
            int equal_hop_count;
            int better_hop_count;
            equal_hop_count = 0;
            better_hop_count = 0;

            // interate through all entries in own routing table
            int entry;
            bool found_entry = false;
            bool discard_entry = false;

            // retrieve routing table through each iteration since it will be updated each iteration
            routing_entry* retrieved_table = r_table.get_all_entries();
            for( entry=0; entry <= r_table.get_entry_num(); entry++ ){
                found_entry = false;
                // array to hold entry number of matching destinations in routing table
                // int matching_dest_entries[2] = new int[2];  // there should never be more than 3 of the same entry in a table
                equal_hop_count = 0;
                // better_hop_count = 0;

                if ( update_packet->payload[r_entry].destination == retrieved_table[entry].destination ) {
                    // interate through all current entries
                    found_entry = true;
                    if ( update_packet->payload[r_entry].cost + 1 < retrieved_table[entry].cost ) {
                        // only overwrite better entry once, all others remove
                        if ( better_hop_count < 1 ) {
                            overwrite_entry = entry;
                            better_hop_count++;
                            found_entry = true;
                        }
                        else {
                            del_entries[better_hop_count - 1] = entry;
                            better_hop_count++;
                        }
                    }
                    else if ( update_packet->payload[r_entry].cost + 1 == retrieved_table[entry].cost ) {
                        // matching_dest_entries[equal_hop_count] = entry;
                        equal_hop_count++;
                        if ( update_packet->source == retrieved_table[entry].next_hop ) {
                            same_entry = true;
                        }
                    }
                    else {
                        discard_entry = true;
                    }
                }
            }

            // add equal entry if room available
            if ( !same_entry and equal_hop_count > 0 and equal_hop_count < 3 ) {
                r_table.add_new_entry(update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);
            }

            if ( better_hop_count > 0 ) {
                r_table.update_entry(overwrite_entry, update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);

                // remove alternative routes if exists
                if ( better_hop_count > 1) {
                    r_table.remove_table_entries(del_entries, better_hop_count - 1);
                }
            }
            // add entry to routing table if entry was not found
            if ( !found_entry and !same_entry and better_hop_count <= 0 and !discard_entry ){
                r_table.add_new_entry(update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);
                found_entry = false;
            }
        }

    //r_table.print_table();
    //packet->kill();
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
    _timer.reschedule_after_sec(7);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(TESTpacketGen)



CLICK_DECLS

packetGen::packetGen() : _timerHELLO(this), _timerHELLO_TO(this), _timerUPDATE(this), _timerUPDATE_TO(this), _timerPrintTable(this), _timerDATA(this) {
    last_tran = 0;
    seq = 0;
    _my_host = 0;
}

packetGen::~packetGen(){

}

int packetGen::initialize(ErrorHandler *errh){
    _timerHELLO_TO.initialize(this);
    _timerHELLO_TO.schedule_after_sec(1);
    _timerHELLO.initialize(this);
    _timerHELLO.schedule_after_sec(5);

    _timerUPDATE_TO.initialize(this);
    _timerUPDATE_TO.schedule_after_sec(1);
    _timerUPDATE.initialize(this);
    _timerUPDATE.schedule_after_sec(5);

    _timerPrintTable.initialize(this);
    _timerPrintTable.schedule_after_sec(10);

    _timerDATA.initialize(this);
    _timerDATA.schedule_now();

    return 0;
}

int packetGen::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
                  "MY_ADDRESS", cpkP+cpkM, cpUnsigned, &_my_host,
                  "CLIENT", cpkP+cpkM, cpUnsigned, &_client,
                  cpEnd) < 0) {
    return -1;
  }
  return 0;
}

void packetGen::run_timer(Timer *timer) {
    // Periodically transmit or retransmit HELLO and UPDATE packet
    if ( _client == 1 ){
        if( timer == &_timerDATA ){

            assert(timer == &_timerDATA);

            // make click packet with size of DATA packet format
            WritablePacket *packet = Packet::make(0,0,sizeof(struct PacketDATA) + 5, 0);

            // set data to 0?
            memset(packet->data(),0,packet->length());

            // make format point to data part of click packet where DATA packet is stored
            struct PacketDATA *format = (struct PacketDATA*) packet->data();

            format->type = 4;
            format->source = _my_host;
            format->sequence = seq;
            format->k = 3;
            format->destination1 = 24;
            format->destination2 = 25;
            format->destination3 = 26;
            format->length = 5;
            format->payload = 1099511623776;

            output(0).push(packet);
            _timerDATA.reschedule_after_sec(20);
        } else if( timer == &_timerHELLO_TO ){
            seq++;
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
            memset(packet->data(),0,packet->length());

            struct PacketHELLO *format = (struct PacketHELLO*) packet->data();

            format->type = 1;
            format->source = _my_host;
            format->sequence = seq;

            last_tran = 1;
            output(4).push(packet);
            _timerHELLO_TO.schedule_after_sec(1);
        } else if( timer == &_timerHELLO ){
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
            memset(packet->data(),0,packet->length());
            struct PacketHELLO *format = (struct PacketHELLO*) packet->data();
            format->type = 1;
            format->source = _my_host;
            format->sequence = seq;

            last_tran = 1;
            output(4).push(packet);
            _timerHELLO.schedule_after_sec(5);
        }
        else if ( timer == &_timerUPDATE_TO ) {
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketUPDATE), 0);

            memset(packet->data(),0,packet->length());

            // make format point to data part of click packet where UPDATE packet is stored
            struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();

            format->type = 2;
            format->source = _my_host;
            format->sequence = seq;
            format->length = r_table.get_entry_num();

            memcpy(format->payload, r_table.get_all_entries(), format->length*sizeof(routing_entry));

            last_tran = 2;
            output(5).push(packet);
            _timerUPDATE.schedule_after_sec(5);

        }
        else if( timer == &_timerUPDATE ) {
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
            memset(packet->data(),0,packet->length());
            struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();
            format->type = 2;
            format->source = _my_host;
            format->sequence = seq;
            format->length = r_table.get_entry_num();

            memcpy(format->payload, r_table.get_all_entries(), format->length*sizeof(routing_entry));

            last_tran = 2;
            output(5).push(packet);
            _timerUPDATE.schedule_after_sec(5);
        }
        else if( timer == &_timerPrintTable ) {
            click_chatter("\n-----Host %u-----", _my_host);
            r_table.print_table();
            _timerPrintTable.schedule_after_sec(10);
        }
        else {
            assert(false);
        }
    }
    else {
        if( timer == &_timerHELLO_TO ){
            seq++;
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
            memset(packet->data(),0,packet->length());

            struct PacketHELLO *format = (struct PacketHELLO*) packet->data();

            format->type = 1;
            format->source = _my_host;
            format->sequence = seq;

            last_tran = 1;
            output(4).push(packet);
            _timerHELLO_TO.schedule_after_sec(1);
        } else if( timer == &_timerHELLO ){
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
            memset(packet->data(),0,packet->length());
            struct PacketHELLO *format = (struct PacketHELLO*) packet->data();
            format->type = 1;
            format->source = _my_host;
            format->sequence = seq;

            last_tran = 1;
            output(4).push(packet);
            _timerHELLO.schedule_after_sec(5);
        }
        else if ( timer == &_timerUPDATE_TO ) {
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketUPDATE), 0);

            memset(packet->data(),0,packet->length());

            // make format point to data part of click packet where UPDATE packet is stored
            struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();

            format->type = 2;
            format->source = _my_host;
            format->sequence = seq;
            format->length = r_table.get_entry_num();

            memcpy(format->payload, r_table.get_all_entries(), format->length*sizeof(routing_entry));

            last_tran = 2;
            output(5).push(packet);
            _timerUPDATE.schedule_after_sec(5);

        }
        else if( timer == &_timerUPDATE ) {
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
            memset(packet->data(),0,packet->length());
            struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();
            format->type = 2;
            format->source = _my_host;
            format->sequence = seq;
            format->length = r_table.get_entry_num();

            memcpy(format->payload, r_table.get_all_entries(), format->length*sizeof(routing_entry));

            last_tran = 2;
            output(5).push(packet);
            _timerUPDATE.schedule_after_sec(5);
        }
        else if( timer == &_timerPrintTable ) {
            click_chatter("\n-----Host %u-----", _my_host);
            r_table.print_table();
            _timerPrintTable.schedule_after_sec(10);
        }
        else {
            assert(false);
        }
    }
}

void packetGen::push(int port, Packet *packet) {

    assert(packet);
    struct PacketType *header = (struct PacketType *)packet->data();

    // Read type, send ACK, and send to next stage
    if ( header->type == 4 ){
        // Handle Data
        struct PacketDATA *header4 = (struct PacketDATA *)packet->data();
        WritablePacket *ack = Packet::make(14,0,sizeof(struct PacketACK), 0);
        memset(ack->data(),0,ack->length());
        struct PacketACK *format = (struct PacketACK*) ack->data();
        format->type = 3;
        format->source = _my_host;
        format->sequence = header4->sequence;
        format->destination = header4->source;
        output(2).push(ack);
        output(3).push(packet);
        // packet->kill();
    }
    else if ( header->type == 1 ){
        // Handle HELLO
        struct PacketHELLO *header1 = (struct PacketHELLO *)packet->data();
        WritablePacket *ack = Packet::make(14,0,sizeof(struct PacketACK), 0);
        memset(ack->data(),0,ack->length());
        struct PacketACK *format = (struct PacketACK*) ack->data();
        format->type = 3;
        format->source = _my_host;
        format->sequence = header1->sequence;
        format->destination = header1->source;
        output(2).push(ack);
        output(0).push(packet);
        // packet->kill();
    }
    else if ( header->type == 2 ){
        // Handle UPDATE
        struct PacketUPDATE *header2 = (struct PacketUPDATE *)packet->data();
        WritablePacket *ack = Packet::make(14,0,sizeof(struct PacketACK), 0);
        memset(ack->data(),0,ack->length());
        struct PacketACK *format = (struct PacketACK*) ack->data();
        format->type = 3;
        format->source = _my_host;
        format->sequence = header2->sequence;
        format->destination = header2->source;
        output(2).push(ack);
        output(1).push(packet);
        // packet->kill();
    }
    else if ( header->type == 3 ) {
        // Handle ACK
        struct PacketACK *header3 = (struct PacketACK *)packet->data();
        if(header3->sequence == seq) {
            if ( last_tran == 1 ) {
                _timerHELLO_TO.unschedule();
                seq++;
                _timerHELLO_TO.schedule_after_sec(5);
            }
            if ( last_tran == 2 ) {
                _timerUPDATE_TO.unschedule();
                seq++;
                _timerUPDATE_TO.schedule_after_sec(5);
            }
            else {
                //packet->kill();
            }
        }
        else {
            //packet->kill();
        }
    }
    else {
        // Kill invalid packet
        //packet->kill();
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(packetGen)



//---------------Forwarding Element--------------//
CLICK_DECLS

ForwardKN::ForwardKN(){
    num_dest = 0;
}

ForwardKN::~ForwardKN(){

}

int ForwardKN::configure(Vector<String> &conf, ErrorHandler *errh){
    return 0;
}

void ForwardKN::push(Packet *packet){
    pckt = packet;
    uint8_t pType = readPacket();

    //Find routing table entries
    if (pType==4){
        //Make array for possibleHops, set number of routes for each destination to 0
        for (int i = 0; i<num_dest; i++){
            possibleHops[i][4] = 0;
        }

        routing_entry* forwarding_table = r_table.get_all_entries();
        for (int i = 0; i<num_dest; i++){
            for (int entry=0; entry <= r_table.get_entry_num(); entry++){
                if (forwarding_table[entry].destination == dest[i]){
                    costs[i] = forwarding_table[entry].cost;
                    possibleHops[i][possibleHops[i][4]] = forwarding_table[entry].next_hop;
                    possibleHops[i][4]++;
                }
            }

            //Discard packet if routing entry not found
            if (possibleHops[i][4]==0){
                //packet->kill();
                click_chatter("Forward Element - One or more destination not found in table. Unable to forward packet");
                goto quit;
            }
        }

        //Perform forwarding algorithm
        chooseKdest();
    }

    quit: ;
}

//returns packet Type, stores all other packet information in class variables
uint8_t ForwardKN::readPacket(){

    assert(pckt);
    struct PacketDATA *header = (struct PacketDATA *)pckt->data();
    uint8_t pType = header->type;
    src = header->source;
    seq = header->sequence;
    k_val = header->k;

    if (pType == 4){
        uint16_t d[3];
        d[0] = header->destination1;
        d[1] = header->destination2;
        d[2] = header->destination3;

        for (int i = 0; i<3; i++){
            if (d[i] != 0){
                dest[num_dest] = d[i];
                num_dest++;
            }
        }

        len = header->length;
        data_payload = header->payload;
    }

    return pType;
}


void ForwardKN::chooseKdest(){
    click_chatter("Forward Element - Beginning Forwarding Algorithm");
    if (k_val==3){
        findNextHop();
    }
    if (k_val==2){
        if (num_dest>k_val){
            //Find index of highest cost destination
            int maxIndex = 0;
            for (int i = 1; i < num_dest; i++){
                if (costs[i] > costs[maxIndex]){
                    maxIndex = i;
                }
            }

            //Remove the highest cost destination from dest and possibleHops
            uint16_t tmpHops[3][4];
            uint16_t tmpDest[3];
            int tmp_index=0;
            for (int i = 0; i<num_dest;i++){
                if (i != maxIndex){
                    for (int j = 0; j<4;j++){
                        tmpHops[tmp_index][j] = possibleHops[i][j];
                    }
                    tmpDest[tmp_index] = dest[i];
                    tmp_index++;
                }
            }
            num_dest = tmp_index+1;
            memcpy(possibleHops, tmpHops, sizeof(possibleHops));
            memcpy(dest, tmpDest, sizeof(dest));

        }
        findNextHop();
    }
    if (k_val==1){
        int leastIndex = 0;
        for (int i = 1; i< num_dest; i++){
            if (costs[i] < costs[leastIndex]){
                leastIndex = i;
            }
        }
        int dest_index[] = {0,0,0};
        dest_index[leastIndex] = 1;
        forwardPacket(1, dest_index, possibleHops[leastIndex][0]);
    }
}

void ForwardKN::findNextHop(){
    bool done = false;
    //Finding common nextHops for 3-pair, k=3 only
    if (k_val == 3){
        for (int i1 = 0; i1<possibleHops[i1][4]; i1++){
            for (int i2 = 0; i2<possibleHops[i2][4]; i2++){
                for (int i3 = 0; i3<possibleHops[i3][4]; i3++){
                    if (possibleHops[0][i1] == possibleHops[1][i2] && possibleHops[1][i2] == possibleHops[2][i3]){
                        int dest_index[] = {1,1,1};
                        forwardPacket(3, dest_index, possibleHops[0][i1]);
                        done = true;
                    }
                }
            }
        }
    }

    //Finding common nextHops for 2-pair. Will run this loop for either k = 2 or when k = 3 and 3-pair not found
    if (!done){
        for (int i1 = 0; i1 < k_val-1; i1++){
            for (int j1 = i1+1; j1<k_val; j1++){
            //Only check for a common nextHop if the current choices are different
                for (int i2 = 0; i2<possibleHops[i1][4]; i2++){
                    for (int j2 = 0; j2<possibleHops[j1][4]; j2++){
                        if (possibleHops[i1][i2] == possibleHops[j1][j2]){
                            //Put multicasted destinations together in new array
                            int dest_index[] = {1,1,1};
                            dest_index[3-i1-i2] = 0;
                            forwardPacket(2, dest_index, possibleHops[i1][i2]);
                            //Still need to send last packet if k = 3
                            if (k_val==3){
                                int dest_index[] = {0,0,0};
                                dest_index[3-i1-i2] = 1;
                                forwardPacket(1, dest_index, possibleHops[3-i1-i2][0]);
                            }
                            done = true;
                        }
                    }
                }
            }
        }
    }
    //Send out unicast packets if no 3 or 2-pairs are found
    if (!done){
        int dest_index[] = {0,0,0};
        for (int i = 0; i<k_val; i++){
            dest_index[i] = 1;
            forwardPacket(1, dest_index, possibleHops[i][0]);
        }
    }

}

void ForwardKN::forwardPacket(uint8_t new_k, int dest_index[3], uint16_t nextHop){

    // make click packet with size of DATA packet format
    WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketDATA) + len, 0);

    // set data to 0?
    memset(packet->data(),0,packet->length());

    // make format point to data part of click packet where UPDATE packet is stored
    struct PacketDATA *format = (struct PacketDATA*) packet->data();

    uint16_t destinations[] = {0,0,0};
    for (int i = 0; i < 3; i++){
        if (dest_index[i] == 1){
            destinations[i] = dest[i];
        }
    }

    format->type = 4;
    format->source = src;
    format->sequence = seq;
    format->k = new_k;
    format->destination1 = destinations[0];
    format->destination2 = destinations[1];
    format->destination3 = destinations[2];
    format->length = len;
    format->payload = data_payload;

    int anno = PAINT_ANNO_OFFSET;
    packet->set_anno_u16(anno, nextHop);

    output(0).push(packet);

    click_chatter("Forwarding packet with k: %u", new_k);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ForwardKN)



//------End of Forwarding Element--------//

CLICK_DECLS

McSwitch::McSwitch(){
}
McSwitch::~McSwitch(){
    port0_addr = 0;
    port1_addr = 0;
    port2_addr = 0;
    port3_addr = 0;
    port4_addr = 0;
    port5_addr = 0;
    port6_addr = 0;
    port7_addr = 0;
}

int McSwitch::initialize(ErrorHandler *errh){
    return 0;
}

int McSwitch::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
                  "PORT0_ADDR", cpkP+cpkM, cpUnsigned, &port0_addr,
                  "PORT1_ADDR", cpkP+cpkM, cpUnsigned, &port1_addr,
                  "PORT2_ADDR", cpkP+cpkM, cpUnsigned, &port2_addr,
                  "PORT3_ADDR", cpkP+cpkM, cpUnsigned, &port3_addr,
                  "PORT4_ADDR", cpkP+cpkM, cpUnsigned, &port4_addr,
                  "PORT5_ADDR", cpkP+cpkM, cpUnsigned, &port5_addr,
                  "PORT6_ADDR", cpkP+cpkM, cpUnsigned, &port6_addr,
                  "PORT7_ADDR", cpkP+cpkM, cpUnsigned, &port7_addr,
                  cpEnd) < 0) {
    return -1;
  }
  return 0;
}

void McSwitch::push(int port, Packet *packet) {

    assert(packet);
    struct PacketType *header = (struct PacketType *)packet->data();

    if (header->type == 1 or header->type == 2){
        for(int i = 0; i < noutputs(); i++){
            output(i).push(packet->clone());
        }
    }
    if (header->type==3){

        uint16_t next_addr;

        struct PacketACK *header = (struct PacketACK *)packet->data();

        next_addr = header->destination;

        if ( next_addr == port0_addr ){
            output(0).push(packet);
        }
        else if ( next_addr == port1_addr ){
            output(1).push(packet);
        }
        else if ( next_addr == port2_addr ){
            output(2).push(packet);
        }
        else if ( next_addr == port3_addr ){
            output(3).push(packet);
        }
        else if ( next_addr == port4_addr ){
            output(4).push(packet);
        }
        else if ( next_addr == port5_addr ){
            output(5).push(packet);
        }
        else if ( next_addr == port6_addr ){
            output(6).push(packet);
        }
        else if ( next_addr == port7_addr ){
            output(7).push(packet);
        }
        else {
            //packet->kill();
        }
    }
    else {
        //packet->kill();
    }
    if (header->type == 4){
        int anno = PAINT_ANNO_OFFSET;
        // int next_port = _ports_table.get(packet->anno_u16(anno));
        uint16_t next_addr;
        next_addr = packet->anno_u16(anno);

        if ( next_addr == port0_addr ){
            output(0).push(packet);
        }
        else if ( next_addr == port1_addr ){
            output(1).push(packet);
        }
        else if ( next_addr == port2_addr ){
            output(2).push(packet);
        }
        else if ( next_addr == port3_addr ){
            output(3).push(packet);
        }
        else if ( next_addr == port4_addr ){
            output(4).push(packet);
        }
        else if ( next_addr == port5_addr ){
            output(5).push(packet);
        }
        else if ( next_addr == port6_addr ){
            output(6).push(packet);
        }
        else if ( next_addr == port7_addr ){
            output(7).push(packet);
        }
        else {
            //packet->kill();
        }
    }
    else {
        //packet->kill();
    }
}
CLICK_ENDDECLS
EXPORT_ELEMENT(McSwitch)
