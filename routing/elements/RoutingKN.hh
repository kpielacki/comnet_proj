#ifndef CLICK_RoutingKN_HH
#define CLICK_RoutingKN_HH
#include <click/element.hh>
#include <click/timer.hh>

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

        int entry_num;

    private:
        Timer _timer;
        int seq;
};

CLICK_ENDDECLS
#endif
