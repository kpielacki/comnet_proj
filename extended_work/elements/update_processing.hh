#ifndef CLICK_UpdateProcessing_HH
#define CLICK_UpdateProcessing_HH

#include <click/element.hh>
#include <click/timer.hh>
// #include "proj_packets.hh"
#include "table_kn.hh"

CLICK_DECLS

class UpdateProcessing : public Element {
    public:
        UpdateProcessing();
        ~UpdateProcessing();
        const char *class_name() const { return "UpdateProcessing";}
        const char *port_count() const { return "1/0";}
        const char *processing() const { return PUSH; }
        int configure(Vector<String> &conf, ErrorHandler *errh);

        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);

    private:
        int seq;
        TableKN *r_table;
};

CLICK_ENDDECLS
#endif
