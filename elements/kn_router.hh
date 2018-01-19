#ifndef CLICK_RoutingKN_HH
#define CLICK_RoutingKN_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include "proj_packets.hh"


class TableKN {
    public:
        TableKN();
        ~TableKN();

        void hello_update(uint16_t h_source);
        void adjust_size(int new_size);
        void update_entry(int entry, uint16_t dest, uint8_t cost, uint16_t next_hop);
        void add_new_entry(uint16_t dest, uint8_t cost, uint16_t next_hop);
        void remove_table_entries(int remove_entries[], int remove_cnt);

        int get_entry_num();
        routing_entry *get_all_entries();
        void print_table();

    private:
        bool first_entry;
        static int entry_num;
        static routing_entry *routing_table;
};


CLICK_DECLS

class RoutingKN : public Element {
    public:
        RoutingKN();
        ~RoutingKN();
        const char *class_name() const { return "RoutingKN";}
        const char *port_count() const { return "1/0";}
        const char *processing() const { return PUSH; }
        int configure(Vector<String> &conf, ErrorHandler *errh);

        void run_timer(Timer*);
        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);

    private:
        Timer _timer;
        int seq;
};

CLICK_ENDDECLS
#endif


#ifndef CLICK_TOPOLOGY_HH
#define CLICK_TOPOLOGY_HH

CLICK_DECLS

class Topology : public Element {
    public:
        Topology();
        ~Topology();
        const char *class_name() const { return "Topology";}
        const char *port_count() const { return "1/0";}
        const char *processing() const { return PUSH; }
        int configure(Vector<String> &conf, ErrorHandler *errh);

        void run_timer(Timer*);
        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);

    private:
        Timer _timer;
        int seq;
};

CLICK_ENDDECLS
#endif


#ifndef CLICK_TESTpacketGen_HH
#define CLICK_TESTpacketGen_HH

CLICK_DECLS

class TESTpacketGen : public Element {
    public:
        TESTpacketGen();
        ~TESTpacketGen();
        const char *class_name() const { return "TESTpacketGen";}
        const char *port_count() const { return "0/1";}
        const char *processing() const { return PUSH; }

        void run_timer(Timer*);
        int initialize(ErrorHandler*);

    private:
        Timer _timer;
        int seq;

        routing_entry *export_table;
};

CLICK_ENDDECLS
#endif


#ifndef CLICK_packetGen_HH
#define CLICK_packetGen_HH

CLICK_DECLS

class packetGen : public Element {
    public:
        packetGen();
        ~packetGen();
        const char *class_name() const { return "packetGen";}
        const char *port_count() const { return "1/6";}
        const char *processing() const { return PUSH; }

        void run_timer(Timer*);
        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);
        int configure(Vector<String> &conf, ErrorHandler *errh);

    private:
        int last_tran;
        uint16_t _my_host;
        int _client;

        Timer _timerHELLO_TO;
        Timer _timerHELLO;
        Timer _timerUPDATE_TO;
        Timer _timerUPDATE;
        Timer _timerPrintTable;
        Timer _timerDATA;

        int seq;
};

CLICK_ENDDECLS
#endif

#ifndef CLICK_FORWARDKN_HH
#define CLICK_FORWARDKN_HH

class ForwardKN : public Element {
    public:
        ForwardKN();
        ~ForwardKN();

        const char *class_name() const { return "ForwardKN";}
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

};

CLICK_ENDDECLS
#endif

#ifndef CLICK_MCSWITCH_HH
#define CLICK_MCSWITCH_HH

CLICK_DECLS

class McSwitch : public Element {
    public:
        McSwitch();
        ~McSwitch();
        const char *class_name() const { return "McSwitch";}
        const char *port_count() const { return "4/1-";}
        const char *processing() const { return PUSH; }

        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);
        int configure(Vector<String> &conf, ErrorHandler *errh);

    private:
        HashTable<int, int> _ports_table;
        int _output;
        int16_t _anno;
        uint8_t _color;

        uint16_t port0_addr;
        uint16_t port1_addr;
        uint16_t port2_addr;
        uint16_t port3_addr;
        uint16_t port4_addr;
        uint16_t port5_addr;
        uint16_t port6_addr;
        uint16_t port7_addr;
};

CLICK_ENDDECLS
#endif
