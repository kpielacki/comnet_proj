#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include "proj_packets.hh"
#include "classifierBlock.hh"

CLICK_DECLS
ClassifierBlock::ClassifierBlock(){}
ClassifierBlock::~ClassifierBlock(){}

int ClassifierBlock::initialize(ErrorHandler*){
    return 0;
}

void ClassifierBlock::push(int port, Packet *packet) {
	assert(packet);
	struct PacketHeader *header = (struct PacketHeader *)packet->data();
	//Type == 1 -> Hello
	if(header->type == 1) {
		output(0).push(packet);
	}
	//Type == 2 -> Update
	else if(header->type == 2) {
		output(1).push(packet);
	}
	//Type == 3 -> Ack
	else if(header->type == 3) {
		output(2).push(packet);
	}
	//Type == 4 -> Data
	else if(header->type == 4) {
		output(3).push(packet);
	}
	else {
		click_chatter("Wrong packet type");
		packet->kill();
	}
}


CLICK_ENDDECLS 
EXPORT_ELEMENT(ClassifierBlock)
