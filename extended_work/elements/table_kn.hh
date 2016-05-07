#ifndef CLICK_TableKN_HH
#define CLICK_TableKN_HH
#include <click/element.hh>
#include <click/vector.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
CLICK_DECLS

struct routing_entry{
    uint16_t destination;
    uint8_t cost;
    uint16_t next_hop;
};

class TableKN: public Element{
    public:
        TableKN();
        ~TableKN();

        const char *class_name() const  { return "TableKN"; }
        const char *port_count() const  { return "0/0"; }
        const char *processing() const  { return PUSH; }

        void hello_update(uint16_t h_source);
        void adjust_size(int new_size);
        void update_entry(int entry, uint16_t dest, uint8_t cost, uint16_t next_hop);
        void add_new_entry(uint16_t dest, uint8_t cost, uint16_t next_hop);
        void remove_table_entries(int remove_entries[], int remove_cnt);

        int get_entry_num();
        routing_entry *get_all_entries();
        void print_table();

    private:
        bool first_entry;
        int entry_num;
        routing_entry *routing_table;
        HashTable<int, int> ports_table;
};

CLICK_ENDDECLS
#endif