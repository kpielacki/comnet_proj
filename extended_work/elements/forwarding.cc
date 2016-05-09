#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include <click/packet_anno.hh>
#include "forwarding.hh"

CLICK_DECLS

Forwarding::Forwarding()  : r_table(0) {

}

Forwarding::~Forwarding(){

}

int Forwarding::initialize(ErrorHandler *errh){
    return 0;
}

int Forwarding::configure(Vector<String> &conf, ErrorHandler *errh){
    if (cp_va_kparse(conf, this, errh, "ROUTINGTABLE", cpkN, cpElement, &r_table, cpEnd) < 0) return -1;
    return 0;
}

void Forwarding::push(int port, Packet *packet){
    num_dest = 0;
    pckt = packet;
    uint8_t pType = readPacket();

    //Find routing table entries
    if ( pType == 0 ){
        goto quit;
    }
    else if (pType==4){
        //Make array for possibleHops, set number of routes for each destination to 0
        for (int i = 0; i<num_dest; i++){
            possibleHops[i][3] = 0;
        }

        routing_entry* forwarding_table = r_table->get_all_entries();
        for (int i = 0; i<num_dest; i++){
            for (int entry=0; entry <= r_table->get_entry_num(); entry++){
                if (forwarding_table[entry].destination == dest[i]){
                    costs[i] = forwarding_table[entry].cost;
                    possibleHops[i][possibleHops[i][3]] = forwarding_table[entry].next_hop;
                    possibleHops[i][3]++;
                }
            }


            //Discard packet if routing entry not found
            if (possibleHops[i][3]==0){
                //packet->kill();
                click_chatter("ERROR - Forward Element - addr: %u not found in table. Terminating" , dest[i]);
                goto quit;
            }
        }

        if ( k_val > num_dest){
            click_chatter("ERROR - Value of k is greater than number of destinations. Terminating");
            goto quit;
        }

        //Code to print out possibleHops
        /* for(int i = 0; i <3; i++){
            click_chatter("%u possibleHops D%u: %u %u %u %u", r_table->get_my_host(), (i+1), possibleHops[i][0], possibleHops[i][1], possibleHops[i][2], possibleHops[i][3]);
        } */

        //Perform forwarding algorithm
        chooseKdest();
    }

    quit: ;
}

//returns packet Type, stores all other packet information in class variables
uint8_t Forwarding::readPacket(){

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
        click_chatter("%u found d1: %u", r_table->get_my_host(), header->destination1);
        click_chatter("%u found d2: %u", r_table->get_my_host(), header->destination2);
        click_chatter("%u found d3: %u", r_table->get_my_host(), header->destination3);
        for (int i = 0; i<3; i++){
            if ( d[i] == r_table->get_my_host() ){
                // packet was meant for me
                click_chatter("Host %u has received the packet with data: %u", r_table->get_my_host(), header->payload);
                pType = 0;
                return pType;
            }
            else if (d[i] != 0){
                dest[num_dest] = d[i];
                num_dest++;
            }
        }

        len = header->length;
        data_payload = header->payload;
    }

    return pType;
}


void Forwarding::chooseKdest(){
    if (k_val==3){
        findNextHop();
    }
    else if (k_val==2){
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
    else if (k_val==1){
        int leastIndex = 0;
        for (int i = 0; i< num_dest; i++){
            if (costs[i] < costs[leastIndex]){
                leastIndex = i;
            }
        }
        int dest_index[] = {0,0,0};
        dest_index[leastIndex] = 1;
        forwardPacket(1, dest_index, possibleHops[leastIndex][0]);
    }
    else{
        click_chatter("Forward Element - k is invalid!");
    }
}

void Forwarding::findNextHop(){
    bool done = false;
    //Finding common nextHops for 3-pair, k=3 only
    if (k_val == 3){
        for (int i1 = 0; i1<possibleHops[0][3]; i1++){
            for (int i2 = 0; i2<possibleHops[1][3]; i2++){
                for (int i3 = 0; i3<possibleHops[2][3]; i3++){
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
                for (int i2 = 0; i2<possibleHops[i1][3]; i2++){
                    for (int j2 = 0; j2<possibleHops[j1][3]; j2++){
                        if (possibleHops[i1][i2] == possibleHops[j1][j2]){
                            //Put multicasted destinations together in new array
                            int dest_index[] = {1,1,1};
                            dest_index[3-i1-j1] = 0;
                            forwardPacket(2, dest_index, possibleHops[i1][i2]);
                            //Still need to send last packet if k = 3
                            if (k_val==3){
                                int dest_index[] = {0,0,0};
                                dest_index[3-i1-j1] = 1;
                                forwardPacket(1, dest_index, possibleHops[3-i1-j1][0]);
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

void Forwarding::forwardPacket(uint8_t new_k, int dest_index[3], uint16_t nextHop){

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
    click_chatter("%u forwarding packet with k: %u to addr: %u", r_table->get_my_host(), new_k, nextHop);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Forwarding)
