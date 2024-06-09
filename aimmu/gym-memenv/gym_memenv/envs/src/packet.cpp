#include "packet.hpp"

packet::packet(){
  opid = 0;//0 is invalid, as opid starts from 1
  pid = 0;
  src = -1;
  dest = -1;
  id = gpacket_id++;
  resp_for_pkt_id = 0;
  pa = 0;
  c_time = 0;
  entry_time = 0;
  ntw_entry_time = 0;
  mcq_delay = 0;
  retry_to_send = 0;
  retry_to_mem = 0;
  hops = 0;
  needs_resp = false;
  lat = 0;
  mem_acc_lat = -1;
  rd_req = false;
  wr_req = false;
  pim_operation = false;
  pim_src1 = -1;
  pim_src2 = -1;
  pim_table_index = -1;
  waiting_for_pim_sources = false;
  pim_src1_pa = 0;
  pim_src2_pa = 0;
  dest_pa = 0;
  dest_va = 0;
  src1_va = 0;
  src2_va = 0;
  migration_pkt = false;
  make_entry_pim_table = false;
  not_ready_to_send = 0;
  pkt_size = 1;
  migr_pg_addr = 0;
  migr_head = false;
  migr_tail = false;
  forward_dest = -1;//for forwarding the result to org location
  forward_pkt = false;
  pei_request = false;
}
    
bool packet::ready_to_send(){
      return (global_clock - entry_time) > (router_stages + pkt_size);
      //return (global_clock - entry_time) > 0;
}//takes care if the packet size
    
bool packet::ready_to_retire(){
  bool ready = (global_clock - entry_time) >= mem_acc_lat;
  if(ready==false){
    stats_pkt_not_ready_to_retire++;
  } 
  else{
    if(stats_pkt_not_ready_to_retire>0){
     stats_pkt_not_ready_to_retire--;
    }
  } 
  return ready;
}//memory access latency
