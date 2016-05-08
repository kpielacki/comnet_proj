#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include <click/args.hh>
#include <click/glue.hh>
#include <click/packet_anno.hh>
#include "mcswitch.hh"
#include "table_kn.hh"
// #include "proj_packets.hh"


CLICK_DECLS

McSwitch::McSwitch(){}
McSwitch::~McSwitch(){}

int McSwitch::initialize(ErrorHandler *errh){
	return 0;
}

int McSwitch::configure(Vector<String> &conf, ErrorHandler *errh) {
	if (cp_va_kparse(conf, this, errh, "ROUTINGTABLE", cpkN, cpElement, &r_table, cpEnd) < 0) return -1;
	return 0;
}

void McSwitch::push(int port, Packet *packet) {
	assert(packet);
	struct PacketType *header = (struct PacketType *)packet->data();
	//If HELLO or UPDATE is received on port 0
	if (port == 0){
		for(int i = 0; i < noutputs(); i++){
			output(i).push(packet->clone());
		}
	}
	//Received ACK
	else if (port == 1){
		struct PacketACK *header1 = (struct PacketACK *)packet->data();
		int next_port = r_table->getPort(header1->destination);
		output(next_port).push(packet);
	}
	//Received DATA
	else if (port == 2){
		int anno = PAINT_ANNO_OFFSET;
		int next_port = r_table->getPort(packet->anno_u16(anno));

		output(next_port).push(packet);
	}
	else {
		click_chatter("Wrong packet type");
		packet->kill();
	}
}
CLICK_ENDDECLS
EXPORT_ELEMENT(McSwitch)
