#ifndef CLICK_CLASSIFER_HH
#define CLICK_CLASSIFER_HH
#include <click/element.hh>

CLICK_DECLS
class Classifier : public Element {
    public:
        Classifier();
        ~Classifier(); 
        const char *class_name() const { return "Classifier";}
        const char *port_count() const { return "1/4";}
        const char *processing() const { return PUSH; }
		void push(int port, Packet *packet);
		int initialize(ErrorHandler*);
};

CLICK_ENDDECLS
#endif
