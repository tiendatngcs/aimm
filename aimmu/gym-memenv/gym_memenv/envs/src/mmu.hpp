#ifndef _MMU_H_
#define _MMU_H_

#include "vmap.hpp"
#include "v2p/v2p.hpp"
#include "config_utils.hpp"

class MigrationCandidate{
  public:
    MigrationCandidate(int pid, unsigned long va, unsigned long nf):
      _pid(pid),_vaddr(va),_new_frame(nf){}

    ~MigrationCandidate(){}

    int return_pid(){return _pid;}
    unsigned long return_vaddr(){return _vaddr;}
    unsigned long return_frame(){return _new_frame;}
    
    void update_cube(int nc){_new_frame = nc;}
  private:
    int _pid;
    unsigned long _vaddr;
    unsigned long _new_frame;
};

class MemoryManagementUnit{
  public:
    void operator=(MemoryManagementUnit const&)=delete;
    
    static MemoryManagementUnit * GetInstance(const Configuration& config){
      if(_mmu == NULL){
        _mmu = new MemoryManagementUnit(config);
      }
      return _mmu;
    }
    
    bool step();    
    void set_up(); 
    v_map_use * get_vmu(int);
    vir2phy * get_v2p(int);
    void add_mig_cand(int pid, unsigned long va, unsigned long nf);
    void clear_mig_queue(int pid){_mgc[pid].clear();}
    void create_input_n2v(int pid);

  private: 
    MemoryManagementUnit(const Configuration &);
    ~MemoryManagementUnit();

    int _mem_size;
    int _page_size;
    int _x_dim;
    int _y_dim;
    int _num_nodes;
    int _num_pkt_per_page;
    double _size_per_node;
    bool _dis_migration;   
    int _migr_q_size; 

    static MemoryManagementUnit * _mmu;
    DirectMemoryAccess * _dma;
    map<int, v_map_use *>_vmu_map;
    map<int, vir2phy *>_v2p_map;
    vector<deque<MigrationCandidate *> >_mgc;
    Configuration _config;
};

#endif
