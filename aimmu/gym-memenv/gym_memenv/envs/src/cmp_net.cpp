#include "cmp_net.hpp"

cmp_net * cmp_net::cn = NULL;

cmp_net::cmp_net(){
}

cmp_net::~cmp_net(){
}

void cmp_net::setup_cmp(const Configuration& config){
  _max_cmp_queue_size = config.GetInt("max_cmpq_size");   
  _num_ports = config.GetInt("num_port");   
  _num_cmp_tiles = config.GetInt("num_cmp_tiles");   
  _pei_enabled = config.GetInt("pei_enabled")>0;

  _cmp_net_q.resize(_num_cmp_tiles);
  _ctag = new cache(config);//unfied addr, physically distributed banked single cache tag obj  
  _mshr = new mshr();
  _arbit = 0;

  for(int j=0; j<_num_cmp_tiles; j++){
    for(int i=0;i<_num_ports;i++){
      deque<packet *>n_dq;
      _cmp_net_q[j].push_back(n_dq);
      cout<<"cmp_net deques for core: "<<j<<" port: "<<i<<endl;
    }
  }
}

void cmp_net::step(){
  if(_pei_enabled==true){
    _handle_pei_resp();
  }
}

void cmp_net::_handle_pei_resp(){
  for(int i=0; i<_ejectiion_q.size(); i++){
    unsigned long key = _ejectiion_q[i];
    unsigned long blk_adddr = _mshr->get_block_addr(key);
    _mshr->transfer_pkts_to_ready_queue(blk_adddr, _pei_ready_q);//in each step it will try to send to mcq
    _mshr->clear_entry(key, blk_adddr); 
    _ctag->access(blk_adddr);
  }
}

packet * cmp_net::_handle_pei_req(int pid, int port){
  //cout<<"[PEI] size of cmp_q: "<<_cmp_net_q[pid][port].size()<<endl;
  if(_cmp_net_q[pid][port].size()>0){
    for(int i=0; i<_cmp_net_q[pid][port].size(); i++){
      //cout<<"[PEI] going to access: index: "<<i<<endl;
      packet *pkt = _cmp_net_q[pid][port][i];
      //cout<<"[PEI] got packet: "<<pkt->id<<endl;
      bool hit = _ctag->access(pkt->pim_src1_pa>>6);
      //cout<<"[PEI] accessed the cache ... hit? "<<hit<<endl;
      if(hit){
        _cmp_net_q[pid][port].erase(_cmp_net_q[pid][port].begin()+i);
        cout << "Dequeue cmp net q" << endl;
        return pkt;
      }
      else{//miss in cache
        bool mshr_hit = _mshr->access(pkt->pim_src1_pa>>6);
        if(mshr_hit){
          _mshr->make_entry(pkt);//adding more into existing entry
        }
        else{//miss in mshr
          if(_mshr->can_make_entry()){
            _mshr->make_entry(pkt);
            _make_entry_read_request_queue(pkt);//this will be checked in each step
            _cmp_net_q[pid][port].erase(_cmp_net_q[pid][port].begin()+i);
            cout << "Dequeue cmp net q" << endl;
          }
          else{
            //try again later
          }
        }
        return NULL;
      }
    }
  }
  else{
    //cout<<"[PEI] cmp q is empty ... "<<endl;
  }
  return NULL;
}

void cmp_net::_make_entry_read_request_queue(packet *pkt){
    packet *l_pkt = new packet;
    l_pkt->pid = pkt->pid;//recording process id
    l_pkt->opid = pkt->opid;//recording operation id
    l_pkt->c_time = global_clock;
    l_pkt->src = pkt->src;
    l_pkt->dest = pkt->dest;
    l_pkt->needs_resp = true;
    l_pkt->pei_request = true;

    _pei_read_req_q.push_back(l_pkt);  
}


void cmp_net::fill_ejection_queue(vector<unsigned long>&eq){ //the ejection queue will be check in each cycle
  _ejectiion_q.assign(eq.begin(), eq.end());//deep copy
}


bool cmp_net::can_make_entry(int pid, int port){//process is pinned to a core
  if(_cmp_net_q[pid][port].size() < _max_cmp_queue_size){
    return true;
  }
  else{
    // cout << "_max_cmp_queue_size " << _max_cmp_queue_size << endl;
    // cout << "Can't make entry: port" << port << " cmp_net_queue " << _cmp_net_q[pid][port].size() << endl;
    // assert(false);
    // exit(0);
    return false;
  }
}

bool cmp_net::make_entry(int pid, int port, packet *pkt){
  
  if(_cmp_net_q[pid][port].size() < _max_cmp_queue_size){
    if(_pei_enabled){
      //pkt->forward_dest = pkt->dest;
      pkt->dest = pkt->pim_src1; 
      //pkt->pim_src1 = -1;
    }
    _cmp_net_q[pid][port].push_back(pkt);
    return true;
  }
  else{
    assert(0 && "the size of cmp_net should have been checked before making entry ... ");
    return false;
  }
}//vmap is going to make entry

packet * cmp_net::read_entry(int pid, int port){//TODO: do we need arbitration here? 
  if(_pei_enabled==true){
    if(++_arbit%3==0){ //RR scheduling ... higher priority on read requests 
      //cout<<"[PEI] calling handle_pei_req ... "<<endl;
      packet *pkt = _handle_pei_req(pid, port);//gets packet on cache hit 
      if(pkt==NULL){
        //cout<<"[PEI] going to check ready quaue as pkt is null "<<endl;
        if(_pei_ready_q.size()>0){//after response
          //cout<<"[PEI] pei ready q not empty ..."<<endl;
          pkt = _pei_ready_q.front();
          _pei_ready_q.pop_front();
        }
        else{
          //cout<<"[PEI] ready q is empty ..."<<endl;
        }
      }
      return pkt;
    }
    else{
      if(_pei_read_req_q.size()>0){
        packet *pkt = _pei_read_req_q.front();
        _pei_read_req_q.pop_front();
        return pkt;
      }
      else{
        return NULL;
      }
    }
  }
  else{
     if(_cmp_net_q[pid][port].size()>0){
       packet* l_pkt = _cmp_net_q[pid][port].front();
       assert(l_pkt!=NULL && "A valid packet can not be NULL");
       _cmp_net_q[pid][port].pop_front();
      //  cout << "Poping _cmp_net_q current size: " << _cmp_net_q[pid][port].size() << endl;
       return l_pkt;
     }
     else{
       return NULL;
     }
  }
}//mc is going to read



cache::cache(const Configuration &config){
  _num_cache_entries = config.GetInt("l2_cache_size");//in terms of number of entries
  _repl_policy = config.GetStr("l2_repl_policy"); 
}


cache::~cache(){
}

void cache::_update_cache(unsigned long blk_addr){
  if(_repl_policy.compare("LRU")==0){//returns ZERO "0" on match
    unsigned long lru_status = _tag[blk_addr];
    _tag[blk_addr]=0;//least recently used (LRU)
    for(auto it=_tag.begin(); it!=_tag.end(); ++it){
      if(it->second < lru_status){
        //logically shifting the status of all the other pages that were higher status than the current one
        it->second++;
        if(it->second == _num_cache_entries){
          _next_victim = it->first;
        }
      } 
    }
  }
  else if(_repl_policy.compare("FIFO")==0){//returns ZERO "0" on match
    //nothing to do here
  }
  else if(_repl_policy.compare("NRU")==0){//returns ZERO "0" on match
    assert(0 && "*** implementation has issue [not use it]***");
    _tag[blk_addr]=0;//recently used 
    //randomly select the NRU candidate
    int tag_size = _ordered_q.size();
    //assert(tag_size <= _num_cache_entries);
    int next_victim_index = rand()% tag_size;
    //cout<<"next victim index: "<<next_victim_index<<" _ordered_q size: "<<_ordered_q.size()<<endl; 
    _next_victim = _ordered_q[next_victim_index];
    if(_next_victim == blk_addr){ //it is higly unlikely that random will pick the same index twice
      int next_victim_index = rand()% tag_size; 
      _next_victim = _ordered_q[next_victim_index];
    }
    if(_ordered_q.size() > _num_cache_entries){
      _ordered_q.erase(_ordered_q.begin()+next_victim_index); 
      _tag.erase(_next_victim);
    } 
  }
  else{
    cout<<"[CMP_NET] *** Wrong L2 repl policy ***"<<endl;
    assert(0 && "Choose the repl polic from the list available");
  }
}

void cache::_victimize(){
  if(_repl_policy.compare("LRU")==0){//returns ZERO "0" on match
    _tag.erase(_next_victim);
  }
  else if(_repl_policy.compare("FIFO")==0){//returns ZERO "0" on match
    _next_victim = _ordered_q.front();
    _tag.erase(_next_victim);
    _ordered_q.pop_front();
  }
  else if(_repl_policy.compare("NRU")==0){//returns ZERO "0" on match
    //_tag.erase(_next_victim);
  }
  else{
    cout<<"[CMP_NET] *** Wrong L2 repl policy ***"<<endl;
    assert(0 && "Choose the repl polic from the list available");
  }

}

void cache::_make_entry(unsigned long blk_addr){
  _tag[blk_addr] = _tag.size();
  
  if(_repl_policy.compare("FIFO")==0 || _repl_policy.compare("NRU")==0){//returns ZERO "0" on match
    _ordered_q.push_back(blk_addr);//maintaining FIFO order
  }
  stats_l2_cache_occupancy = _tag.size();
}

bool cache::access(unsigned long blk_addr){
  auto it = _tag.find(blk_addr);//cache block size is 64B
  bool hit = false;
  stats_l2_cache_access++;
  //hit
  if(it!=_tag.end()){
    _update_cache(blk_addr);//cache block size is 64B
    hit = true;
    stats_l2_cache_hits++;
  }
  else{
    if(_tag.size() >= _num_cache_entries){
      _victimize();//does need to return anything
    }
    _make_entry(blk_addr);//cache block 64B
  }
  return hit; 
}

void mshr::make_entry(packet *pkt){
  unsigned long blk_addr = pkt->pim_src1_pa>>6;
  auto it = _mshr_tab.find(blk_addr);
  _mshr_tab[blk_addr].push_back(pkt);
  if(it==_mshr_tab.end()){
    _opid_addr[pkt->opid] = blk_addr;
  }  
}

void mshr::set_up(const Configuration &config){
  _max_entry = config.GetInt("max_mshr_entry");
}
