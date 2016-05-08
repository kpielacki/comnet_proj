#ifndef CLICK_ClassifierAndGenerator_HH
#define CLICK_ClassifierAndGenerator_HH

#include <click/element.hh>
#include <click/timer.hh>
#include "table_kn.hh"

CLICK_DECLS

class ClassifierAndGenerator : public Element {
    public:
        ClassifierAndGenerator();
        ~ClassifierAndGenerator();
        const char *class_name() const { return "ClassifierAndGenerator";}
        const char *port_count() const { return "1-/5";}
        // ---Processing Input Ports---
        // port 0 to switch sending periodic HELLO's and UPDATE's
        // port 1 to switch for sending ACK
        // port 2 to forwarding stage for handling DATA
        // ---Processing Output Ports---
        // port 3 to HELLO processing
        // port 4 to UPDATE processing

        const char *processing() const { return PUSH; }

        void run_timer(Timer*);
        void push(int port, Packet *packet);
        int initialize(ErrorHandler*);
        int configure(Vector<String> &conf, ErrorHandler *errh);

    private:
        int last_tran;
        int acks_left;
        TableKN *r_table;

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
