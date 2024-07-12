#include "mmu.hpp"

MemoryManagementUnit * MemoryManagementUnit::_mmu = NULL;

MemoryManagementUnit::MemoryManagementUnit(const Configuration& config):_config(config){
  _mem_size = config.GetInt("mem_size");
  _page_size = config.GetInt("page_size");
  _x_dim = config.GetInt("x_dim");
  _y_dim = config.GetInt("y_dim");
  _num_nodes = _x_dim * _y_dim;
  _size_per_node = _mem_size / (double)(_num_nodes);
  _num_pkt_per_page = (_page_size * 1024)/64;//64 byte packet size 
  _dis_migration = (config.GetInt("dis_migr")>=1);
  _migr_q_size = config.GetInt("migr_q_size");
}

MemoryManagementUnit::~MemoryManagementUnit(){}


bool MemoryManagementUnit::step(){
  bool is_done = false;
  #pragma omp parallel for num_threads(8) collapse(2)
  // cout << "num_proc_submission: " << num_proc_submission << endl;
  assert(num_proc_submission == 1);
  for(int proc=0; proc < num_proc_submission; proc++){
    if(_vmu_map[proc]->is_done()){
      is_done = true;
      cout<<"[MMU] set done: "<<is_done<<endl;
    }
    if(!_dis_migration && /*sleep_process[proc]==1 &&*/ _mgc[proc].size()){
      MigrationCandidate * l_mg = _mgc[proc].front();
      if(l_mg!=nullptr && l_mg->return_vaddr()>0){
        _v2p_map[proc]->DoPageMigration(l_mg->return_vaddr(), l_mg->return_frame());
        _mgc[proc].pop_front();
        stats_total_migr_q_size--;
        delete(l_mg);
      }
    }  
  }
  return is_done;
}

void MemoryManagementUnit::set_up(){
  //_mgc.resize(num_proc_submission);
  for(int i=0; i<num_proc_submission; i++){
    _mgc.push_back(deque<MigrationCandidate *>());
  }
  cout<<"[MMU] Migration Queue size: "<<_mgc.size()<<" process"<<endl;
}

v_map_use * MemoryManagementUnit::get_vmu(int pid){
  auto iter = _vmu_map.find(pid);
  if(iter!=_vmu_map.end()){
    return iter->second;
  }
  else{
    v_map_use * new_vmu = new v_map_use(_config);
    _vmu_map[pid] = new_vmu;
    return new_vmu; 
  } 
}

vir2phy * MemoryManagementUnit::get_v2p(int pid){
  auto iter = _v2p_map.find(pid);
  if(iter!=_v2p_map.end()){
    return iter->second;
  }
  else{
    vir2phy * new_v2p = new vir2phy();
    _v2p_map[pid] = new_v2p;
    return new_v2p; 
  } 
}

void MemoryManagementUnit::add_mig_cand(int pid, unsigned long page, unsigned long new_cube){
  bool found = false;
  MigrationCandidate * m = new MigrationCandidate(pid,page,new_cube);
  #pragma omp parallel for num_threads(8) collapse(2)
  for(int i = 0; i < _mgc[pid].size(); i++){
    if(_mgc[pid][i]->return_vaddr()==page){
      _mgc[pid][i]->update_cube(new_cube);
      found = true;
      break; 
    }
  }
  if(!found){
    _mgc[pid].push_back(m);
    stats_total_migr_q_size++;
    //cout<<"[MMU] migration queue size: "<<stats_total_migr_q_size<<endl;
    if(_mgc[pid].size() > _migr_q_size){ //FIFO implementation 
      _mgc[pid].pop_front();
      cout<<"[MMU] migration queue size exceeded: "<<stats_total_migr_q_size<<endl;
      stats_total_migr_q_size--;
    }
    //cout<<"adding migration candidates ..."<<endl;
  }
}

void MemoryManagementUnit::create_input_n2v(int pid){
  v_map_use *l_vmap = get_vmu(pid);
  l_vmap->create_input_n2v();
}
