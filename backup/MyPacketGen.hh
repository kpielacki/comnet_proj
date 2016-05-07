#ifndef CLICK_MYPACKETGEN_HH
#define CLICK_MYPACKETGEN_HH

#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class MyPacketGen : public Element {

public:
	MyPacketGen();
	~MyPacketGen();
	
	const char *class_name() const { return "MyPacketGen";}
	const char *port_count() const { return "0/1"; }
	const char *processing() const { return PUSH; }
	
	int configure(Vector<String>&, ErrorHandler*);
	void push(int, Packet *);
	int initialize(ErrorHandler *errh);
	void run_timer(Timer *timer);
	void sendRequest();
	void sendResponse();
	void sendUpdate();
	
private:
	Timer _timer;
	int current_loop;
};

CLICK_ENDDECLS

#endif
