#ifndef _CMP_NET_H_
#define _CMP_NET_H_

#include <iostream>
#include <vector>
#include <deque>
#include "packet.hpp"
#include <assert.h>
#include "config_utils.hpp"

using namespace std;


class mshr{
  private:
    int _max_entry;
    unordered_map<unsigned long, vector<packet *> > _mshr_tab;
    unordered_map<unsigned long, unsigned long> _opid_addr;

  public:
    mshr(){}
    ~mshr(){}
    void set_up(const Configuration &config);
    void make_entry(packet *);
    unsigned long get_block_addr(unsigned long opid){return _opid_addr[opid];}
    void transfer_pkts_to_ready_queue(unsigned long blk_addr, deque<packet *> &ready_q){
      ready_q.assign(_mshr_tab[blk_addr].begin(), _mshr_tab[blk_addr].end());
    }
    bool access(unsigned long blk_addr){
      auto it = _mshr_tab.find(blk_addr);
      if(it!=_mshr_tab.end()){
        return true;
      }
      else{
        return false;
      }
    }
    bool can_make_entry(){return _mshr_tab.size() < _max_entry;}

    void clear_entry(unsigned long opid, unsigned long blk_addr){
      _mshr_tab.erase(blk_addr);
      _opid_addr.erase(opid);
    }
};


class cache{
  private:
    int _num_cache_entries;//signifies the size of the cache, each entry being 64B
    unsigned long _next_victim;
    string _repl_policy;
    unordered_map<unsigned long, unsigned long> _tag;//for accessing
    deque<unsigned long> _ordered_q;//for maintaining order for repl

    void _update_cache(unsigned long);
    void _victimize();
    void _make_entry(unsigned long);

  public:
    cache(const Configuration &config);//unified banked cache, so one is enough
    ~cache();
    bool access(unsigned long);
};


class cmp_net{
  private:
    cmp_net();
    ~cmp_net();

    static cmp_net *cn;
    cache *_ctag;//cache tags
    mshr * _mshr;
    
    int _num_ports;//get it from config 
    int _max_cmp_queue_size; 
    int _num_cmp_tiles;
    bool _pei_enabled;    
    unsigned long _arbit;

    vector<vector<deque<packet *> > >_cmp_net_q;//one channel for each process 
    vector<unsigned long>_ejectiion_q; 
    deque<packet *> _pei_ready_q;//packets from ready queue will be sent to mcq
    deque<packet *> _pei_read_req_q;

    void _make_entry_read_request_queue(packet *pkt);//this will be checked in each step

  public:
    cmp_net(cmp_net const&)=delete;
    void operator=(cmp_net const&)=delete;
    static cmp_net * GetInstance(){
      if(cn == NULL)
        cn = new cmp_net;
      return cn;  
    }

    void step();
    void setup_cmp(const Configuration& config);
    bool can_make_entry(int pid, int port);
    bool make_entry(int pid, int port, packet *pkt);
    packet * read_entry(int pid, int port);
    packet * _handle_pei_req(int pid, int port);
    void _handle_pei_resp();
    void fill_ejection_queue(vector<unsigned long>&eq);
};

#endif
