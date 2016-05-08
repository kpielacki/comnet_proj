#include <click/config.h>
#include <click/confparse.hh>
#include <click/error.hh>
#include <click/timer.hh>
#include <click/packet.hh>
#include "ClassifierAndGenerator.hh"

CLICK_DECLS

ClassifierAndGenerator::ClassifierAndGenerator() : _timerHELLO(this), _timerHELLO_TO(this), _timerUPDATE(this), _timerUPDATE_TO(this), _timerPrintTable(this), _timerDATA(this), r_table(0) {
    last_tran = 0;
    seq = 0;
    acks_left = ninputs();
}

ClassifierAndGenerator::~ClassifierAndGenerator(){

}

int ClassifierAndGenerator::initialize(ErrorHandler *errh){
    _timerHELLO_TO.initialize(this);
    _timerHELLO_TO.schedule_after_sec(1);
    _timerHELLO.initialize(this);
    _timerHELLO.schedule_after_sec(5);

    _timerUPDATE_TO.initialize(this);
    _timerUPDATE_TO.schedule_after_sec(1);
    _timerUPDATE.initialize(this);
    _timerUPDATE.schedule_after_sec(5);

    _timerPrintTable.initialize(this);
    _timerPrintTable.schedule_after_sec(10);

    _timerDATA.initialize(this);
    _timerDATA.schedule_now();

    return 0;
}

int ClassifierAndGenerator::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (cp_va_kparse(conf, this, errh,
                  "ROUTINGTABLE", cpkN, cpElement, &r_table,
                  cpEnd) < 0) {
    return -1;
  }
  return 0;
}

void ClassifierAndGenerator::run_timer(Timer *timer) {
    // Periodically transmit or retransmit HELLO and UPDATE packet
    if( timer == &_timerHELLO_TO ){
        seq++;
        WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
        memset(packet->data(),0,packet->length());

        struct PacketHELLO *format = (struct PacketHELLO*) packet->data();

        format->type = 1;
        format->source = r_table->get_my_host();
        format->sequence = seq;

        last_tran = 1;
        output(0).push(packet);
        _timerHELLO_TO.schedule_after_sec(1);
    } else if( timer == &_timerHELLO ){
        WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketHELLO), 0);
        memset(packet->data(),0,packet->length());
        struct PacketHELLO *format = (struct PacketHELLO*) packet->data();
        format->type = 1;
        format->source = r_table->get_my_host();
        format->sequence = seq;

        last_tran = 1;
        output(0).push(packet);
        _timerHELLO_TO.schedule_after_sec(1);
    }
    else if ( timer == &_timerUPDATE_TO ) {
        if ( r_table->get_if_first_entry() ) {
            _timerUPDATE.schedule_after_sec(5);
        }
        else {
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketUPDATE), 0);

            memset(packet->data(),0,packet->length());

            // make format point to data part of click packet where UPDATE packet is stored
            struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();

            format->type = 2;
            format->source = r_table->get_my_host();
            format->sequence = seq;
            format->length = r_table->get_entry_num();
            // click_chatter("Timeout Entry Num: %u", r_table->get_entry_num());


            memcpy(format->payload, r_table->get_all_entries(), (format->length + 1)*sizeof(routing_entry));

            last_tran = 2;
            output(0).push(packet);
            _timerUPDATE_TO.schedule_after_sec(1);
        }
    }
    else if( timer == &_timerUPDATE ) {
        if ( r_table->get_if_first_entry() ) {
            _timerUPDATE.schedule_after_sec(5);
        }
        else {
            WritablePacket *packet = Packet::make(14,0,sizeof(struct PacketUPDATE), 0);
            memset(packet->data(),0,packet->length());
            struct PacketUPDATE *format = (struct PacketUPDATE*) packet->data();
            format->type = 2;
            format->source = r_table->get_my_host();
            format->sequence = seq;
            format->length = r_table->get_entry_num();

            memcpy(format->payload, r_table->get_all_entries(), (r_table->get_entry_num() + 1)*sizeof(routing_entry));

            struct PacketUPDATE *format2 = (struct PacketUPDATE*) packet->data();
            // click_chatter("Sending After Click Packet Creation Entry Num: %u", format2->length);

            last_tran = 2;
            output(0).push(packet);
            _timerUPDATE_TO.schedule_after_sec(1);
        }
    }
    else if( timer == &_timerPrintTable ) {
        click_chatter("\n-----Host %u-----", r_table->get_my_host());
        r_table->print_table();
        _timerPrintTable.schedule_after_sec(10);
    }
    else {
        // assert(false);
    }
}

void ClassifierAndGenerator::push(int port, Packet *packet) {

    assert(packet);
    struct PacketType *header = (struct PacketType *)packet->clone()->data();

    // Read type, send ACK, and send to next stage
    if ( header->type == 4 ){
        // Handle Data
        struct PacketDATA *header4 = (struct PacketDATA *)packet->clone()->data();
        WritablePacket *ack = Packet::make(14,0,sizeof(struct PacketACK), 0);
        memset(ack->data(),0,ack->length());
        struct PacketACK *format = (struct PacketACK*) ack->data();
        format->type = 3;
        format->source = r_table->get_my_host();
        format->sequence = header4->sequence;
        format->destination = header4->source;
        r_table->setPort(header4->source, port); //Add addr to ports table
        output(1).push(ack);
        output(2).push(packet);
        // packet->kill();
    }
    else if ( header->type == 1 ){
        // Handle HELLO
        output(3).push(packet->clone());
        struct PacketHELLO *header1 = (struct PacketHELLO *)packet->data();
        WritablePacket *ack = Packet::make(14,0,sizeof(struct PacketACK), 0);
        memset(ack->data(),0,ack->length());
        struct PacketACK *format1 = (struct PacketACK*) ack->data();
        format1->type = 3;
        format1->source = r_table->get_my_host();
        format1->sequence = header1->sequence;
        format1->destination = header1->source;
        r_table->setPort(header1->source, port); //Add addr to ports table
        output(1).push(ack);
        // packet->kill();
    }
    else if ( header->type == 2 ){
        struct PacketUPDATE *header2 = (struct PacketUPDATE *)packet->clone()->data();
        // Handle UPDATE
        output(4).push(packet->clone());
        // click_chatter("After Creation Type %u", header2->type);
        // click_chatter("After Creation Source %u", header2->source);
        // click_chatter("After Creation Seq %u", header2->sequence);
        // click_chatter("After Creation Length %u", header2->length);

        WritablePacket *ack = Packet::make(14,0,sizeof(struct PacketACK), 0);
        memset(ack->data(),0,ack->length());
        struct PacketACK *format = (struct PacketACK*) ack->data();
        format->type = 3;
        format->source = r_table->get_my_host();
        format->sequence = header2->sequence;
        format->destination = header2->source;
        r_table->setPort(header2->source, port); //Add addr to ports table
        output(1).push(ack);
        // click_chatter("---HOST %u---", r_table->get_my_host());
        // click_chatter("After ACK Before Proc Length %u", header2->length);
        // packet->kill();
    }
    else if ( header->type == 3 ) {
        // Handle ACK
        struct PacketACK *header3 = (struct PacketACK *)packet->clone()->data();
        if(header3->sequence == seq) {
            if ( last_tran == 1 ) {
                acks_left--;
                if (acks_left==0){
                    _timerHELLO_TO.unschedule();
                    seq++;
                    _timerHELLO.schedule_after_sec(5);
                    acks_left = ninputs();
                }
            }
            if ( last_tran == 2 ) {
                acks_left--;
                if (acks_left==0){                
                    _timerUPDATE_TO.unschedule();
                    seq++;
                    _timerUPDATE.schedule_after_sec(5);
                    acks_left = ninputs();
                }
            }
            // else {
            //     packet->kill();
            // }
        }
        else {
            packet->kill();
        }
    }
    else {
        // Kill invalid packet
        packet->kill();
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(ClassifierAndGenerator)
