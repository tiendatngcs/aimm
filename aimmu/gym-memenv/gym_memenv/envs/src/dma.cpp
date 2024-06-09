#include "dma.hpp"

DirectMemoryAccess * DirectMemoryAccess::_dma = NULL;

DirectMemoryAccess::DirectMemoryAccess(){}
DirectMemoryAccess::~DirectMemoryAccess(){}

void DirectMemoryAccess::make_entry(int pid, int s, int d, unsigned long pg, unsigned long tot_migr){
  MigrCand *l_mgc = new MigrCand(s,d,pg,tot_migr);
  _MgCand[pid].push_back(l_mgc);
  //for(int i=0; i<_MgCand[pid].size(); i++){
  //  cout<<"[DMA] pid:"<<pid<< "while recording -- pg: "<<_MgCand[pid][i]->get_pg_num()<<" tot_migr: "<<_MgCand[pid][i]->get_migr_count()<<endl;
  //}
}

void DirectMemoryAccess::set_up(const Configuration &config){
  _mem_size = config.GetInt("mem_size");
  _page_size = config.GetInt("page_size");
  _x_dim = config.GetInt("x_dim");
  _y_dim = config.GetInt("y_dim");
  _simulate_migr = config.GetInt("dis_migr_sim")==0;
  _num_nodes = _x_dim * _y_dim;
  _size_per_node = _mem_size / (double)(_num_nodes);
  _num_pkt_per_page = (_page_size/*KB*/ * 1024/*byte each flit*/)/256/*packet size in bytes*/; 
  create_ports(_num_nodes);//assuming that all the nodes can be connected to DMA
  _mem_net = MemoryNetwork::GetInstance(config);
  _MgCand.resize(num_proc_submission, vector<MigrCand *>());
  cout<<" migration candiate queue size: "<<_MgCand.size()<<endl;
}

void DirectMemoryAccess::step(){
  int num_nodes = _x_dim * _y_dim;
  //reading packets
  //#pragma omp parallel for
  //for(int sp=0; sp<sleep_process.size(); sp++){
  //  int pid = sleep_process[sp];
  if(_simulate_migr){
    #pragma omp parallel for num_threads(8) collapse(2)
    for(unsigned long pid=0; pid < num_proc_submission; pid++){ 
      assert(pid < num_proc_submission);
      int busy_port = 0;
      for(int node=0; node < num_nodes; node++){
        if(ports[node].get_state()==0){//means idle
          for(int c = 0; c < _MgCand[pid].size(); c++){
            int src = _MgCand[pid][c]->get_src();
            long long dest = _MgCand[pid][c]->get_dest();
            unsigned long pg = _MgCand[pid][c]->get_pg_num();
            //cout<<"[DMA] pid:"<<pid <<" Setting Pkt details: pg: "<<pg<<endl; 
            if(src == node){
              ports[node].set_pkt_details(pid, src, dest, pg, _num_pkt_per_page);
              ports[node].set_state(1);//busy 
              //cout<<"[DMA] XXXXXXX DELTE XXXXXXX pid:"<<pid<< "while recording -- pg:"<<_MgCand[pid][c]->get_pg_num()<<" tot_migr: "<<_MgCand[pid][c]->get_migr_count()<<endl;
              _MgCand[pid].erase(_MgCand[pid].begin()+c); 
              break;        
            }
          }
        }
        else{
          busy_port++;
        }
      }
      if(busy_port == num_nodes){
        break;
      }
    }
    //sending packets
    //#pragma omp parallel for
    #pragma omp parallel for num_threads(8) collapse(2)
    for(int node=0; node < num_nodes; node++){
      if(ports[node].get_state()==1){//means busy
        packet *pkt =  ports[node].get_packet();
        if(pkt!= nullptr && _mem_net->can_inject(node, pkt, 2/*migr/DMA NI*/) && sleep_process[pkt->pid]==1){
           
          if(ports[node].return_num_pkt_remained()==1){
            unsigned long pg = pkt->migr_pg_addr;
            if(pg!=0){
              page_info_map[pkt->pid][pg]->incr_num_migr();
              unsigned long tot_acc = page_info_map[pkt->pid][pg]->get_num_max_acc();  
              unsigned long acc_comp = page_info_map[pkt->pid][pg]->get_acc_done();
              unsigned long migr = page_info_map[pkt->pid][pg]->get_migr_cnt();
              //cout<<"[DMA] Pg:"<<pg<<" | Remaining access after migration: "<<tot_acc-acc_comp<<
              //  " tot acc: "<<tot_acc<< " acc comp: "<<acc_comp<<" migr: "<<migr<<endl;  
            }
          }

          pkt->ntw_entry_time = global_clock;
          _mem_net->inject(pkt, 2/*DMA NI-id*/);
          stats_total_pkts++;//also counting the migration packets
          if(!(pkt->pid >= 0 && pkt->pid <= num_proc_submission)){cout<<"[DMA] Invalid PID: "<<pkt->pid<<endl;}
          assert(pkt->pid >= 0 && pkt->pid <= num_proc_submission);
          migration_completion_check[pkt->pid]++;
          remove_pkt_entry(node);
          //printf("[DMA] %lu | migration packet %lu (src:%d|dest:%lld)\n",
          //    global_clock,pkt->id, pkt->src,pkt->dest);
#ifdef DEBUG_PRINT_MNET
          if((watch_packet!=-1 && pkt->id == watch_packet) || watch_all_packets){
            fprintf(debugfile,"%lu | migration packet %lu (src:%d|dest:%lld)\n",
                global_clock,pkt->id, pkt->src,pkt->dest);
          }
#endif
        }
        else{
          free(pkt);
          stats_retired_pkt++;
        }
      } 
    }  
  }
} 

///////////////////////////////////////////////////////////////////
packet * DMAPort::get_packet(){
  if(packet_remained()){
    packet * pkt = new packet();
    pkt->pid = _pid;
    pkt->src = _cand_src; 
    pkt->dest = _cand_dest;
    pkt->migration_pkt = true;
    pkt->c_time = global_clock;
    pkt->pkt_size = 8;//8 flits 256 bit (32 Byte) (only for migr)
    pkt->migr_pg_addr = _cand_pg;
    stats_total_pkts++;
    if(first_packet()){
      pkt->migr_head = true; 
      page_info_map[_pid][_cand_pg]->set_migr_in_progress(true);
      page_info_map[_pid][_cand_pg]->set_migr_start_time(global_clock); 
    }
    if(last_packet()){
      pkt->migr_tail = true;  
    }
    assert(pkt->pid >= 0 && pkt->pid <= num_proc_submission);
    return pkt;
  }
  else{
    set_state(0);//free  
    return NULL;
  }
}
