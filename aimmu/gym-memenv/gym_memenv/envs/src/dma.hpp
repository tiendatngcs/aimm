#ifndef _DMA_H_
#define _DMA_H_

#include "config_utils.hpp"
#include "packet.hpp"
#include "mc.hpp"
#include "extern_vars.hpp"

class MigrCand{
  public:
    MigrCand();
    MigrCand(int s, int d, unsigned long pg, unsigned long migr_count):
      _s(s), _d(d), _pg(pg), _migr_count(migr_count){}
    
    int get_src(){return _s;}
    int get_dest(){return _d;}
    unsigned long get_pg_num(){return _pg;}
    unsigned long get_migr_count(){return _migr_count;}
  
  private:
    int _s;
    int _d;
    unsigned long _pg;
    unsigned long _migr_count;
};


class DMAPort{
  public:
    
    enum{
      idle,
      busy
    };
    
    DMAPort(){
      _pid = 0;
      _state = idle;
      _cand_src = -1;
      _cand_dest = -1;
      _cand_pg = 0;
      _remain_pkt = -1;
    }
    
    void set_state(int st){_state=(st==0)?idle:busy;}
    int get_state(){return _state;}
    void decr(){_remain_pkt--;}
    void set_pkt_details(unsigned long pid, int s, long long d, unsigned long pg, int size){ 
      assert(pid >= 0 && pid <= num_proc_submission);
      _pid = pid;
      _cand_src = s;
      _cand_dest = d;
      _cand_pg = pg;
      _remain_pkt = size;
      _max_num_pkt = size;
    }
    packet * get_packet();
    bool packet_remained(){return _remain_pkt>0;}
    int return_num_pkt_remained(){return _remain_pkt;}
    bool first_packet(){return _remain_pkt == _max_num_pkt;}
    bool last_packet(){return _remain_pkt == 1;}

  private:
    unsigned long _pid;
    int _state;
    int _cand_src;
    int _max_num_pkt;
    long long _cand_dest;
    unsigned long _cand_pg;
    int _remain_pkt;//number of packets need to be inserted
};

class DirectMemoryAccess{
  public:
    void operator=(DirectMemoryAccess const&)=delete;
    
    static DirectMemoryAccess * GetInstance(){
      if(_dma == NULL){
        _dma = new DirectMemoryAccess();
      }
      return _dma;
    }
  
    void create_ports(int num_ports){ports.resize(num_ports);}
    void make_entry(int pid, int src, int dest, unsigned long pg, unsigned long tot_migr);//making entry for migration candidate
    bool send_packet(packet *);//if failed delete the packet
    void remove_pkt_entry(int port_num){ports[port_num].decr();}
    void set_up(const Configuration &);
    void step(); 
    int get_port_status(int port){return ports[port].get_state();}

  private: 
    DirectMemoryAccess();
    ~DirectMemoryAccess();

    int _mem_size;
    int _page_size;
    int _x_dim;
    int _y_dim;
    int _num_nodes;
    int _num_pkt_per_page;
    double _size_per_node;
    bool _simulate_migr;

    static DirectMemoryAccess * _dma;
    vector<DMAPort>ports;
    vector<vector<MigrCand *> >_MgCand;
    MemoryNetwork *_mem_net; 
};

#endif
