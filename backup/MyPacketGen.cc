#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include "MyPacketGen.hh"
#include "packets.hh"

CLICK_DECLS

MyPacketGen::MyPacketGen() : _timer(this), current_loop(0) {}
MyPacketGen::~MyPacketGen(){}

int MyPacketGen::configure(Vector<String> &conf, ErrorHandler *errh) {
	return 0;
}

int MyPacketGen::initialize(ErrorHandler *) {
	_timer.initialize(this);   // Initialize timer object (mandatory).
	_timer.schedule_after_sec(2);
	return 0;
}

void MyPacketGen::run_timer(Timer *timer) {
	// This function is called when the timer fires.
	Timestamp now = Timestamp::now();
	click_chatter("%s: %{timestamp}: timer fired!\n",
               declaration().c_str(), &now);
	if(current_loop == 0) {
		this->sendRequest();
	} else if (current_loop == 1) {
		this->sendResponse();
	} else {
		this->sendUpdate();
	}
	current_loop++;
	if(current_loop < 3 ){
		_timer.reschedule_after_sec(2);
	}
}

void MyPacketGen::sendRequest() {
	click_chatter("Sending request");
	WritablePacket *packet = Packet::make(0, 0, sizeof(request_header_t), 0);
	request_header_t *header = (request_header_t *)packet->data();
	header->type = REQUEST;
	header->content_id = 0;
	output(0).push(packet);
}

void MyPacketGen::sendResponse() {
	click_chatter("Sending response");
	//Creating space for content of size 10
	WritablePacket *packet = Packet::make(0, 0, sizeof(response_header_t) + 10, 0);
	response_header_t *header = (response_header_t *)packet->data();
	header->type = RESPONSE;
	header->content_id = 0;
	output(0).push(packet);
}

void MyPacketGen::sendUpdate() {
	click_chatter("Sending update");
	WritablePacket *packet = Packet::make(0, 0, sizeof(update_header_t), 0);
	update_header_t *header = (update_header_t *)packet->data();
	header->type = UPDATE;
	header->content_id = 0;
	output(0).push(packet);
}

void MyPacketGen::push(int port, Packet *p) {
	click_chatter("ERROR: this should not happen");
	return;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(MyPacketGen)
