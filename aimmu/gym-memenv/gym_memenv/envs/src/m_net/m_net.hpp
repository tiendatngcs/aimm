#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <deque>
#include <math.h>
#include <algorithm>
#include <list>
#include <numeric>
#include <string>
#include <sstream>
#include <cstring>

#include "../configs/config_utils.hpp"
#include "../stats.hpp"
#include "../extern_vars.hpp"
#include "../packet.hpp"
#include "../data_collect.hpp"

using namespace std;

class pim_entry{//each node has one table if pim entries 
  public:
    pim_entry(){
      //cout << "debug pim_entry() " << this << " " << &valid << endl;
      valid = false;//initially all the entries are invalid
      ready_to_do_pim=false;
      src1_ready=true;
      src2_ready=true;
      src1_pkt_id = -1;
      src2_pkt_id = -1;
      pkt=NULL;
    }
    bool valid;
    bool ready_to_do_pim;
    bool src1_ready;//tracking the first pim source
    bool src2_ready;//tracking second pim source
    int src1_pkt_id;
    int src2_pkt_id;
    unsigned long entry_time;
    unsigned long src1_reach_time;
    unsigned long src2_reach_time;
    packet *pkt;//this is the main request packet
};


class node{
  public:
    node(const Configuration &config);
    ~node(){}
    void inject_vc_r_s(int from_node, packet *pkt, int NI_id);
    void make_entry_vc_r_s(int from_node, packet *pkt);
    void make_entry_mem_cube(packet *pkt){mem_cube.push_back(pkt);}
    void make_entry_resp_queue(packet *pkt){resp_queue.push_back(pkt);}
    int record_rowbuff_entry(packet *);
    int vc_r_s_occupancy();
    int mem_cube_occupancy();
    bool can_read(int i){return true;}
    packet *read_front_pkt(int, int);
    bool can_receive_pkt(int from_node, packet *pkt);
    void pop_front_pkt(int port, packet *pkt);
    int get_inport(int from_node, int src);
    bool can_make_entry_for_mem_pim(){return true;}
    bool can_make_entry_for_mem(){return true/*(mem_cube.size() < max_entry_mem_cube)*/;}
    bool check_and_make_response(packet *);
    bool avail_resp_queue(){return true/*(resp_queue.size() < max_entry_resp_q)*/;}
    packet *get_resp_pkt(){return resp_queue.front();}
    void remove_resp_pkt(){resp_queue.pop_front();}
    void send_response();
    bool can_inject_req(int port){return (vc_r_s[5].size() < _max_entry_vc_r_s);}
    bool can_inject_resp(int port){return (vc_r_s_resp[port].size() < _max_entry_vc_r_s);}
    bool has_response(){return (resp_queue.size()>0);}
    packet *get_pim_pkt(){return pim_req_queue.front();}
    void remove_pim_pkt(){pim_req_queue.pop_front();}
    void send_pim_requests();
    bool has_pim_pkt(){return (pim_req_queue.size()>0);}
    void make_pim_packets(int, int, packet *);
    int find_empty_space_pim_tab();
    int return_vc_req_NI_buff_size(int, packet *);
    int return_vc_buff_size(int vnet, int port);
    unsigned long return_resp_queue_size(){return resp_queue.size();}   
    int return_max_entry_vc_r_s(){return _max_entry_vc_r_s;}
    int return_num_pim_entries_reserved(){return _pim_tab_occupancy;}
    void incr_pim_tab_occ(){_pim_tab_occupancy++;}
    void dcr_pim_tab_occ(){_pim_tab_occupancy--;}
    void incr_num_pim_entries(){_num_pim_entries++;}
    void dcr_num_pim_entries(){_num_pim_entries--;}
    int return_vc_r_s_size(int vnet, int port);
    
    void build_pim_table(int size);
    void set_id(int setid){this->id = setid;}
    void reset_sending_pkts(){sending_pkts_in_current_cyc = 0;}
    void incr_sending_pkts(){sending_pkts_in_current_cyc++;}
    int return_sending_pkts(){return sending_pkts_in_current_cyc;}
    int return_mem_cube_size(){return mem_cube.size();}
    packet * return_pkt_mem_queue(int entry){return mem_cube[entry];}
    void erase_mem_cube_entry(int i){mem_cube.erase(mem_cube.begin()+i);}
    bool return_pim_entry_tab_status(int i){return pim_entry_tab[i].valid;}
    void set_pim_tab_entry(bool status, int index){pim_entry_tab[index].valid=status;}
    void set_pim_entry_time(unsigned long time, int index){pim_entry_tab[index].entry_time=time;}
    void set_pim_entry_src1_status(bool status, int index){pim_entry_tab[index].src1_ready=status;}
    void set_pim_entry_src2_status(bool status, int index){pim_entry_tab[index].src2_ready=status;}
    void set_pim_entry_src1_pktid(unsigned long id, int index){pim_entry_tab[index].src1_pkt_id=id;}
    void set_pim_entry_src2_pktid(unsigned long id, int index){pim_entry_tab[index].src2_pkt_id=id;}
    void set_pim_src1_reach_time(unsigned long time, int index){pim_entry_tab[index].src1_reach_time = time;}
    void set_pim_src2_reach_time(unsigned long time, int index){pim_entry_tab[index].src2_reach_time = time;}
    void set_pim_entry_pkt(packet *pkt, int index){pim_entry_tab[index].pkt = pkt;}
    unsigned long return_src1_pkt_id(int index){return pim_entry_tab[index].src1_pkt_id;}
    unsigned long return_src2_pkt_id(int index){return pim_entry_tab[index].src2_pkt_id;}
    unsigned long return_pim_entry_time(int index){return pim_entry_tab[index].entry_time;}
    bool return_pim_entry_src1_status(int index){return pim_entry_tab[index].src1_ready;}
    bool return_pim_entry_src2_status(int index){return pim_entry_tab[index].src2_ready;}
    packet * get_pim_entry_pkt(int index){return pim_entry_tab[index].pkt;}
    unsigned long return_pim_src1_reach_time(int index){return pim_entry_tab[index].src1_reach_time;}
    unsigned long return_pim_src2_reach_time(int index){return pim_entry_tab[index].src2_reach_time;}
    long return_pim_entry_pkt_id(int index){return pim_entry_tab[index].pkt->id;}
    void make_entry_pim_req_queue(packet *pkt){pim_req_queue.push_back(pkt);}
    
    void sw_arb();
    int rr_vnet_impl();
    int rr_port_impl();

    bool link_busy(int);//only check if the link is busy
    void make_link_busy(int);
    void clear_link(){_link.clear();}

    //debug purposes
    void record_retry_per_vnet_per_port(int v, int p){_retry_per_vnet_per_port[v][p]++;}//only on consecutive retries
    void clear_retry_per_vnet_per_port();//it will also make the size zero, so resize again
    unsigned long return_retry_per_vnet_per_port(int v, int p){
      return _retry_per_vnet_per_port[v][p];
    }
    
    unsigned long return_front_pim_tab_packets_id(){return pim_req_queue.front()->id;}
    unsigned long return_front_resp_packets_id(){return resp_queue.front()->id;}
    
    int num_pkt(int v, int p);
    pair<int, int> return_pkt_src_dest(int v, int p, int i);

    bool make_entry_forward_queue(packet *);//part of handling computation migration 
    void send_forward_pkt();
    bool has_forward(){return _results_forward_queue.size()>0;}
    
    void check_pim_op_roundtrip(unsigned long id, unsigned long cyc, bool req);
    void eject_rcvd_pkts(vector<unsigned long>&);


    bool count_and_return_pg_hist(unsigned long pg);
    void create_and_enqueue_info_pkt(unsigned long pg, int pid);
    void send_info_pkt();
    bool has_info_pkt(){return _info_pkt_queue.size()>0;}

  private:
    vector<deque<packet *> >vc_r_s;//request queue for forked pim op sources, to new pim tab entry
    vector<deque<packet *> >vc_r_s_pimop;//request queue for pim op that needs to create entry in pim tab
    vector<deque<packet *> >vc_r_s_resp;//for all the response packets
    vector<deque<packet *> >vc_r_s_mig;//for all the migration packets (no response required)
    vector<deque<packet *> >vc_r_s_forward;//for all the migration packets (no response required)
    
    vector<int>req_q; 
    vector<int>req_pimop_q; 
    vector<int>resp_q; 
    vector<int>mig_q; 
    vector<int>forward_q; 
    
    int _router_radix;
    int _num_vnet;
    int _max_entry_vc_r_s;
    int max_entry_mem_cube;
    int max_entry_resp_q;
    int num_vaults;
    int num_banks_per_vault;
    int mem_row_hit_lat;
    int mem_row_miss_lat;
    int id;  
    int x_dim;
    int y_dim;
    int sending_pkts_in_current_cyc;
    int _num_pim_entries;
    int _pim_tab_size;

    unsigned long pim_tab_index;
    unsigned long _pim_tab_occupancy;//keeps track of both reservations and actual 
    
    int _rr_vnet;
    int _rr_port;
    int _num_count_port;
    int _num_count_vnet;
    int _rr_set_port;
    int _rr_set_vnet;

    int _info_pkt_queue_len;

    vector<packet *> mem_cube;
    deque<packet *> resp_queue;
    deque<packet *> _results_forward_queue;
    vector<vector<unsigned long> > row_buff;//records the open page numbers 
    pim_entry *pim_entry_tab;//keeps track of pim entries 
    deque<packet *> pim_req_queue;
    map<int,int>_link;
    vector<vector<unsigned long> > _retry_per_vnet_per_port;//for debugging only 
    
    //<packet id, <request inj time, resp rcv time> >
    unordered_map<unsigned long, pair<unsigned long, unsigned long> >_mem_nw_roundtrip;//entry del on rcv resp
    //<packet id, <request inj time, resp rcv time> >
    unordered_map<unsigned long, pair<unsigned long, unsigned long> >_pimop_response_rcvd;//entry created on rcv resp
    
    unordered_map<unsigned long, unsigned long> _info_monitor;//pagenumber, #retired access
    deque<packet *> _info_pkt_queue;
    vector<int> _mc_ports;//holds the ports
};

class MemoryNetwork{
  public:
    void operator=(MemoryNetwork const&)=delete;

    static MemoryNetwork * GetInstance(const Configuration &config){
      if(_mnet == NULL){
        _mnet = new MemoryNetwork(config);
      }
      return _mnet;  
    }

  private:
    
    ~MemoryNetwork();
    MemoryNetwork(const Configuration &);
    
    static MemoryNetwork *_mnet;

    int num_nodes;//total number of nodes
    double size_per_node;//in GBs
    int *mem_ports;
    int x_dim;
    int y_dim;
    unsigned long _tot_frame_per_cube;
    unsigned long _frame_size;
    bool _run_baseline;
    int _pim_tab_size;
    unsigned long _diff_retry_cyc;
    int _consecutive_possible_deadlock;

    vector<node>node_list;//set of nodes
    //vector<vector<int> >adj; //made it global    

  public:
    
    int _router_radix_mnet;
    int _num_vnet_mnet;

    void create_network(const Configuration& config);
    void read_topology(string topology);
    int addr_to_nodeNum_conv(unsigned long long);//need to divide equally among nodes
    int mesh_neighbor_closer_to_dest(int current, int dest);
    void step();
    void evaluate(int);
    void end_of_epoch_reset();
    void handle_regular_pkts_at_dest(int node, int port, packet *pkt);
    void retire(int);//once the memory access is completed
    bool can_inject(int node, packet *pkt, int NI_id);
    void inject(packet *i, int NI_id);
    bool has_response(int node){return (node_list[node].return_resp_queue_size()>0);}

    void handle_pim_inst(int node, int port, packet * pkt);
    packet * create_src_pkt(packet *, int, int, int);
    
    void received_responses(vector<unsigned long> &);

    /*
     * Stats and debug
     */
    void print_stats(string);
    void debug_evaluate(int node, int vnet_it, int vnet, int port_it, int port);

};
