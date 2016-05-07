#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include "MyClassifier.hh"
#include "packets.hh"

CLICK_DECLS

MyClassifier::MyClassifier(){}
MyClassifier::~MyClassifier(){}

int MyClassifier::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}

void MyClassifier::push(int port, Packet *p) {
	if (port != 0){
		click_chatter("ERROR: Got a packet on wrong port");
		return;
	}
	const unsigned char *data = p->data();
	msg_t type = (msg_t)data[0];
	if (type == REQUEST) {
		output(0).push(p);
	} else if(type == RESPONSE) {
		output(1).push(p);
	} else if(type == UPDATE) {
		output(2).push(p);
	} else {
		p->kill();
	}
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MyClassifier)
