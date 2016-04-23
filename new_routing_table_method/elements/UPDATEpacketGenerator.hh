#ifndef CLICK_UPDATEPACKETGENERATOR_HH
#define CLICK_UPDATEPACKETGENERATOR_HH
#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class UPDATEpacketGenerator : public Element {
    public:
        UPDATEpacketGenerator();
        ~UPDATEpacketGenerator();
        const char *class_name() const { return "UPDATEpacketGenerator";}
        const char *port_count() const { return "0/1";}
        const char *processing() const { return PUSH; }

        void run_timer(Timer*);
        int initialize(ErrorHandler*);

    private:
        Timer _timer;
        int seq;
};

CLICK_ENDDECLS
#endif
