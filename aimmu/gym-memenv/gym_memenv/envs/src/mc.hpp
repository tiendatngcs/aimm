#ifndef _MC_H_
#define _MC_H_

#include <iostream>
#include "cmp_net.hpp"
#include "m_net/m_net.hpp"
#include "v2p/pfa.hpp"

class PageInfoCache{
  public:
    PageInfoCache(const Configuration &config);
    ~PageInfoCache();

    bool access(int, unsigned long);//returns hit or miss
  
  private:
    int _num_cahe_entries;
    unsigned long _next_victim;//pointing to the next victim while updating status
    void _update_cahe(unsigned long);
    void _victimize(int pid);
    void _make_entry(unsigned long);
};

class MemoryController{
  public:
    void operator=(MemoryController const&)=delete;
    
    static MemoryController * GetInstance(const Configuration & config){
      if(_mc == NULL){
        _mc = new MemoryController(config);
      }
      return _mc;
    }

    void step();
    void set_up(const Configuration &config);
    void make_entry_mcq(int port, packet *pkt);
    bool is_mcq_full(int port);
    int get_port(unsigned long);
    void create_stats_vars();
    void print_stats(string);
    void create_mem(const Configuration &);
    void feed_page_info_cache(int port, packet *pkt); 

  private:
    MemoryController(const Configuration &);
    ~MemoryController(){};
    static MemoryController *_mc;
    const int EPOCH = 5000000;
    int _num_port;
    int _max_mcq_size;
    bool _disable_training;
    bool _periodic_break;
    bool _pei_enabled;
    vector<int> _port;//holds the ports
    vector<vector<packet *> > _mcq;
    vector<PageInfoCache *>_pg_info_cache;
    vector<unsigned long>_rcv_resp_queue;//try to get responses at each step

    MemoryNetwork * _m_net;      //memory network
};

#endif
