#ifndef CLICK_HELLOPACKETGENERATOR_HH
#define CLICK_HELLOPACKETGENERATOR_HH
#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class HELLOpacketGenerator : public Element {
    public:
        HELLOpacketGenerator();
        ~HELLOpacketGenerator();
        const char *class_name() const { return "HELLOpacketGenerator";}
        const char *port_count() const { return "0/1";}
        const char *processing() const { return PUSH; }

        void run_timer(Timer*);
        int initialize(ErrorHandler*);
        int configure(Vector<String> &conf, ErrorHandler *errh);

    private:
        Timer _timer;
        int seq;
        uint16_t _my_host;
};

CLICK_ENDDECLS
#endif
