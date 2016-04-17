#ifndef CLICK_TOPOLOGY_HH
#define CLICK_TOPOLOGY_HH
#include <click/element.hh>
#include <click/timer.hh>
#include "routing_entry.hh"

CLICK_DECLS

class Topology : public Element {
    public:
        Topology();
        ~Topology();
        const char *class_name() const { return "Topology";}
        const char *port_count() const { return "1/1";}
        const char *processing() const { return PUSH; }
        int configure(Vector<String> &conf, ErrorHandler *errh);

        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);

        int entry_num;

    private:
        bool first_entry;
        bool updated_entry;
        int new_index;
        routing_entry *routing_table;
};

CLICK_ENDDECLS
#endif
