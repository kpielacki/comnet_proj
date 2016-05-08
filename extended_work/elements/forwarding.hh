#ifndef CLICK_FORWARDING_HH
#define CLICK_FORWARDING_HH
#include <click/element.hh>
#include <click/timer.hh>
#include "table_kn.hh"

class Forwarding : public Element {
    public:
        Forwarding();
        ~Forwarding();

        const char *class_name() const { return "Forwarding";}
        const char *port_count() const { return "1/1";}
        const char *processing() const { return PUSH; }
        int configure(Vector<String> &conf, ErrorHandler *errh);

        void push(Packet *packet);
        uint8_t readPacket(); //returns packet Type, stores all other packet information in class variables
        void chooseKdest();
        void findNextHop();
        void forwardPacket(uint8_t new_k, int dest_index[3], uint16_t nextHop);

    private:
        uint16_t src;
        uint8_t seq;
        uint8_t k_val;
        uint16_t dest[3];
        int num_dest;
        int dest_index[];
        uint8_t len;
        uint64_t data_payload;
        uint16_t possibleHops[3][4];
        uint8_t costs[3];
        Packet *pckt;
        TableKN *r_table;
};

CLICK_ENDDECLS
#endif
