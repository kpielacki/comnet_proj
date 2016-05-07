#ifndef CLICK_MYCLASSIFIER_HH
#define CLICK_MYCLASSIFIER_HH

#include <click/element.hh>

CLICK_DECLS

class MyClassifier : public Element {

public:
	MyClassifier();
	~MyClassifier();
	
	const char *class_name() const { return "MyClassifier";}
	const char *port_count() const { return "1/3"; }
	const char *processing() const { return PUSH; }
	
	int configure(Vector<String>&, ErrorHandler*);
	void push(int, Packet *);
	
private:

};

CLICK_ENDDECLS

#endif
