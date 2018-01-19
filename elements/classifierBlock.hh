#ifndef CLICK_CLASSIFERBLOCK_HH
#define CLICK_CLASSIFERBLOCK_HH
#include <click/element.hh>

CLICK_DECLS
class ClassifierBlock : public Element {
    public:
        ClassifierBlock();
        ~ClassifierBlock(); 
        
        const char *class_name() const { return "ClassifierBlock";}
        const char *port_count() const { return "1/4";}
        const char *processing() const { return PUSH; }

		void push(int port, Packet *packet);
		int initialize(ErrorHandler*);
};

CLICK_ENDDECLS
#endif
