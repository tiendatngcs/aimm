#ifndef _PACKET_H_
#define _PACKET_H_

#include <iostream>
#include "extern_vars.hpp"

class packet{
  public:
    packet();
    packet(unsigned long, int, int){}//creation time, src, dest
    ~packet(){}
    unsigned long pid;
    unsigned long id;//all the packet ids are unique
    unsigned long opid;//operation id
    unsigned long resp_for_pkt_id;//all the packet ids are unique
    unsigned long pa;//physical address for the address
    unsigned long c_time;
    unsigned long entry_time;
    unsigned long ntw_entry_time;
    unsigned long mcq_delay;
    int src;
    long long dest;
    unsigned long retry_to_send;
    unsigned long retry_to_mem;
    int hops;
    bool needs_resp;//true if this pkt contains a mem request
    int pkt_size;//number of flits
    int lat;
    int mem_acc_lat;
    bool rd_req;
    bool wr_req;
    bool pim_operation;
    long long pim_src1, pim_src2;//node numbers --> mc.cpp
    long long pim_table_index;
    bool waiting_for_pim_sources;
    unsigned long pim_src1_pa;
    unsigned long pim_src2_pa;
    unsigned long dest_pa;
    unsigned long dest_va;
    unsigned long src1_va;
    unsigned long src2_va;
    bool migration_pkt;
    bool make_entry_pim_table;
    unsigned long not_ready_to_send;
    unsigned long migr_pg_addr;
    bool migr_head;
    bool migr_tail;
    int forward_dest;//for forwarding the result to org location
    bool forward_pkt;
    bool pei_request;

    void mark_entry_time(unsigned long);
    unsigned long get_entry_time(){return entry_time;}
    bool ready_to_send();
    int get_dest(){return dest;}
    bool ready_to_retire();
};
#endif
