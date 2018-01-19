#ifndef CLICK_MCSWITCH_HH
#define CLICK_MCSWITCH_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include "table_kn.hh"

CLICK_DECLS

class McSwitch : public Element {
    public:
        McSwitch();
        ~McSwitch();
        const char *class_name() const { return "McSwitch";}
        const char *port_count() const { return "3/1-";}
        const char *processing() const { return PUSH; }

        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);
        int configure(Vector<String> &conf, ErrorHandler *errh);

	private:
        TableKN *r_table;

};

CLICK_ENDDECLS
#endif
