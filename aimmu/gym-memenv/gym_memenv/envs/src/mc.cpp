#include "mc.hpp"

unsigned long gPID = 0;
unsigned long gPORT = 0;
unsigned long gPORT_SEND = 0;
unsigned long gNI_SEND = 0;
unsigned long gPORT_RCV = 0;
unsigned long gNI_RCV = 0;


PageInfoCache::PageInfoCache(const Configuration &config){
  _num_cahe_entries = config.GetInt("page_info_cache_size");//in terms of number of entries
}


PageInfoCache::~PageInfoCache(){
}

void PageInfoCache::_update_cahe(unsigned long pg){
  unsigned long lru_status = page_info_cache[pg];
  page_info_cache[pg]=0;//least recently used (LRU)
  for(auto it=page_info_cache.begin(); it!=page_info_cache.end(); ++it){
    if(it->second < lru_status){
      //logically shifting the status of all the other pages that were higher status than the current one
      it->second++;
      if(it->second == _num_cahe_entries){
        _next_victim = it->first;
      }
    } 
  }
}

void PageInfoCache::_victimize(int pid){
  page_info_cache.erase(_next_victim);
  auto it = page_info_map[pid].find(_next_victim);
  if(it!=page_info_map[pid].end()){
    page_info_map[pid][_next_victim]->clear_info(_next_victim); 
  }
}

void PageInfoCache::_make_entry(unsigned long pg){
  page_info_cache[pg] = page_info_cache.size();
}

bool PageInfoCache::access(int pid, unsigned long addr){
  auto it = page_info_cache.find(addr>>11);
  bool hit = false;
  //hit
  if(it!=page_info_cache.end()){
    _update_cahe(addr>>11);
    hit = true;
  }
  else{
    if(page_info_cache.size() >= _num_cahe_entries){
      _victimize(pid);//does need to return anything
    }
    _make_entry(addr>>11);
  }
  return hit; 
}


MemoryController * MemoryController::_mc = NULL;

MemoryController::MemoryController(const Configuration& config){
}

void MemoryController::set_up(const Configuration &config){
  _max_mcq_size = config.GetInt("max_mcq_size");
  _num_port = config.GetInt("num_port");
  _port = config.GetIntArray("port_positions");
  _disable_training = (config.GetInt("dis_training")>=1);
  _periodic_break = (config.GetInt("periodic_break")>=1);  
  _pei_enabled = config.GetInt("pei_enabled")>0;
  _mcq.resize(_num_port);
  create_mem(config);
  create_stats_vars();
  
  _m_net = MemoryNetwork::GetInstance(config);

  _pg_info_cache.resize(_num_port);
  for(int i=0; i< _num_port; i++){
    _pg_info_cache[i] = new PageInfoCache(config);
  }  
}

void MemoryController::create_mem(const Configuration& config){
  int mem_size = config.GetInt("mem_size");
  int page_frame_size = config.GetInt("page_size");
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"Creating PageFrames for Memory of Size: %d\n",mem_size);  
#endif
  PageFrameAllocator * MM = PageFrameAllocator::GetInstance();
  MM->create_page_frames(config);
}

void MemoryController::make_entry_mcq(int port, packet *pkt)
{
  _mcq[port].push_back(pkt);
}

bool MemoryController::is_mcq_full(int port)
{
  return !(_mcq[port].size() < _max_mcq_size);
}


void MemoryController::feed_page_info_cache(int port, packet *pkt){
  _pg_info_cache[port]->access(pkt->pid, pkt->dest_va); 
  _pg_info_cache[port]->access(pkt->pid, pkt->src1_va); 
  _pg_info_cache[port]->access(pkt->pid, pkt->src2_va); 
}

void MemoryController::step()
{
  unsigned long tot_inst_committed = stats_retired_mem_acc + stats_total_non_memory_inst; //TODO have bring the idea of instr commit also
  cmp_net *l_cmp_net = cmp_net::GetInstance();

  long long l_dest = -1;
  long long l_pim_src1 = -1;
  long long l_pim_src2 = -1;
 
  unsigned long l_dest_pa = 0; 
  unsigned long l_pim_src1_pa = 0;
  unsigned long l_pim_src2_pa = 0;
 
  for(int p=0; p< num_proc_submission; p++){ 
    int count = _num_port;
    gPID = gPID % num_proc_submission;

    for(int i = 0; i < _num_port; i++) {
    
      gPORT = gPORT % _num_port;
      
      if(!is_mcq_full(gPORT)){
        packet *l_pkt = l_cmp_net->read_entry(gPID, gPORT);
        
        gPORT = (gPORT+1) % _num_port;
        count--;
        if(count==0){
          gPID = (gPID+1) % num_proc_submission;
        }
        
        if(l_pkt!=NULL){
          
          //l_dest = l_pkt->dest;
          //l_pim_src1  = l_pkt->pim_src1;
          //l_pim_src2  = l_pkt->pim_src2;
          //assert(l_pkt->dest_pa!=0 && l_pkt->pim_src1_pa!=0);
          //assert(l_dest!=0 && l_pim_src1!=0);
          //int port = get_port(l_dest);
          //l_pkt->src = _port[port];
          l_pkt->src = _port[gPORT];
          //l_pkt->dest = _m_net->addr_to_nodeNum_conv(l_dest);
          //if(l_pkt->pim_src1!=-1){
          //  l_pkt->pim_src1 = _m_net->addr_to_nodeNum_conv(l_pim_src1);
          //}
          //if(l_pkt->pim_src2!=-1){//it will be zero for 2-operand pim op, set in vmap for trace_pim
          //  l_pkt->pim_src2 = _m_net->addr_to_nodeNum_conv(l_pim_src2);
          //}
          make_entry_mcq(gPORT,l_pkt);
          ////////////////////////////////////// ONLY OP BREAK DOWN STATS /////////////////////////////////
          if(!multi_program && !l_pkt->pei_request){
            assert(l_pkt->opid!=0);
            unsigned long opid = l_pkt->opid;
            unsigned long id = l_pkt->id;
            auto it = op_lat_breakdown_tracker[l_pkt->pid].find(opid);
            if(it==op_lat_breakdown_tracker[l_pkt->pid].end()){
              cout<<"[MC-1] Entry not found for pkt: "<<id<<" opid: "<<opid<<endl;
              assert("0" && "*** Entry not found for OPID while retiring packet ***");
            }
            //cout<<"[MC-1] Push back for OP: "<<opid<<" global clock: "<<global_clock<<endl;
            op_lat_breakdown_tracker[l_pkt->pid][opid].push_back(global_clock);//recording m_net entry making time
          }
          ////////////////////////////////////// ONLY OP BREAK DOWN TATS /////////////////////////////////
          #ifdef DEBUG_PRINT_MNET
          fprintf(debugfile, "%lu | [node:%d] enquing packet %lu at port %d [src:%d|dest:%lld]\n"
              ,global_clock,l_pkt->src,l_pkt->id,5,l_pkt->src,l_pkt->dest);
          #endif
          //cout<<"make entry in mcq for port: "<<i<<endl;
        }
        else{
          //means packet is not there !!!!
        }
      }
      else{
        // cout<<"mcq full @cycle: "<<global_clock<<endl;
      }
    }
  }
  
 //received responses
 _m_net->received_responses(_rcv_resp_queue);//passing the vector
 if(_pei_enabled){
  l_cmp_net->fill_ejection_queue(_rcv_resp_queue);//sharing the opid for which the response is received
  _rcv_resp_queue.clear();
 }
 //send requests 
 for(int i = 0; i < _num_port; i++) {

    gPORT_SEND = (gPORT_SEND + 1) % _num_port;

    if (_mcq[gPORT_SEND].size() > 0) {
      gNI_SEND = gNI_SEND++ % 2;
      assert(_mcq[gPORT_SEND].front() != NULL && "Packet in step shouldn't be NULL");
      packet * l_pkt = _mcq[gPORT_SEND].front();
      bool can_inject = _m_net->can_inject(_port[gPORT_SEND], l_pkt, gNI_SEND);
      // cout << "can_inject: " << can_inject << endl;
      if (can_inject) {
        stats_reading_from_mcq++; // TODO: string value needs to be constant
        
        if(!_disable_training){
          feed_page_info_cache(i, l_pkt);
        }
        
        injection_rate = (stats_reading_from_mcq*1.0) / global_clock;
        l_pkt->ntw_entry_time = global_clock;
        double mcq_d = (global_clock - l_pkt->c_time);///stats_reading_from_mcq;
        stats_tot_mcq_queuing_delay+= mcq_d;
        l_pkt->mcq_delay = mcq_d;
        //cout<<"[MC] id: "<<l_pkt->id<<" dest: "<<l_pkt->dest<<endl;
        _m_net->inject(l_pkt, gNI_SEND);
        
        ////////////////////////////////////// ONLY OP BREAK DOWN STATS /////////////////////////////////
        if(!multi_program && !l_pkt->pei_request){
          assert(l_pkt->opid!=0);
          unsigned long opid = l_pkt->opid;
          unsigned long id = l_pkt->id;
          auto it = op_lat_breakdown_tracker[l_pkt->pid].find(opid);
          if(it==op_lat_breakdown_tracker[l_pkt->pid].end()){
            cout<<"[MC-2] Entry not found for pkt: "<<id<<" opid: "<<opid<<endl;
            assert("0" && "*** Entry not found for OPID while retiring packet ***");
          }
          //cout<<"[MC-2] Push back for OP: "<<opid<<" global clock: "<<global_clock<<endl;
          op_lat_breakdown_tracker[l_pkt->pid][opid].push_back(global_clock);//recording m_net entry making time
        }
        ////////////////////////////////////// ONLY OP BREAK DOWN TATS /////////////////////////////////
        
        _mcq[gPORT_SEND].erase(_mcq[gPORT_SEND].begin());
      }
      else{
      }
    }
    else{
    }
  }
  double ipc = tot_inst_committed / (double)global_clock;
  stats_avg_ipc=ipc;
  for(int port=0; port<_num_port;port++){
    stats_mcq_occ_hist[_port[port]] = (double)(_mcq[port].size()*1.0) / _max_mcq_size; 
  }
}

int MemoryController::get_port(unsigned long dest_pa)
{
  return ((dest_pa>>11 & 0x0fff)) % _num_port;//_port[((dest_pa>>12 & 0x0fff)) % _num_port];
}

void MemoryController::create_stats_vars(){
}

void MemoryController::print_stats(string bench){
}
