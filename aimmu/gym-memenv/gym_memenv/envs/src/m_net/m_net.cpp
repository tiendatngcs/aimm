#include "m_net.hpp"
#include <bits/stdc++.h>
#include <vector>
#include <math.h>

node::node(const Configuration& config){
  _mc_ports = config.GetIntArray("port_positions");
  _max_entry_vc_r_s = config.GetInt("pkt_buffer_per_vc");
  max_entry_resp_q = config.GetInt("resp_q_size");
  num_vaults = config.GetInt("num_vaults");
  num_banks_per_vault = config.GetInt("banks_per_vault");
  mem_row_hit_lat = config.GetInt("mem_row_hit_lat");
  mem_row_miss_lat = config.GetInt("mem_row_miss_lat");
  max_entry_mem_cube = num_vaults * num_banks_per_vault;
  x_dim = config.GetInt("x_dim");
  y_dim = config.GetInt("y_dim");
  
  _router_radix = config.GetInt("router_radix");
  _num_vnet = config.GetInt("num_vnet");
  _pim_tab_size =  config.GetInt("pim_tab_size");
  
  _info_pkt_queue_len = config.GetInt("info_pkt_qlen_node");

  _num_pim_entries = 0;//keeps track of exact entries made
  _pim_tab_occupancy = 0;//keeps track of reserved + actual entries

  req_q.resize(_router_radix+1);
  req_pimop_q.resize(_router_radix+1);
  resp_q.resize(_router_radix+1);
  mig_q.resize(_router_radix+1);
  forward_q.resize(_router_radix+1);

  for(int i=0; i<=_router_radix; i++){
    vc_r_s.push_back(deque<packet *>());
    vc_r_s_pimop.push_back(deque<packet *>());
    vc_r_s_resp.push_back(deque<packet *>());
    vc_r_s_mig.push_back(deque<packet *>());
    vc_r_s_forward.push_back(deque<packet *>());
    req_q[i]=0;
    req_pimop_q[i]=0;
    resp_q[i]=0;
    mig_q[i]=0;
    forward_q[i]=0;
  }

  row_buff.resize(num_vaults);
  for(int i=0;i<num_vaults;i++){
    row_buff[i].resize(num_banks_per_vault);
  }

  _retry_per_vnet_per_port.resize(_num_vnet);
  for(int v=0; v<_num_vnet; v++){
    _retry_per_vnet_per_port[v].resize(_router_radix+1);//all are set to zero initially
  }

  pim_tab_index = 0;
  
  _rr_vnet = 0;
  _rr_port = 0;
  _rr_set_port = 0;
  _num_count_vnet = 0;
  _num_count_port = 0;
}


int node::num_pkt(int v, int p){
  int size = -1;
  switch(v){
    case 0://request pkts, no pim tab entry
      size = vc_r_s[p].size();
      break;
    case 1://request pkts, pimop
      size = vc_r_s_pimop[p].size();
      break;
    case 2://response pkts
      size = vc_r_s_resp[p].size();
      break;
    case 3://migration pkts
      size = vc_r_s_mig[p].size();
      break;
    case 4://forward pkts
      size = vc_r_s_forward[p].size();
      break;
    default:
      assert(0 && "***[MNET] wrong virtual network ***");
  }    
  return size;
}

pair<int, int> node::return_pkt_src_dest(int v, int p, int i){
  int src = -1, dest = -1;
  pair<int, int>sd;
  switch(v){
    case 0://request pkts
      sd.first = vc_r_s[p][i]->src;
      sd.second = vc_r_s[p][i]->dest;
      break;
    case 1://request pkts, pimop
      sd.first = vc_r_s_pimop[p][i]->src;
      sd.second = vc_r_s_pimop[p][i]->dest;
      break;
    case 2://response pkts
      sd.first = vc_r_s_resp[p][i]->src;
      sd.second = vc_r_s_resp[p][i]->dest;
      break;
    case 3://migration pkts
      sd.first = vc_r_s_mig[p][i]->src;
      sd.second = vc_r_s_mig[p][i]->dest;
      break;
    case 4://forward pkts
      sd.first = vc_r_s_forward[p][i]->src;
      sd.second = vc_r_s_forward[p][i]->dest;
      break;
    default:
      assert(0 && "***[MNET] wrong virtual network ***");
  }
  return sd;
}


void node::clear_retry_per_vnet_per_port(){
  _retry_per_vnet_per_port.clear();
  _retry_per_vnet_per_port.resize(_num_vnet);
  for(int v=0; v<_num_vnet; v++){
    _retry_per_vnet_per_port[v].resize(_router_radix+1);//all are set to zero initially
  }
}

int node::return_vc_r_s_size(int vnet, int port){
  if(vnet==0){
    return vc_r_s[port].size();
  }
  else if(vnet==1){
    return vc_r_s_pimop[port].size();
  }
  else if(vnet==2){
    return vc_r_s_resp[port].size();
  }
  else if(vnet==3){
    return vc_r_s_mig[port].size();
  }
  else if(vnet==4){
    return vc_r_s_forward[port].size();
  }
  else{
    assert(0 && "[MNET] *** wrong vnet ***");
  }
}

int node::return_vc_req_NI_buff_size(int NI_id, packet *pkt){
  if(NI_id==0){
    assert(pkt->make_entry_pim_table || pkt->pei_request);
    return vc_r_s_pimop[5].size();
  }
  else if(NI_id==1){
    return vc_r_s[6].size();
  }
  else if(NI_id==2){
    return vc_r_s_mig[5].size();
  }
  else if(NI_id==3){
    return vc_r_s_mig[6].size();
  }
  else if(NI_id==4){
    return vc_r_s_forward[5].size();
  }
  else if(NI_id==5){
    return vc_r_s_forward[6].size();//info injection !!!
  }
  else{
    assert(0 && "*** invalid NI id ***");
  }
}

void node::inject_vc_r_s(int from_node, packet *pkt, int NI_id){
  int port = -1;
  switch(NI_id){
    case 0:
      port = 5;
      break;
    
    case 1:
      port = 6;
      break;
    
    case 2:
      port = 5;
      break;
    
    case 3:
      port = 6;
      break;
    
    case 4:
      port = 5;
      break;
    
    case 5:
      port = 6;
      break;

    default:
      assert(0 && "*** wrong NI id ***");
  }
  if(pkt->migration_pkt==true ){
    vc_r_s_mig[port].push_back(pkt);
    mig_q[port]++;
  }
  else if(pkt->forward_pkt==true){
    vc_r_s_forward[port].push_back(pkt);
    forward_q[port]++;
  }
  else if(pkt->needs_resp==true ){
    if(pkt->make_entry_pim_table==true){
      vc_r_s_pimop[port].push_back(pkt);
      req_pimop_q[port]++;
    }
    else{
      vc_r_s[port].push_back(pkt);
      req_q[port]++;
    }
  }
  else{
    assert(pkt->migration_pkt!=true);
    assert(pkt->forward_pkt!=true);
    vc_r_s_resp[port].push_back(pkt);
    resp_q[port]++;
  }
  #ifdef DEBUG_PRINT_MNET
      fprintf(debugfile, "%lu | [node:%d] injecting packet %ld at port %d occupancy[%d|%d]\n"
        ,global_clock,id,pkt->id, port, req_q[port], resp_q[port]);
  #endif        
}

void node::make_entry_vc_r_s(int from_node, packet *pkt){
  int port = get_inport(from_node, pkt->src);
  if(pkt->migration_pkt==true){
    vc_r_s_mig[port].push_back(pkt);
    mig_q[port]++;
  }
  else if(pkt->forward_pkt==true){
    vc_r_s_forward[port].push_back(pkt);
    forward_q[port]++;
  }
  else if(pkt->needs_resp==true){//if not migration packet!!!
    if(pkt->make_entry_pim_table==true){
      vc_r_s_pimop[port].push_back(pkt);
      req_pimop_q[port]++;
    }
    else{
      vc_r_s[port].push_back(pkt);
      req_q[port]++;
    }
  }
  else{
    assert(pkt->migration_pkt!=true);
    assert(pkt->forward_pkt!=true);
    vc_r_s_resp[port].push_back(pkt);
    resp_q[port]++;
  }
  #ifdef DEBUG_PRINT_MNET
  fprintf(debugfile, "%lu | [node:%d] received packet %ld at port %d\n"
        ,global_clock,id,pkt->id, port);
  #endif        
}

packet * node::read_front_pkt(int vnet, int port){ //loop will be invoked for (req+resp).size
  packet *pkt = NULL;
  if(vnet==0){
    if(req_q[port]>0){
      pkt = vc_r_s[port].front();
      assert(pkt!=NULL);
      //cout<<" "<<global_clock<<" | packet: "<<pkt->id<<" port; "<<
        //port<<endl;
      #ifdef DEBUG_PRINT_MNET
      fprintf(debugfile, "%lu | [node:%d] reading front req packet %ld at port %d [src:%d|dest:%lld]\n"
        ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
      #endif        
    }
  }
  else if(vnet==1){
    if(req_pimop_q[port]>0){
      pkt = vc_r_s_pimop[port].front();
      assert(pkt!=NULL);
      #ifdef DEBUG_PRINT_MNET
      fprintf(debugfile, "%lu | [node:%d] reading front pimop-req packet %ld at port %d [src:%d|dest:%lld]\n"
        ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
      #endif        
    }
  }
  else if(vnet==2){
    if(resp_q[port]>0){
      pkt = vc_r_s_resp[port].front();
      #ifdef DEBUG_PRINT_MNET
      fprintf(debugfile, "%lu | [node:%d] reading front resp packet %ld at port %d [src:%d|dest:%lld]\n"
        ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
      #endif        
    }
  }
  else if(vnet==3){
    if(mig_q[port]>0){
      pkt = vc_r_s_mig[port].front();
      #ifdef DEBUG_PRINT_MNET
      fprintf(debugfile, "%lu | [node:%d] reading front mig packet %ld at port %d [src:%d|dest:%lld]\n"
        ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
      #endif        
    }
  }
  else if(vnet==4){
    if(forward_q[port]>0){
      pkt = vc_r_s_forward[port].front();
      #ifdef DEBUG_PRINT_MNET
      fprintf(debugfile, "%lu | [node:%d] reading front mig packet %ld at port %d [src:%d|dest:%lld]\n"
        ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
      #endif        
    }
  }
  return pkt;
}//can read first four packets 
    
bool node::can_receive_pkt(int from_node, packet *pkt){
  int port = get_inport(from_node, pkt->src);//this is correctly getting inport
  bool can_receive = false;

  assert(port<5 && port>=0);

  if(pkt->migration_pkt){
    if(mig_q[port]==0){
      return true;
    }
    can_receive  = ((vc_r_s_mig[port].size() < _max_entry_vc_r_s));
    return can_receive;
  }
  if(pkt->forward_pkt){
    if(forward_q[port]==0){
      return true;
    }
    can_receive  = ((vc_r_s_forward[port].size() < _max_entry_vc_r_s));
    return can_receive;
  }
  else if(pkt->needs_resp && !pkt->make_entry_pim_table){
    if(req_q[port]==0){
      return true;
    }
    can_receive  = ((vc_r_s[port].size() < _max_entry_vc_r_s));
    return can_receive;
  }
  else if(pkt->needs_resp && pkt->make_entry_pim_table){
    bool can_inject_pim = true;
    if(req_pimop_q[port]==0){
      return true;
    }
    can_receive  = ((vc_r_s_pimop[port].size() < _max_entry_vc_r_s));
    
    if(pkt->dest==id){
      can_inject_pim = (return_num_pim_entries_reserved() < _pim_tab_size);   
      if(can_inject_pim == false){
        stats_pim_tab_full_hist[id]++;//only failed because of table size 
      }
    }
    return (can_receive && can_inject_pim);
  }
  else{
    if(resp_q[port]==0){
      return true;
    }
    can_receive = ((vc_r_s_resp[port].size() < _max_entry_vc_r_s));
    return can_receive;
  }
}//for sending packet

void node::pop_front_pkt(int port, packet *pkt){
  if(pkt->migration_pkt){
    #ifdef DEBUG_PRINT_MNET
    if(pkt->id != vc_r_s_mig[port].front()->id){
      fprintf(debugfile, "%lu | [node:%d] ***ERROR POP MIG*** packet %ld at port %d | front packet: %ld\n"
        ,global_clock,id, pkt->id, port, vc_r_s[port].front()->id);
    }
    #endif        
    assert(pkt->id == vc_r_s_mig[port].front()->id);
    vc_r_s_mig[port].pop_front();
    mig_q[port]--;
    #ifdef DEBUG_PRINT_MNET
    fprintf(debugfile, "%lu | [node:%d] poping front request packet %ld at port %d [src:%d|dest:%lld]\n"
      ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
    #endif        
    assert(mig_q[port]>=0);
  }
  else if(pkt->forward_pkt){
  #ifdef DEBUG_PRINT_MNET
    if(pkt->id != vc_r_s_forward[port].front()->id){
      fprintf(debugfile, "%lu | [node:%d] ***ERROR POP FORW*** packet %ld at port %d | front packet: %ld\n"
        ,global_clock,id, pkt->id, port, vc_r_s[port].front()->id);
    }
  #endif        
    assert(pkt->id == vc_r_s_forward[port].front()->id);
    vc_r_s_forward[port].pop_front();
    forward_q[port]--;
  #ifdef DEBUG_PRINT_MNET
    fprintf(debugfile, "%lu | [node:%d] poping front request packet %ld at port %d [src:%d|dest:%lld]\n"
      ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
  #endif        
    assert(forward_q[port]>=0);
  }
  else if(pkt->needs_resp && !pkt->make_entry_pim_table){
  #ifdef DEBUG_PRINT_MNET
    if(pkt->id != vc_r_s[port].front()->id){
      fprintf(debugfile, "%lu | [node:%d] ***ERROR POP REQ*** packet %ld at port %d | front packet: %ld\n"
        ,global_clock,id, pkt->id, port, vc_r_s[port].front()->id);
    }
  #endif        
    assert(pkt->id == vc_r_s[port].front()->id);
    vc_r_s[port].pop_front();
    req_q[port]--;
  #ifdef DEBUG_PRINT_MNET
    fprintf(debugfile, "%lu | [node:%d] poping front request packet %ld at port %d [src:%d|dest:%lld]\n"
      ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
  #endif        
    assert(req_q[port]>=0);
  }
  else if(pkt->needs_resp && pkt->make_entry_pim_table){
  #ifdef DEBUG_PRINT_MNET
    if(pkt->id != vc_r_s_pimop[port].front()->id){
      fprintf(debugfile, "%lu | [node:%d] ***ERROR POP REQ*** packet %ld at port %d | front packet: %ld\n"
        ,global_clock,id, pkt->id, port, vc_r_s[port].front()->id);
    }
  #endif        
    assert(pkt->id == vc_r_s_pimop[port].front()->id);
    vc_r_s_pimop[port].pop_front();
    req_pimop_q[port]--;
  #ifdef DEBUG_PRINT_MNET
    fprintf(debugfile, "%lu | [node:%d] poping front request packet %ld at port %d [src:%d|dest:%lld]\n"
      ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
  #endif        
    assert(req_q[port]>=0);
  }
  else{
  #ifdef DEBUG_PRINT_MNET
    if(pkt->id != vc_r_s_resp[port].front()->id){
      fprintf(debugfile, "%lu | [node:%d] ***ERROR POP RESP*** packet %ld at port %d | front packet: %ld\n"
        ,global_clock,id, pkt->id, port, vc_r_s_resp[port].front()->id);
    }
  #endif        
    assert(pkt->id == vc_r_s_resp[port].front()->id);
    vc_r_s_resp[port].pop_front();
    resp_q[port]--;
  #ifdef DEBUG_PRINT_MNET
    fprintf(debugfile, "%lu | [node:%d] poping front response packet %ld at port %d [src:%d|dest:%lld]\n"
      ,global_clock,id, pkt->id, port, pkt->src, pkt->dest);
  #endif        
    assert(resp_q[port]>=0);
  }
}

int node::get_inport(int from_node, int src){
  int curr = id;
  int dest = from_node;
  int inport = -1;
  int src_row = curr / x_dim;
  int dest_row = dest / x_dim;
  int src_col = curr % y_dim;
  int dest_col = dest % y_dim;

  if(curr==src){//injection
    inport = 5;
  }
  else if(curr==dest){//destination
    inport = 4;//ejection port
  }
  else{
    if(src_col == dest_col){
      if(src_row > dest_row){
        //go_up = true;
        inport = 0;
      }
      else{
        //go_down = true;
        inport = 1;
      }
    }
    else{
      if(src_col > dest_col){
        //go_left = true;
        inport = 2;
      }
      else{
        //go_right = true;
        inport = 3;
      }
    }
  }
  assert(inport!=-1);
  //cout<<"curr: "<<curr<<" dest: "<<dest<<" inport: "<<inport<<" for: "<<curr<<endl;
  return inport;
}  

int node::return_vc_buff_size(int vnet, int port){
  if(vnet==0){
    return req_q[port];
  }
  if(vnet==1){
    return req_pimop_q[port];
  }
  else if(vnet==2){
    return resp_q[port];
  }
  else if(vnet==3){
    return mig_q[port];
  }
  else{
    assert(0 && "***[MNET] wrong VNET ***");
  }
}

void node::build_pim_table(int size){
  pim_entry_tab = new pim_entry[size];
}

/////////////////////////////////////////////////////////////////
//////////////////// NODE END ///////////////////////////////////
/////////////////////////////////////////////////////////////////
MemoryNetwork * MemoryNetwork::_mnet = NULL;

MemoryNetwork::~MemoryNetwork(){}

MemoryNetwork::MemoryNetwork(const Configuration &config){
  _diff_retry_cyc = 0;
  _consecutive_possible_deadlock = 0;
  create_network(config);
}

bool MemoryNetwork::can_inject(int node, packet *pkt, int NI_id){
  bool can_inject = false;
  bool can_inject_pim = true;
  if(pkt->pim_operation){
    can_inject = (node_list[node].return_vc_req_NI_buff_size(NI_id, pkt) 
      < node_list[node].return_max_entry_vc_r_s());  
    if(pkt->dest==node){
      can_inject_pim = (node_list[node].return_num_pim_entries_reserved() < _pim_tab_size/1.1);   
    }
    return can_inject && can_inject_pim;
  }
  else{
    return (node_list[node].return_vc_req_NI_buff_size(NI_id, pkt) 
      < node_list[node].return_max_entry_vc_r_s()); 
  }
}

void MemoryNetwork::create_network(const Configuration& config){

  router_stages = config.GetInt("router_stages");
  mem_acc_lat = config.GetInt("mem_acc_lat");
  x_dim = config.GetInt("x_dim");
  y_dim = config.GetInt("y_dim");
  _router_radix_mnet = config.GetInt("router_radix");
  _num_vnet_mnet = config.GetInt("num_vnet");
  size_per_node = config.GetInt("mem_size") / (double)(x_dim * y_dim); 
  num_nodes = x_dim * y_dim;
  _frame_size =  config.GetInt("page_size");
  _tot_frame_per_cube = size_per_node * 1024 * 1024;  
  _run_baseline = (config.GetInt("dis_training")>=1);

  vector<int> tmp = config.GetIntArray("port_positions");
  mem_ports = (int *)malloc(tmp.size() * sizeof(int));
  std::copy(tmp.begin(), tmp.end(), mem_ports);
  
  //adj.resize(num_nodes*num_nodes);
  node_list.resize(num_nodes, node(config));

  string topo_file_name =  config.GetStr("topology") + ".ini";
  cout<<" "<<topo_file_name<<endl;
  read_topology(topo_file_name);

  _pim_tab_size =  config.GetInt("pim_tab_size");
  
  fprintf(debugfile,"\n===== Printing Network Adjecency List ====== \n");
  for(int i=0;i<num_nodes;i++){
    node_list[i].build_pim_table(_pim_tab_size);
    node_list[i].set_id(i);
    fprintf(debugfile,"%d | ",i);
    for(int j=0;j<(int)adj[i].size();j++){
      fprintf(debugfile,"%d ",adj[i][j]);
    }
    fprintf(debugfile,"\n");
  }
  fprintf(debugfile,"\n");
}

void MemoryNetwork::read_topology(string filename){
  cout<<"reading topology ...from "<<filename<<endl;
  ifstream file(filename.c_str());
  //assert(file!=NULL);
  string temp_str;
  int node_num=0;
  int iL;
  while(getline(file, temp_str)){
    stringstream ss(temp_str); 
    while(ss >> iL){
      adj[node_num].push_back(iL); 
      if(ss.peek()==',' || ss.peek()==' '){
        ss.ignore();
      }
    }
  node_num++;  
  }
  cout<<endl;
}

int MemoryNetwork::addr_to_nodeNum_conv(unsigned long long addr){
  unsigned long frame_num = addr >> int(log2(_frame_size * 1024));
  int cube_id = (frame_num - (frame_num% _tot_frame_per_cube))/(_tot_frame_per_cube+1);
  return cube_id;
}


void MemoryNetwork::step(){
  
  for(int node=0;node<num_nodes;node++){
    
    if((unsigned long)global_clock%100000==0 && (unsigned long)stats_total_retry_cycles!=0){
      if(node==0){
        cout<<"\n\n[(node 0 | pim occ: "<<node_list[node].return_num_pim_entries_reserved()<<") ";
      }
      else {
        cout<<"\n(node "<<node<<" | pim occ: "<<node_list[node].return_num_pim_entries_reserved()<<") ";
      }
    }

    evaluate(node);
    node_list[node].sw_arb();  
    stats_pim_full_rate_hist[node] = (node_list[node].return_num_pim_entries_reserved()*1.0) / _pim_tab_size;
    
    if(node_list[node].return_num_pim_entries_reserved() > 0){
      stats_tot_pim_tab_in_use++;
    }
  }

  if((unsigned long)global_clock%1000==0){ 
    double avg_pim_tab_usage = ((stats_tot_pim_tab_in_use*1.0)/num_nodes)/1000; 
    string v = to_string(avg_pim_tab_usage);
    if(!multi_program){
      dch_pimtab->collect(v);
    }
    stats_tot_pim_tab_in_use = 0;
    
    dch_active_pages->collect(to_string(active_pages_epochwise.size()));
    active_pages_epochwise.clear();
  }

  for(int node=0;node<num_nodes;node++){
    retire(node);
  }

}


int MemoryNetwork::mesh_neighbor_closer_to_dest(int curr, int dest){
  int min = INT_MAX;
  int next = -1;
  int src_row = curr / x_dim;
  int dest_row = dest / x_dim;
  int src_col = curr % x_dim;
  int dest_col = dest % x_dim;
  bool go_right = false;
  bool go_left = false;
  bool go_up = false;
  bool go_down = false;

  if(curr==dest){
    next = -1;
  }	    
  else{

    if(src_col == dest_col){
      if(src_row > dest_row){
        go_up = true;
        next = curr - x_dim;
      }
      else{
        go_down = true;
        next = curr + x_dim;
      }
    }
    else{
      if(src_col > dest_col){
        go_left = true;
        next = curr -1;
      }
      else{
        go_right = true;
        next = curr + 1;
      }
    }
  }
  return next;
}

void node::sw_arb(){
    //RR policy impl
    bool no_incr = false;
    if(_rr_set_vnet == _num_vnet-1){
      _rr_vnet = 0;
      _rr_set_vnet = 0;
    }
    if(_rr_set_port == _router_radix){
      _rr_port = 0;
      _rr_set_port = 0;
      no_incr=true;
    }
    _rr_port = _rr_set_port;
    _rr_vnet = _rr_set_vnet;
    
    if(!no_incr)
      _rr_set_port++;//setting the starting value for the next time
    
    _rr_set_vnet++;//setting the starting value for the next time
}

int node::rr_vnet_impl(){
 int vnet = _rr_vnet;
 _rr_port = _rr_set_port; 
 if(_rr_vnet==_num_vnet){
  _rr_vnet = 0;
  _num_count_vnet = 0;
  vnet = 0;
 }
  _num_count_vnet++;
  _rr_vnet++;
 return vnet;//should return the value first and then increment 
}

int node::rr_port_impl(){
 int port = _rr_port;
 if(_rr_port==_router_radix+1){
  _rr_port = 0;
  port = 0;
  _num_count_port = 0;
 }
  _num_count_port++;
  _rr_port++;
 return port;//should return the value first and then increment 
}

void node::make_link_busy(int next_node){
  auto it = _link.find(next_node);
  if(it == _link.end()){
    _link[next_node] = 1;  
  }
}

bool node::link_busy(int next_node){
  auto it = _link.find(next_node);
  bool busy = false;
  if(it != _link.end()){
    busy = true;
  }
  return busy;
}

void MemoryNetwork::debug_evaluate(int node, int vnet_it, int vnet, int port_it, int port){
  if((unsigned long)global_clock%100000==0 && (unsigned long)stats_total_retry_cycles!=0){
    if(node_list[node].return_vc_r_s_size(vnet,port)!=0){
      cout<<"<"<<vnet<<","<<port<<">"<<node_list[node].return_vc_r_s_size(vnet,port)<<",";
    }
    if(port_it==_router_radix_mnet && vnet_it<_num_vnet_mnet){
      cout<<" | ";
    }
    if(node==(num_nodes-1) && vnet_it==(_num_vnet_mnet-1) && port_it==_router_radix_mnet){
      cout<<"]\n\n"<<endl;
      _diff_retry_cyc = stats_total_retry_cycles - _diff_retry_cyc;
      if(_diff_retry_cyc >= num_nodes * 50000){
        cout<<"\n*** Possible Network Deadlock Detected ***\n"<<endl;
        _consecutive_possible_deadlock++;
      }
      _diff_retry_cyc = stats_total_retry_cycles;
      if(_consecutive_possible_deadlock >= 3){
        cout<<"\nRetry per vnet per port: \n";
        //retry debud data print
        for(int n=0; n < num_nodes; n++){
          cout<<"NODE: "<<n<<": ";
          for(int v=0; v < _num_vnet_mnet; v++){
            for(int p=0; p < _router_radix_mnet; p++){
              if(node_list[n].return_retry_per_vnet_per_port(v,p)){
                cout<<"<"<<v<<","<<p<<">"<<node_list[n].return_retry_per_vnet_per_port(v, p)<<","; 
              }
            }
          }
          cout<<endl;
        }
        cout<<"\n\n"<<endl;

        //packets src-dest info print
        for(int n=0; n< num_nodes; n++){
          cout<<"NODE: "<<n<<" "<<endl;
          for(int v=0; v < _num_vnet_mnet; v++){
            cout<<"VNET: "<<v;
            for(int p=0; p <= _router_radix_mnet; p++){
              cout<<" [PORT: "<<p<<" "; 
              for(int i=0; i< node_list[n].num_pkt(v,p); i++){
                pair<int, int> pktinfo = node_list[n].return_pkt_src_dest(v,p,i);
                cout<<"{"<<pktinfo.first<<","<<pktinfo.second<<"}";
              }
              cout<<"] ";
            }
            cout<<endl;
          }
        }
        
        cout<<"\nPIM source request queue:"<<endl;
        for(int n=0; n< num_nodes; n++){
          if(node_list[n].has_pim_pkt()){    
            cout<<"node: "<<n<<" front pkt: "<<node_list[n].return_front_pim_tab_packets_id()
              <<", ";
          }
        }
        
        cout<<"\nResponse queue:"<<endl;
        for(int n=0; n< num_nodes; n++){
          if(node_list[n].has_response()){    
            cout<<"node: "<<n<<" front pkt: "<<node_list[n].return_front_resp_packets_id()
              <<", ";
          }
        }
        
        cout<<"\n"<<endl;
        assert(0 && "*** Possible Network Deadlock Detected Consecutive Three Times ***");
      }
    }
  } 
}

void MemoryNetwork::evaluate(int node){

  node_list[node].reset_sending_pkts();
  node_list[node].clear_link();

  int served_pkt = 0; 

  int vnet = -1;
  int port = -1;

  for(int vnet_it=0; vnet_it<_num_vnet_mnet; vnet_it++){//request network & response network
    
    vnet = node_list[node].rr_vnet_impl(); 
    
    for(int port_it=0; port_it<=_router_radix_mnet; port_it++){
      
      port = node_list[node].rr_port_impl();
      
      debug_evaluate(node, vnet_it, vnet, port_it, port);
      
      packet *l_pkt = node_list[node].read_front_pkt(vnet, port);
      
      if(l_pkt!=NULL && l_pkt->ready_to_send()){ //ready to send is about whether it spent enough time in the router
        long long l_dest = l_pkt->get_dest();//it has to return next router
        int next_node = mesh_neighbor_closer_to_dest(node,l_dest);
          
        if(node_list[node].link_busy(next_node)){//implementing link contentions 
            continue;
        }  
        
        if(l_dest!=node){//current node is not the destination
          //it checks in the inport of the next node: from_node--> node, to node -->next_node
          bool can_send = node_list[next_node].can_receive_pkt(node, l_pkt);// it is checking for credit to go to
          int inport = node_list[next_node].get_inport(node, l_pkt->src);
          if(can_send){

            node_list[node].make_link_busy(next_node); 
             
            _consecutive_possible_deadlock = 0;
            node_list[node].clear_retry_per_vnet_per_port();
            node_list[node].incr_sending_pkts();
            unsigned long retry_cnt = l_pkt->retry_to_send;
            #ifdef DEBUG_PRINT_MNET
            if((watch_packet!=-1 && l_pkt->id == watch_packet) || watch_all_packets){
              fprintf(debugfile,"%lu | sending packet %lu from node %d [outport: %d] to node %d [inport:%d] (src:%d|dest:%lld) retry %lu\n",
                  global_clock,l_pkt->id, node, port, next_node,inport,l_pkt->src,l_dest,retry_cnt);        
            }
            #endif
            node_list[node].pop_front_pkt(port, l_pkt);
            l_pkt->hops++;
            l_pkt->retry_to_send = 0;
            node_list[next_node].make_entry_vc_r_s(node, l_pkt);
            stats_total_hops++;
            if(l_pkt->pim_operation){
              stats_pim_hops++;
            }
          }
          else{
            #ifdef DEBUG_PRINT_MNET
            if((watch_packet!=-1 && l_pkt->id == watch_packet) || watch_all_packets){
              fprintf(debugfile,"%lu | retry to send packet %lu from node %d [outport: %d] to node %d [inport:%d] (src:%d|dest:%lld) retry %lu\n",
                  global_clock,l_pkt->id, node, port, next_node,inport,l_pkt->src,l_dest,l_pkt->retry_to_send);
            }
            #endif
            l_pkt->retry_to_send++;
            stats_total_retry_cycles++;
            if(_consecutive_possible_deadlock > 0){
              node_list[node].record_retry_per_vnet_per_port(vnet,port);
            }
            double avg_retry_cycles_per_pkt = stats_total_retry_cycles / stats_total_pkts; 
            stats_retry_ntw_cycles_per_pkt = avg_retry_cycles_per_pkt;
          }
        }
        else{//if destination
          if(l_pkt->pim_operation){
            //assert(l_pkt->needs_resp==true);
            if(l_pkt->dest_va!=0)
            active_pages_epochwise[l_pkt->dest_va >> 11]++;
            if(l_pkt->src1_va!=0)
            active_pages_epochwise[l_pkt->src1_va >> 11]++;
            if(l_pkt->src2_va!=0)
            active_pages_epochwise[l_pkt->src2_va >> 11]++;

            handle_pim_inst(node, port, l_pkt);
          }
          else{
            handle_regular_pkts_at_dest(node, port,l_pkt);
          }
        }
      }
      else{
        if(l_pkt!=nullptr){
          assert(l_pkt->not_ready_to_send++ <= router_stages + l_pkt->pkt_size);
        }
      }
    }
  }
}


void MemoryNetwork::handle_regular_pkts_at_dest(int node, int port, packet *l_pkt){
  
  l_pkt->lat += global_clock - l_pkt->ntw_entry_time; 
  assert(l_pkt->pid!=-1);
  
  if(l_pkt->needs_resp==false && l_pkt->migration_pkt==false){//to make sure it is a resp pkt

    node_list[node].pop_front_pkt(port,l_pkt);
    //node_list[node].make_link_busy(node); 
    
    node_list[node].check_pim_op_roundtrip(l_pkt->resp_for_pkt_id, global_clock, false);

    #ifdef DEBUG_PRINT_MNET
    if((watch_packet!=-1 && l_pkt->id == watch_packet) || watch_all_packets){
      fprintf(debugfile,"%lu | consuming response packet %lu(src:%d|dest:%lld) [migration? %d]\n",
          global_clock,l_pkt->id,l_pkt->src,l_pkt->dest, (l_pkt->migration_pkt==true?1:0));        
    }
    #endif      
    
    if(l_pkt->forward_pkt){
      stats_retiring_forward_pkt++;
    }

    stats_total_hops++;
    stats_retired_pkt++;
    if(l_pkt->pim_operation){
      stats_pim_hops++;
    }
    
    free(l_pkt);
  }
  else{//read or write request packets

    if(l_pkt->retry_to_mem==0){ 
      double net_lat_per_acc = stats_total_network_lat /stats_total_pkts;
      stats_avg_pkt_lat=net_lat_per_acc;
      stats_total_network_lat+=l_pkt->lat;
    }
    if(node_list[node].can_make_entry_for_mem() /*&& !l_pkt->waiting_for_pim_sources*/){
      //node_list[node].make_link_busy(node); 
      node_list[node].pop_front_pkt(port,l_pkt);
      l_pkt->entry_time = global_clock;
      //////////////////////////////////////////////// OP STATS ///////////////////////////////////////////
      if(!multi_program && !l_pkt->pei_request){
        if(l_pkt->opid!=0){
          auto it = op_lat_breakdown_tracker[l_pkt->pid].find(l_pkt->opid);
          if(it==op_lat_breakdown_tracker[l_pkt->pid].end()){
            cout<<"Entry not found for pkt: "<<l_pkt->id<<" opid: "<<l_pkt->opid<<endl;
            assert(0 && "*** Entry not found for OPID while reached dest node ***");
          }
          op_lat_breakdown_tracker[l_pkt->pid][l_pkt->opid].push_back(global_clock);//recording when reached dest node
        }
      }
      //////////////////////////////////////////////// OP STATS ///////////////////////////////////////////
      l_pkt->hops++;
      unsigned long time_2_dest = global_clock - l_pkt->c_time;
      node_list[node].make_entry_mem_cube(l_pkt);//for pim operation writing the destinationh!!!
      #ifdef DEBUG_PRINT_MNET
      if((watch_packet!=-1 && l_pkt->id == watch_packet) || watch_all_packets){
        fprintf(debugfile,"%lu | packet %lu reached memQ dest %lld hops %d cycles %lu retry %lu mem queuing %lu\n",
            global_clock,l_pkt->id,l_pkt->dest,l_pkt->hops,time_2_dest,l_pkt->retry_to_send,l_pkt->retry_to_mem);
      }
      #endif     
      stats_avg_num_hops = ++stats_total_hops / stats_total_pkts;
      if(l_pkt->pim_operation){
        stats_pim_hops++;
      }
    }
    else{
      unsigned long time_2_dest = global_clock - l_pkt->c_time;
      stats_mem_queuing_delay++;
      double q_delay_per_acc = stats_mem_queuing_delay / stats_total_memory_acc;
      stats_mem_queuing_delay_per_acc=q_delay_per_acc;
      l_pkt->retry_to_mem++;
      #ifdef DEBUG_PRINT_MNET
      if((watch_packet!=-1 && l_pkt->id == watch_packet) || watch_all_packets){
        fprintf(debugfile,"%lu | packet %lu retry in memQ dest %lld hops %d cycles %lu retry %lu mem queuing %lu\n",
            global_clock,l_pkt->id,l_pkt->dest,l_pkt->hops,time_2_dest,l_pkt->retry_to_send,l_pkt->retry_to_mem);
      }
      #endif     
    }
  }
}

bool node::count_and_return_pg_hist(unsigned long pg){
  bool create_pkt = false;
  auto it = _info_monitor.find(pg);
  if(it != _info_monitor.end()){
    if(it->second > pg_hist_len){
      _info_monitor.erase(it);
      return true;
    }
    else{
      it->second++;
    }
  }
  else{
    _info_monitor[pg] = 1;//first access   
  }
  return create_pkt;
}

void node::create_and_enqueue_info_pkt(unsigned long pg, int pid){
  
  packet *l_pkt = new packet;
  l_pkt->pid = pid;
  l_pkt->c_time = global_clock;
  l_pkt->src = id;
  l_pkt->forward_pkt = true;
  int dest_mc = rand()%_mc_ports.size();
  l_pkt->dest = _mc_ports[dest_mc];//TODO: one of the node which has MC
  l_pkt->pkt_size = ceil(resp_pkt_size /(double) link_width);
  stats_num_info_pkts++; 
  if(_info_pkt_queue.size() > _info_pkt_queue_len){
    _info_pkt_queue.pop_front();//implementing fixed length FIFO queue
  }
  
  _info_pkt_queue.push_back(l_pkt); 
}


void MemoryNetwork::retire(int node){
  bool entered_loop = false;
  for(int i=0;i<node_list[node].return_mem_cube_size();i++){
    entered_loop = true;
    if(node_list[node].return_mem_cube_size()>0){
      
      packet *l_pkt = node_list[node].return_pkt_mem_queue(i);
      
      if(l_pkt->mem_acc_lat==-1){//access latency is not set
        l_pkt->mem_acc_lat = node_list[node].record_rowbuff_entry(l_pkt);
      }
      if(l_pkt->ready_to_retire() && node_list[node].check_and_make_response(l_pkt)){
        node_list[node].erase_mem_cube_entry(i);
        unsigned long time_taken = global_clock - l_pkt->c_time -l_pkt->mcq_delay;
        #ifdef DEBUG_PRINT_MNET
        if((watch_packet!=-1 && l_pkt->id == watch_packet) || watch_all_packets){
          fprintf(debugfile,"%lu | retiring packet %lu in %lu cycles\n",global_clock, l_pkt->id, time_taken);
        }
        #endif
        if(l_pkt->pim_operation){
          stats_pim_acc_lat += global_clock - l_pkt->c_time;
          stats_pim_op_ret++;
        }

        assert(l_pkt->pid!=-1 && "*** packet must have a pid ***");
        sys_hop_count_monitor[l_pkt->pid].first++;
        sys_hop_count_monitor[l_pkt->pid].second += l_pkt->hops;
        hop_count_monitor[l_pkt->pid].first++;
        hop_count_monitor[l_pkt->pid].second += l_pkt->hops;
        unsigned long temp_pkt_lat = 0;

        if(l_pkt->opid!=0){
          stats_types_of_pkts_retire[0]++;
            if(!multi_program && !l_pkt->pei_request){
            auto it = op_lat_breakdown_tracker[l_pkt->pid].find(l_pkt->opid);
            if(it==op_lat_breakdown_tracker[l_pkt->pid].end()){
              cout<<"Entry not found for pkt: "<<l_pkt->id<<" opid: "<<l_pkt->opid<<endl;
              assert(0 && "*** Entry not found for OPID while retiring packet ***");
            }
            op_lat_breakdown_tracker[l_pkt->pid][l_pkt->opid].push_back(global_clock);//recording final retire time
            vector<unsigned long> &tmp = op_lat_breakdown_tracker[l_pkt->pid][l_pkt->opid];
            for(int i=0; i<5; i++){
              assert(tmp[i+1] >= tmp[i]);
              temp_pkt_lat += tmp[i+1] - tmp[i];
              stats_op_latency_breakdown[i] += tmp[i+1] - tmp[i];
            }
          op_lat_breakdown_tracker[l_pkt->pid].erase(l_pkt->opid);//final record done 
          }
        }
        else{//response packets, pim source requests, migration 
          if(l_pkt->migration_pkt==false){//resp, pim src req
            stats_types_of_pkts_retire[1]++;
          }
          else{//migrration pkts
            stats_types_of_pkts_retire[2]++;
          }
        }
        if(l_pkt->migration_pkt==true){
          if(!(l_pkt->pid >= 0 && l_pkt->pid <= num_proc_submission)){cout<<"[MNET] Invalid PID: "<<l_pkt->pid<<endl;}
          assert(l_pkt->pid >=0 && l_pkt->pid <= num_proc_submission);
          migration_completion_check[l_pkt->pid]--;
        }
        
        /////////////////////////////////// page_info_map: start ////////////////////////////////
        if(l_pkt->dest_va!=-1 && l_pkt->opid!=0){
          auto it = page_info_map[l_pkt->pid].find(l_pkt->dest_va>>11);
          if(it != page_info_map[l_pkt->pid].end()){
            page_info_map[l_pkt->pid][l_pkt->dest_va>>11]->add_op_hist(l_pkt->hops, temp_pkt_lat);
            if(node_list[node].count_and_return_pg_hist(l_pkt->dest_va >> 11)){
              node_list[node].create_and_enqueue_info_pkt(l_pkt->dest_va >> 11, l_pkt->pid);
            }
          }
        } 
        if(l_pkt->src1_va!=-1 && l_pkt->opid!=0){
          auto it = page_info_map[l_pkt->pid].find(l_pkt->src1_va>>11);
          if(it != page_info_map[l_pkt->pid].end()){
            page_info_map[l_pkt->pid][l_pkt->src1_va>>11]->add_op_hist(l_pkt->hops, temp_pkt_lat);
            if(node_list[node].count_and_return_pg_hist(l_pkt->src1_va >> 11)){
              node_list[node].create_and_enqueue_info_pkt(l_pkt->src1_va >> 11, l_pkt->pid);
            }
          }
        } 
        if(l_pkt->src2_va!=-1 && l_pkt->opid!=0){
          auto it = page_info_map[l_pkt->pid].find(l_pkt->src2_va>>11);
          if(it != page_info_map[l_pkt->pid].end()){
            page_info_map[l_pkt->pid][l_pkt->src2_va>>11]->add_op_hist(l_pkt->hops, temp_pkt_lat);
            if(node_list[node].count_and_return_pg_hist(l_pkt->src2_va >> 11)){
              node_list[node].create_and_enqueue_info_pkt(l_pkt->src2_va >> 11, l_pkt->pid);
            }
          }
        }
        if(l_pkt->migration_pkt==true){
          auto it = page_info_map[l_pkt->pid].find(l_pkt->migr_pg_addr);
          if(it != page_info_map[l_pkt->pid].end()){
            if(l_pkt->migr_tail){
              page_info_map[l_pkt->pid][l_pkt->migr_pg_addr]->set_migr_end_time(global_clock);
              page_info_map[l_pkt->pid][l_pkt->migr_pg_addr]->set_migr_in_progress(false);
            }
            unsigned long migr_delay = page_info_map[l_pkt->pid][l_pkt->migr_pg_addr]->get_migration_delay();
            page_info_map[l_pkt->pid][l_pkt->migr_pg_addr]->add_migr_hist(migr_delay);
          }
        }

        //----------------------------
        // create info packet
        //----------------------------
        // in each node 
        // check the pages in that node being accessed
        // keep counting history for each page 
        // when the history length is reached for one page 
        // prepare a packet and 
        // if a packet for the page already exist
        //    replace with the new one
        // else 
        //    enqueue that in the info queue
        
        //------------------------------
        // inject and send info packet
        //------------------------------
        // at each cycle
        // info queue should be checked 
        // if packet is there
        // should try to inject the packets   
        
        // need to have a info virtual network !!!

        /////////////////////////////////// page_info_map: end //////////////////////////////////
        
        
        stats_retired_pkt++;
        if(!l_pkt->migration_pkt){
          stats_retired_mem_acc++;
        }
        stats_retired_mem_acc_with_migr++;
        stats_retired_mem_acc_apps[l_pkt->pid]++;
        stats_total_mem_s2ret_lat += time_taken; 
        stats_total_DRAM_acc_lat += global_clock - l_pkt->entry_time;
        stats_avg_mem_acc_lat = stats_total_mem_s2ret_lat / stats_retired_mem_acc_with_migr;
        stats_avg_DRAM_acc_lat = stats_total_DRAM_acc_lat / stats_retired_mem_acc_with_migr;
        
        free(l_pkt);
      }
    }
    else{
      stats_mem_cube_empty++;
    }  
  }
  if(entered_loop==false){
    stats_mem_cube_size_zero++;
  }
  inflight_ntw_pkt=0;
  for(int i=0; i<=5; i++){
    inflight_ntw_pkt += node_list[node].return_vc_buff_size(0,i) + 
      node_list[node].return_vc_buff_size(1,i);
  }
  inflight_mem_pkt = node_list[node].return_mem_cube_size();
  
  node_list[node].send_response();
  node_list[node].send_forward_pkt();
  node_list[node].send_pim_requests();
  if(global_clock%1000==0){//sending info in regular interval
  	node_list[node].send_info_pkt();
  }
}

///////////////////////// injection to network ////////////////////////////////////
void MemoryNetwork::inject(packet *l_pkt, int NI_id){
  node_list[l_pkt->src].inject_vc_r_s(l_pkt->src, l_pkt, NI_id); 
  if(l_pkt->make_entry_pim_table){ 
    node_list[l_pkt->src].check_pim_op_roundtrip(l_pkt->id, global_clock, true/*req*/);
  }
}


///////////////////////// ejection from network ////////////////////////////////////
void MemoryNetwork::received_responses(vector<unsigned long> &ejection_q){
  for(int n = 0; n < num_nodes; n++){
    node_list[n].eject_rcvd_pkts(ejection_q);
  }
}

void node::eject_rcvd_pkts(vector<unsigned long>&ejec){
  for(auto it=_pimop_response_rcvd.begin(); it!=_pimop_response_rcvd.end(); ++it){
    ejec.push_back(it->first);
  }
  _pimop_response_rcvd.clear();
}

void node::check_pim_op_roundtrip(unsigned long id, unsigned long cyc, bool req){
  auto it = _mem_nw_roundtrip.find(id);
  if(it != _mem_nw_roundtrip.end()){
    //it must be a response
    assert(req == false);
    unsigned long roundtrip_time = cyc - it->second.first;
    _pimop_response_rcvd[id] = make_pair(it->second.first, it->second.second);
    _mem_nw_roundtrip.erase(it); 
  }
  else if(req==true){
    _mem_nw_roundtrip[id] = make_pair(cyc,0);
  }
  stats_roundtrip_pimq_size = _mem_nw_roundtrip.size(); 
}
///////////////////////////////////////////////////////////////////////////////////



int node::record_rowbuff_entry(packet *pkt){
  unsigned long addr = pkt->pa;
  int lat = -1;
  if(addr!=0){ 
    int phy_page_num = pkt->pa >> 11;//keeping same as VA. Can be different
    int bank = phy_page_num & (num_banks_per_vault-1); 
    assert(bank<num_banks_per_vault); 
    int vault = (phy_page_num >> num_banks_per_vault) & (num_vaults-1);
    assert(vault<num_vaults);
    if(row_buff[vault][bank] == phy_page_num){
      stats_mem_row_hit++;
      stats_tot_row_hit_hist[id]++;
      lat = mem_row_hit_lat;      
    }
    else{
      stats_mem_row_miss++;
      stats_tot_row_miss_hist[id]++;
      row_buff[vault][bank] = phy_page_num;
      lat = mem_row_miss_lat;      
    }
    stats_row_buffer_missrate_hist[id] = (stats_tot_row_miss_hist[id] * 1.0) / 
      (stats_tot_row_hit_hist[id] + stats_tot_row_miss_hist[id]);
  return lat;
  }
  else{
    //TODO: need to see this happens????
  }
}

void node::send_response(){
  for(int i=0;i<5;i++){
    bool can_inj = can_inject_resp(5);
    if(can_inj && has_response()){
      int NI_id = 0;
      packet *l_pkt = get_resp_pkt();
      l_pkt->ntw_entry_time = global_clock;
      make_entry_vc_r_s(this->id, l_pkt);  
      remove_resp_pkt();
      stats_can_not_inject_response_packet--;
      stats_has_no_pim_packet--;
    }
    else{
      if(can_inj==false){
        stats_can_not_inject_response_packet++;
      }
      if(has_response()==false){
        stats_has_no_response_packet++;
      } 
    }
  }
}

bool node::check_and_make_response(packet *pkt){
  if(avail_resp_queue()){//cheking space in the resp queue
    if(pkt->needs_resp){
      packet *l_pkt = new packet;
      l_pkt->pid = pkt->pid;
      l_pkt->c_time = global_clock;
      l_pkt->src = id;
      l_pkt->dest = pkt->src;
      l_pkt->pa = pkt->src;
      l_pkt->resp_for_pkt_id = pkt->id;
      l_pkt->pim_src1_pa = pkt->pim_src1_pa;
      l_pkt->pim_src2_pa = pkt->pim_src2_pa;
      l_pkt->dest_pa = pkt->dest_pa;//this is coming after src_to_node conversion
      l_pkt->pim_operation = pkt->pim_operation;
      l_pkt->pim_table_index = pkt->pim_table_index;
      l_pkt->pkt_size = ceil(resp_pkt_size /(double) link_width);
      l_pkt->needs_resp = false;
      make_entry_resp_queue(l_pkt);
      stats_total_resp_pkts++;
      stats_total_pkts++;
    }
    return true;
  }
  else if(!pkt->needs_resp){
    return true;
  }
  else{
    stats_failed_to_create_resp_pkts++;
    return false;//can not retire the current req packet
  }
}

void node::send_info_pkt(){
  for(int i=0;i<5;i++){
    int NI_id = 5;
    bool can_inj = return_vc_req_NI_buff_size(NI_id, NULL) 
      < _max_entry_vc_r_s; 
    if(can_inj && has_info_pkt()){
      packet *l_pkt = _info_pkt_queue.front();
      l_pkt->ntw_entry_time = global_clock;
      
      inject_vc_r_s(l_pkt->src, l_pkt, NI_id);
      
      _info_pkt_queue.pop_front();
    }
    else{
      if(can_inj==false){
        //stats_can_not_inject_response_packet++;
      }
      if(has_info_pkt()==false){
        //stats_has_no_response_packet++;
      } 
    }
  }
}


void node::send_forward_pkt(){
  for(int i=0;i<5;i++){
    int NI_id = 4;
    //bool can_inj = can_inject_resp(6);
    bool can_inj = return_vc_req_NI_buff_size(NI_id, NULL) 
      < _max_entry_vc_r_s; 
    if(can_inj && has_forward()){
      packet *l_pkt = _results_forward_queue.front();
      l_pkt->ntw_entry_time = global_clock;
      //make_entry_vc_r_s(this->id, l_pkt);  
      
      inject_vc_r_s(l_pkt->src, l_pkt, NI_id);
      
      _results_forward_queue.pop_front();
      //stats_can_not_inject_response_packet--;
      //stats_has_no_pim_packet--;
    }
    else{
      if(can_inj==false){
        //stats_can_not_inject_response_packet++;
      }
      if(has_forward()==false){
        //stats_has_no_response_packet++;
      } 
    }
  }
}

bool node::make_entry_forward_queue(packet *l_pkt){
      //packet *l_pkt = new packet;
      //l_pkt->pid = pkt->pid;
      //l_pkt->c_time = global_clock;
      l_pkt->src = id;
      l_pkt->opid = 0;
      l_pkt->dest = l_pkt->forward_dest;
      //l_pkt->entry_time = global_clock;
      //l_pkt->pa = pkt->src;
      //l_pkt->resp_for_pkt_id = pkt->id;
      //l_pkt->pim_src1_pa = pkt->pim_src1_pa;
      //l_pkt->pim_src2_pa = pkt->pim_src2_pa;
      //l_pkt->dest_pa = pkt->dest_pa;//this is coming after src_to_node conversion
      //l_pkt->pim_operation = pkt->pim_operation;
      //l_pkt->pim_table_index = pkt->pim_table_index;
      l_pkt->not_ready_to_send = 0;
      l_pkt->pkt_size = ceil(req_pkt_size /(double) link_width);//only one calculation at a time
      l_pkt->needs_resp = false;
      l_pkt->pim_operation = false;
      l_pkt->make_entry_pim_table = false;
      l_pkt->forward_pkt = true;
      _results_forward_queue.push_back(l_pkt);
      //stats_total_pkts++;
      stats_forward_pkt_created++;
      return true;
}

void node::send_pim_requests(){
  for(int i=0;i<5-return_sending_pkts();i++){
    bool can_inj = can_inject_req(5);
    if(can_inj && has_pim_pkt()){
      packet *l_pkt = get_pim_pkt();
      int NI_id = 0;
      l_pkt->ntw_entry_time = global_clock;
      l_pkt->entry_time = global_clock;
      make_entry_vc_r_s(this->id, l_pkt);  
      //inject_vc_r_s(id, l_pkt, NI_id);  
      remove_pim_pkt();
      stats_can_not_inject_pim_packet--;
      stats_has_no_pim_packet--;
    }
    else{
      if(can_inj==false){
        stats_can_not_inject_pim_packet++;
      }
      if(has_pim_pkt()==false){
        stats_has_no_pim_packet++;
      } 
    }
  }
}

void node::make_pim_packets(int i, int node, packet *pkt){
}

int node::find_empty_space_pim_tab(){
  for(int i=0;i<_pim_tab_size;i++){
    if(!return_pim_entry_tab_status(i)){
      return i;
    }
  }
  return -1;//menas no space available
}


packet * MemoryNetwork::create_src_pkt(packet *org_pkt, int src_num, int node, int index){
        
  packet *pkt = new packet;
  
  pkt->pid = org_pkt->pid;
  pkt->c_time = global_clock;
  pkt->src = node;
  pkt->pkt_size = ceil(req_pkt_size /(double) link_width);
  pkt->needs_resp = true;
  pkt->pim_table_index = index; 
  pkt->pim_operation = true;
  pkt->dest_pa = org_pkt->dest_pa;//this is coming after src_to_node conversion
  
  switch(src_num){
    case 0:
      pkt->pim_src1_pa = org_pkt->pim_src1_pa;
      pkt->dest = org_pkt->pim_src1;//node
      pkt->pa = org_pkt->pim_src1;//this is coming after src_to_node conversion
      break;
    case 1:
      pkt->pim_src2_pa = org_pkt->pim_src2_pa;
      pkt->dest = org_pkt->pim_src2;//node
      pkt->pa = org_pkt->pim_src2;//this is coming after src_to_node conversion
      break;
    default:
      assert(0 && "*** only provisioned for max 2 source operands ***");
  }
  return pkt; 
}

void MemoryNetwork::handle_pim_inst(int node, int port, packet *pkt){//must be a pim request
  
  if(pkt->make_entry_pim_table==true){
    assert(pkt->make_entry_pim_table==true && "need to make entry in pim table must be true");
    assert(pkt->needs_resp==true && "Must be a PIM request that needs response");  
    int l_pim_table_index = node_list[node].find_empty_space_pim_tab();
    
    if(l_pim_table_index == -1){
      stats_pim_tab_full++;
    }
    else {
      
      int num_src_sent_out = 0;

      assert(l_pim_table_index!=-1 && "Invalid PIM table index returned");
      
      node_list[node].set_pim_tab_entry(true, l_pim_table_index);
      node_list[node].set_pim_entry_time(global_clock, l_pim_table_index);
      node_list[node].incr_num_pim_entries();//occupancy counter incr should notbe here with pim entries
                                             // as that is already incremented before
      node_list[node].incr_pim_tab_occ();

      pkt->waiting_for_pim_sources = true;
      
      //sending request for the source-1 even if it is the same node
      if(pkt->pim_src1!=-1 /*&& pkt->pim_src1!=node*/){
        stats_total_memory_acc++;
        packet *src1_pkt = create_src_pkt(pkt, 0, node, l_pim_table_index);
        //assert(src1_pkt->pim_src1_pa!=0 && src1_pkt->dest_pa!=0);
        node_list[node].set_pim_entry_src1_status(false, l_pim_table_index); 
        node_list[node].set_pim_entry_src1_pktid(src1_pkt->id, l_pim_table_index); 
        stats_total_pkts++;
        node_list[node].make_entry_pim_req_queue(src1_pkt);//we will try to push it to vc_r_s in next cycle
        pkt->pim_src1 = -1;//resetting the field as it is more required

        #ifdef DEBUG_PRINT_MNET
        if((watch_packet!=-1 && pkt->id == watch_packet) || watch_all_packets){
          fprintf(debugfile,"%lu | [node:%d] PIM packet %lu | req src-1 packet %lu [src:%d|dest:%lld] \n"
              ,global_clock, node, pkt->id, src1_pkt->id, src1_pkt->src, src1_pkt->dest);        
        }
        #endif
        num_src_sent_out++;
      }
      //else{
      //  node_list[node].set_pim_entry_src1_status(true, l_pim_table_index); 
      //}

      //sending request for source-2 if applicable
      if(pkt->pim_src2!=-1 /*&& pkt->pim_src2!=node*/){
        stats_total_memory_acc++;
        packet *src2_pkt = create_src_pkt(pkt, 1, node, l_pim_table_index);
        //assert(src2_pkt->pim_src2_pa!=0 && src2_pkt->dest_pa!=0);
        node_list[node].set_pim_entry_src2_status(false, l_pim_table_index); 
        node_list[node].set_pim_entry_src2_pktid(src2_pkt->id, l_pim_table_index); 
        stats_total_pkts++;
        node_list[node].make_entry_pim_req_queue(src2_pkt);//we will try to push it to vc_r_s in next cycle
        pkt->pim_src2 = -1;//resetting the field as it is no more required

        #ifdef DEBUG_PRINT_MNET
        if((watch_packet!=-1 && pkt->id == watch_packet) || watch_all_packets){
          fprintf(debugfile,"%lu | [node:%d] PIM packet %lu | req src-2 packet %lu [src:%d|dest:%lld] \n"
              ,global_clock, node, pkt->id, src2_pkt->id, src2_pkt->src, src2_pkt->dest);        
        }
        #endif
        num_src_sent_out++;
      }
      //else{
      //  node_list[node].set_pim_entry_src2_status(true, l_pim_table_index); 
      //}

      //set table entry
      pkt->pim_table_index = l_pim_table_index; 
      node_list[node].set_pim_entry_pkt(pkt, l_pim_table_index);//this one is the orginal pkt 
      node_list[node].pop_front_pkt(port,pkt);//removing packet from vc_r_s
      
      //handling the case when both the sources are in the same node 
      if(num_src_sent_out==0){
        assert(0 && "****Legacy code not handling this way ****");
        pkt->pim_operation = false;
        pkt->needs_resp = true;
        pkt->waiting_for_pim_sources = false;
        pkt->entry_time = global_clock;
        pkt->hops++;
        
        if(pkt->forward_dest == -1){
          node_list[node].make_entry_mem_cube(pkt);//for pim operation writing the destinationh!!!
        }
        else{
          pkt->make_entry_pim_table = false;
          if(pkt->forward_dest != node){
            node_list[node].make_entry_forward_queue(pkt);
          }
          //node_list[node].make_entry_mem_cube(l_pkt);//for pim operation writing the destinationh!!!
          free(pkt);
        }
        node_list[node].set_pim_tab_entry(false, l_pim_table_index);
        node_list[node].dcr_num_pim_entries();
        node_list[node].dcr_pim_tab_occ(); 
        if(stats_pim_tab_full>0){
          stats_pim_tab_full--;
        }
        //////////////////////////////////////////////// OP STATS ///////////////////////////////////////////
        if(!multi_program && !pkt->pei_request){
          if(pkt->opid > 0){
            //cout<<"pid: "<<pkt->pid<<endl;
            int pid = pkt->pid % num_proc_submission;
            if(pkt->pid <= num_proc_submission){//corrupted packet //TODO: check it later 
              auto it = op_lat_breakdown_tracker[pid].find(pkt->opid);
              if(it==op_lat_breakdown_tracker[pid].end()){
                cout<<"Entry not found for pkt: "<<pkt->id<<" opid: "<<pkt->opid<<endl;
                assert(0 && "*** Entry not found for OPID while reached dest node ***");
              }
              op_lat_breakdown_tracker[pid][pkt->opid].push_back(global_clock);//recording when reached dest node
            }
          }
        }
        //////////////////////////////////////////////// OP STATS ///////////////////////////////////////////
      }
      //both entries in the same node handled
    }
  }  
  else{//need not make an entry in the pim table
    if(pkt->needs_resp==true){//THIS MUST BE HANDLED IN THE REGULAR FLOW
      pkt->waiting_for_pim_sources = false;
      handle_regular_pkts_at_dest(node, port, pkt);
    }
    else{//got the response, so update the pim tables
      int index = pkt->pim_table_index;
      assert(index!=-1 && "*** Invalid PIM entry table index ***");

      if(node_list[node].return_src1_pkt_id(index) == pkt->resp_for_pkt_id){
        node_list[node].set_pim_entry_src1_status(true, index);
        node_list[node].set_pim_src1_reach_time(global_clock, index);
        node_list[node].pop_front_pkt(port, pkt);
        unsigned long entry_time = node_list[node].return_pim_entry_time(index);
        //assert(pkt->pim_src1_pa!=0);
        stats_retired_pkt++;
        free(pkt);
      } 
      if(node_list[node].return_src2_pkt_id(index) == pkt->resp_for_pkt_id){
        node_list[node].set_pim_entry_src2_status(true, index);
        node_list[node].set_pim_src2_reach_time(global_clock, index);
        unsigned long entry_time = node_list[node].return_pim_entry_time(index);
        //assert(pkt->pim_src2_pa!=0);
        node_list[node].pop_front_pkt(port,pkt);
        stats_retired_pkt++;
        free(pkt);
      }
      if(node_list[node].return_pim_entry_src1_status(index) && 
          node_list[node].return_pim_entry_src2_status(index)){
        if(node_list[node].can_make_entry_for_mem_pim()){
          packet * l_pkt =  node_list[node].get_pim_entry_pkt(index);
          l_pkt->pim_operation = false;
          l_pkt->needs_resp = true;
          l_pkt->waiting_for_pim_sources = false;
          node_list[node].set_pim_tab_entry(false, index);
          node_list[node].dcr_num_pim_entries();
          node_list[node].dcr_pim_tab_occ(); 
          if(stats_pim_tab_full>0){
            stats_pim_tab_full--;
          }
          //TODO
          l_pkt->entry_time = global_clock;
          //////////////////////////////////////////////// OP STATS ///////////////////////////////////////////
          if(!multi_program && !l_pkt->pei_request){
            if(l_pkt->opid!=0){
              auto it = op_lat_breakdown_tracker[l_pkt->pid].find(l_pkt->opid);
              if(it==op_lat_breakdown_tracker[l_pkt->pid].end()){
                cout<<"Entry not found for pkt: "<<l_pkt->id<<" opid: "<<l_pkt->opid<<endl;
                assert(0 && "*** Entry not found for OPID while reached dest node ***");
              }
              op_lat_breakdown_tracker[l_pkt->pid][l_pkt->opid].push_back(global_clock);//recording when reached dest node
            }
          }
          //////////////////////////////////////////////// OP STATS ///////////////////////////////////////////
          l_pkt->hops++;
          unsigned long time_2_dest = global_clock - l_pkt->c_time;
          if(l_pkt->forward_dest == -1){
            node_list[node].make_entry_mem_cube(l_pkt);//for pim operation writing the destination!!!
          }
          else{
            l_pkt->make_entry_pim_table = false;
            if(pkt->forward_dest != node){
              node_list[node].make_entry_forward_queue(l_pkt);
              //node_list[node].make_entry_mem_cube(l_pkt);//for pim operation writing the destination!!!
            }
            //node_list[node].make_entry_mem_cube(l_pkt);//for pim operation writing the destination!!!
            node_list[node].check_pim_op_roundtrip(l_pkt->id, global_clock, false);
            //stats_retired_pkt++;
            //free(l_pkt);
          }
          //TODO
          unsigned long src1_reach_time = node_list[node].return_pim_src1_reach_time(index);
          unsigned long src2_reach_time = node_list[node].return_pim_src2_reach_time(index);
          unsigned long entry_time = node_list[node].return_pim_entry_time(index);
          unsigned long avg_reach_time = (src1_reach_time + src2_reach_time)/2;
          #ifdef DEBUG_PRINT_MNET
            if((watch_packet!=-1 && l_pkt->id == watch_packet) || watch_all_packets){
              fprintf(debugfile,"%lu | received both pim sources for packet %ld (can inject)\n"
                  ,global_clock, l_pkt->id);        
            }
          #endif
        }
        else{
        #ifdef DEBUG_PRINT_MNET
            if((watch_packet!=-1 && node_list[node].return_pim_entry_pkt_id(index) == watch_packet) || watch_all_packets){
              fprintf(debugfile,"%lu | received both pim sources for packet %ld (can NOT inject)\n",
                  global_clock, node_list[node].return_pim_entry_pkt_id(index));        
            }
        #endif
        }
      }
    }
  }
}
