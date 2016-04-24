#ifndef CLICK_RoutingKN_HH
#define CLICK_RoutingKN_HH
#include <click/element.hh>
#include <click/timer.hh>
#include "routing_entry.hh"


class TableKN {
    public:
        TableKN();
        ~TableKN();

        void hello_update(uint16_t h_source);
        void adjust_size(int new_size);
        void update_entry(int entry, uint16_t dest, uint8_t cost);

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
        const char *port_count() const { return "1/1";}
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
        const char *port_count() const { return "1/1";}
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
