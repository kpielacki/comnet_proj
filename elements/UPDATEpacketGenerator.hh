#ifndef CLICK_UpdatepacketGen_HH
#define CLICK_UpdatepacketGen_HH

CLICK_DECLS

class UpdatepacketGen : public Element {
    public:
        UpdatepacketGen();
        ~UpdatepacketGen();
        const char *class_name() const { return "UpdatepacketGen";}
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
