#ifndef CLICK_HelloProcessing_HH
#define CLICK_HelloProcessing_HH

#include <click/element.hh>
#include <click/timer.hh>
// #include "proj_packets.hh"
#include "table_kn.hh"

CLICK_DECLS

class HelloProcessing : public Element {
    public:
        HelloProcessing();
        ~HelloProcessing();
        const char *class_name() const { return "HelloProcessing";}
        const char *port_count() const { return "1/0";}
        const char *processing() const { return PUSH; }

        int initialize(ErrorHandler*);
        int configure(Vector<String> &conf, ErrorHandler *errh);
        void push(int port, Packet *packet);

    private:
        int seq;
        TableKN *r_table;
};

CLICK_ENDDECLS
#endif
