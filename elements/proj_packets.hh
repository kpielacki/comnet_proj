struct routing_entry{
    uint16_t destination;
    uint8_t cost;
    uint16_t next_hop;
};

struct PacketHELLO{
     uint8_t type;
	 uint16_t source;
	 uint8_t sequence;
};

struct PacketUPDATE{
     uint8_t type;
	 uint16_t source;
	 uint8_t sequence;
	 uint16_t length;
     routing_entry payload[];
};

struct PacketACK{
     uint8_t type;
	 uint16_t source;
	 uint8_t sequence;
	 uint16_t destination;
};

struct PacketDATA{
     uint8_t type;
	 uint16_t source;
     uint8_t sequence;
	 uint8_t k;
	 uint16_t destination1;
	 uint16_t destination2;
	 uint16_t destination3;
	 uint16_t length;
     uint64_t payload;
};

struct PacketType{
     uint8_t type;
};
