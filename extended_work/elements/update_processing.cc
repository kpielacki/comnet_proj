#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "update_processing.hh"

CLICK_DECLS

// struct routing_entry{
//     uint16_t destination;
//     uint8_t cost;
//     uint16_t next_hop;
// };

UpdateProcessing::UpdateProcessing() : r_table(0) {
    seq = 0;
}

UpdateProcessing::~UpdateProcessing(){

}

int UpdateProcessing::initialize(ErrorHandler *errh){
    return 0;
}

int UpdateProcessing::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh, "ROUTINGTABLE", cpkN, cpElement, &r_table, cpEnd) < 0) return -1;
    return 0;
}

void UpdateProcessing::push(int port, Packet *packet) {
    assert(packet);

    // look at update packet
    struct PacketUPDATE *update_packet = (struct PacketUPDATE *)packet->data();

    if ( update_packet->type != 2 ){
        packet->kill();
    }
    else {
        //routing_entry* pay_read = new routing_entry[update_packet->length];
        //memcpy( pay_read, &update_packet->payload, update_packet->length * sizeof(routing_entry) );

        int r_entry;
        click_chatter("Received UPDATE Packet Length: %u", update_packet->length);
        click_chatter("Received UPDATE Packet Source: %u", update_packet->source);
        click_chatter("\n\n");


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
            routing_entry* retrieved_table = r_table->get_all_entries();

            for( entry=0; entry <= r_table->get_entry_num(); entry++ ){
                found_entry = false;
                // array to hold entry number of matching destinations in routing table
                // int matching_dest_entries[2] = new int[2];  // there should never be more than 3 of the same entry in a table
                equal_hop_count = 0;
                // better_hop_count = 0;

                if ( !r_table->get_if_first_entry() ) {
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
            }

            // add equal entry if room available
            if ( !same_entry and equal_hop_count > 0 and equal_hop_count < 3 ) {
                r_table->add_new_entry(update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);
            }

            if ( better_hop_count > 0 ) {
                r_table->update_entry(overwrite_entry, update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);

                // remove alternative routes if exists
                if ( better_hop_count > 1) {
                    r_table->remove_table_entries(del_entries, better_hop_count - 1);
                }
            }
            // add entry to routing table if entry was not found
            if ( !found_entry and !same_entry and better_hop_count <= 0 and !discard_entry ){
                r_table->add_new_entry(update_packet->payload[r_entry].destination, update_packet->payload[r_entry].cost + 1, update_packet->source);
                found_entry = false;
            }
        }

    packet->kill();
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(UpdateProcessing)
