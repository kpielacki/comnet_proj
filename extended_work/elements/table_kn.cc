#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "table_kn.hh"

CLICK_DECLS
//--------------ROUTING TABLE CLASS--------------
//routing_entry *routing_table = new routing_entry[0];

TableKN::TableKN(){
    first_entry = true;
    entry_num = 0;
    _my_host = 0;
    _sender = false;
    //static routing_entry *routing_table = new routing_entry[0];
}

TableKN::~TableKN(){

}

int TableKN::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
                  "MY_HOST", cpkP+cpkM, cpUnsigned, &_my_host,
                  "IF_SENDER", cpkP+cpkM, cpBool, &_sender,
                  cpEnd) < 0) {
    return -1;
  }
return 0;
}

uint16_t TableKN::get_my_host() {
    return _my_host;
}

void TableKN::adjust_size(int new_size){

    routing_entry* newArr = new routing_entry[1+new_size];


    memcpy( newArr, routing_table, new_size * sizeof(routing_entry) );

    // if ( routing_table != NULL ) {
    //     click_chatter("TEST");
    //     delete [] routing_table;
    // }

    routing_table = newArr;
    entry_num = new_size;
}

int TableKN::get_entry_num(){
    return entry_num;
}

bool TableKN::get_if_first_entry(){
    return first_entry;
}

bool TableKN::get_if_sender(){
    return _sender;
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

    if ( !first_entry ) {
        click_chatter("-------------Routing Table For Host %u-------------", _my_host);
        for( entry=0; entry <= entry_num; entry++ ){
            click_chatter("Entry: %u, Destination: %u, Cost: %u, Next Hop: %u", entry, routing_table[entry].destination, routing_table[entry].cost, routing_table[entry].next_hop);
        }
        click_chatter("\n");
    }
    else {
        click_chatter("-------------Routing Table-------------");
        click_chatter("Empty\n");
    }
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

void TableKN::setPort(uint16_t addr, int port){
    ports_table.set(addr, port);
}

int TableKN::getPort(uint16_t addr){
    return ports_table.get(addr);
}


CLICK_ENDDECLS
EXPORT_ELEMENT(TableKN)
