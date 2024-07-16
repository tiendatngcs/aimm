#include "vmap.hpp"
#include <string.h>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

unsigned long pim_file_no = 0;
unsigned long pim_op_cnt = 0;

FILE *op_trace;

vm::vm(unsigned long sa, unsigned long ea, unsigned long se, int pid)
  : _s_addr(sa), _e_addr(ea), _size_element(se), _pid(pid) {}

long vm::address_at(int index)
{
  stats_memory_requested += _size_element; 

  return _s_addr + index * _size_element;
}

int vm::get_pid() {return _pid;}

v_map_use::v_map_use(const Configuration& config)
  : _text_base(-1), _heap_base(-1), _stack_base(-1), _shared_base(-1),
  _text_upperbound(-1), _shared_upperbound(-1),
  _r_heap_base(-1), _r_stack_base(-1),
  _num_port(-1),
  _req_pkt_size(-1), _resp_pkt_size(-1), _link_width(-1)
{
 
  /************************************************/
  /*            Setting up CMP nnetwork           */
  /************************************************/ 
  cmp_net::GetInstance()->setup_cmp(config);
  
  /*************************************************/
  /*              Set address range                */
  /*************************************************/
  _text_base = config.GetAddr("text_s");
  _text_upperbound = config.GetAddr("text_e");
  _heap_base = config.GetAddr("heap_s");
  _stack_base = config.GetAddr("stack_s");
  _shared_base = config.GetAddr("shared_s");
  _shared_upperbound = config.GetAddr("shared_e");
  req_pkt_size = config.GetInt("req_pkt_size");
  resp_pkt_size = config.GetInt("resp_pkt_size");
  link_width = config.GetInt("link_width");
  _num_port = config.GetInt("num_port");
  _x_dim = config.GetInt("x_dim");
  _y_dim = config.GetInt("y_dim");
  _size_per_node = config.GetInt("mem_size") / (double)(_x_dim * _y_dim); 
  _frame_size =  config.GetInt("page_size");
  _tot_frame_per_cube = (_size_per_node * 1024 * 1024) / _frame_size;  
  _disable_training = (config.GetInt("dis_training")>=1);  
  _periodic_break = (config.GetInt("periodic_break")>=1);

  _baseline_load_bal = config. GetInt("load_balance_baseline")>0;//load balance off by default


  _num_req_flits = ceil(req_pkt_size /(double) link_width);
  _num_write_flits = ceil(resp_pkt_size /(double) link_width);

  _tom_enable = config.GetInt("tom_phy_cube_remap")>0;
  if(_tom_enable){
    _tom_bit_masks = 0x1E;
    _tom_collocation_count.resize(_x_dim * _y_dim);
    _tom_create_possible_masks();
    _pmi = 0;
    _tom_epoch = 0;
    _tom_training = true;
    _prev_cube = -1;
  }

  _r_heap_base = _heap_base;
  _r_stack_base = _stack_base;
  _l_v2p = nullptr;//this will be connected using connect_v2p
  _g = new Graph(config);
  _done = false;

  cout << "text: "<< hex << _text_base <<" - " << _text_upperbound << endl;
  cout << "heap: "<< hex << _heap_base << endl; //" - " << _heap_upperbound << endl;
  cout << "stack: "<< hex << _stack_base << endl; //<<" - " << _stack_upperbound << endl;
  cout << "available space for heap and stack: " << hex << _stack_base - _heap_base << endl;
  cout << "shared: "<< hex << _shared_base <<" - " << _shared_upperbound << endl;

  fprintf(debugfile,"\n===== Virtual Memory Configurations =====\n");
  fprintf(debugfile,"text: %lx - %lx\n", _text_base, _text_upperbound);
  fprintf(debugfile,"heap: %lx\n", _heap_base);
  fprintf(debugfile,"stack: %lx\n", _stack_base);
  fprintf(debugfile,"available space for heap and stak: %lx\n", _stack_base - _heap_base);
  fprintf(debugfile,"shared: %lx - %lx\n", _shared_base, _shared_upperbound);

  //create_stats_vars();

  op_trace = fopen("pim_trace/pim-0.op", "w");
}

v_map_use::~v_map_use()
{
  //g_stats.display_stats("stats_file_name.stats");
}

vm *v_map_use::allocate_heap(unsigned long n, unsigned long s, int pid)
{
  unsigned long  size = n * s;
  unsigned long s_addr = _r_heap_base;
  unsigned long e_addr = _r_heap_base + size;
  
  cout<<" n= "<<n<<" s= "<<s<<endl; 
  cout<<"start address: "<<s_addr<<" end address: "<<e_addr<<" size: "<<size<<" stack base: "<<_r_stack_base<<endl;  
  assert(e_addr <= _r_stack_base && "heap is overflowed");

  vm *l_vm = new vm(_r_heap_base, e_addr, s, pid);
  _r_heap_base += size;

#ifdef DEBUG_PRINT_VMAP
  fprintf(debugfile, "%lu | allocate heap, start:%lx end:%lx size:%lu Bytes\n",
      global_clock, s_addr, e_addr, size);
#endif
 stats_memory_allocated += size;//bytes 
 return l_vm;
}

bool v_map_use::read_memory(vm* varray, int index)
{
  bool success = false;
  assert(varray != NULL && "*** accessing null pointer in read_memory() ***");

  long target_addr = varray->address_at(index);
  
  bool got_it = get_phy_addr(varray,index);  
  if(got_it==false) return false;
  
  unsigned long long dest_pa = _l_v2p->GetPhyAddr(varray->get_pid(), target_addr);
  
  //cout<<"----- read request to: "<<dest_pa<<endl; 

  int port = get_port(dest_pa);

  stats_total_instructions_exec++;

  cmp_net *l_cmp_net = cmp_net::GetInstance();
  
  if(l_cmp_net->can_make_entry(varray->get_pid(), port)){ 
    packet *l_pkt = new packet;
    l_pkt->c_time = global_clock;
    l_pkt->src = rand()%4;//_port[port];
    l_pkt->dest = dest_pa;
    //l_pkt->id = _packet_id;
    l_pkt->pa = dest_pa;
    l_pkt->pkt_size = _num_req_flits;
    l_pkt->needs_resp = true;
    l_pkt->rd_req = true;
    //_packet_id++;
    stats_total_pkts++;
    stats_total_memory_acc++;
 
    l_cmp_net->make_entry(varray->get_pid(),port, l_pkt);
    success = true;
  }
  else{
    success = false;
  }
#ifdef DEBUG_PRINT_VMAP
  //fprintf(debugfile,"%lu | read_memory: mcq[%d].size: %lu, packet %lu, src %d, dest %d\n",
  //    global_clock, port, _mcq[port].size(), l_pkt->id, l_pkt->src, dest_node);
#endif
return success;
}

void v_map_use::non_memory(const double ratio)
{
  float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  while (r < ratio) {
    //step();
    //g_stats.incr("total_non_memory_inst",1);
    stats_total_non_memory_inst++;
    stats_total_instructions_exec++;
    r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
  }
}

bool v_map_use::write_memory(vm* varray, int index)
{
  bool success = false;
  assert(varray != NULL && "*** accessing null pointer in read_memory() ***");

  long target_addr = varray->address_at(index);
  
  bool got_it = get_phy_addr(varray,index);  
  if(got_it==false) return false;
  
  unsigned long long dest_pa = _l_v2p->GetPhyAddr(varray->get_pid(), target_addr); 
  
  //cout<<"++++++ write request to: "<<dest_pa<<endl; 

  int port = get_port(dest_pa);

  stats_total_instructions_exec++;

  cmp_net *l_cmp_net = cmp_net::GetInstance();
  if(l_cmp_net->can_make_entry(varray->get_pid(),port)){ 
    packet *l_pkt = new packet;
    l_pkt->c_time = global_clock;
    l_pkt->src = rand()%4;//_port[port];
    l_pkt->dest = dest_pa;
    l_pkt->pa = dest_pa;
    //l_pkt->id = _packet_id;
    l_pkt->pkt_size = _num_write_flits;//contains data
    l_pkt->needs_resp = false;
    l_pkt->wr_req = true;
    //_packet_id++;
    stats_total_pkts++;
    stats_total_memory_acc++;
 
    l_cmp_net->make_entry(varray->get_pid(), port, l_pkt);
    success = true;
  }
  else{
    success = false;
  }
#ifdef DEBUG_PRINT_VMAP
  //fprintf(debugfile,"%lu | write_memory: mcq[%d].size: %lu, packet %lu, src %d, dest %d\n",
  //    global_clock, port, _mcq[port].size(), l_pkt->id, l_pkt->src, dest_node);
#endif
  return success;
}


bool v_map_use::get_phy_addr(vm* addr, int index){
  bool got_frame = false;
  long l_addr = addr->address_at(index);
  int pid = addr->get_pid();
  unsigned long long phy_addr = _l_v2p->GetPhyAddr(pid,l_addr);
  if(phy_addr!=0){ 
    got_frame = true;
  }
  missed_pid = l_addr;
 //cout<<" get phy address +++++++++++++++++++++++++++"<<" vaddr: "<<l_addr<<" phyaddr: "<<phy_addr<<endl;
 return got_frame; 
}


void v_map_use::_tom_create_possible_masks(){
  ///int num_bits_ele = int(log2(8));//8 Byte max
  ///int num_ele_in_cache_blk = int(log2(8));
  ///_tom_LSB = num_ele_in_cache_blk + num_bits_ele;
  ///_tom_MSB = 10;
  ///int num_mask_bits = _tom_MSB - _tom_LSB + 1;
  //int num_of_ones = int(log2(_x_dim * _y_dim));
  //int num_of_zeros = num_mask_bits - num_of_ones; 
  assert(_x_dim * _y_dim == 16 && "[VMAP: TOM]the logic is not scalable");
  _possible_masks.push_back(0x0F);//@index 0
  _possible_masks.push_back(0x17);
  _possible_masks.push_back(0x1B);
  _possible_masks.push_back(0x1D);
  _possible_masks.push_back(0x1E);//@index 4
}

int v_map_use::_tom_training_for_epoch(unsigned long vaddr, unsigned long op_id){
  int cube_id = _get_tom_cube_id(vaddr);
  if(_prev_op_id == op_id){
     _tom_collocation_count[_pmi%5]++;//basically a confidence counter   
  } 
  _prev_cube = cube_id;
  return cube_id;
}

void v_map_use::_tom_select_mask(){
  unsigned long max = 0;
  int index = -1;
  for(int i=0; i<5; i++){
    if(_tom_collocation_count[i] >= max){
      max = _tom_collocation_count[i];
      index = i;
    }
  }
  assert(index != -1);
  assert(index < 5);
  _tom_bit_masks = _possible_masks[index];
  assert(_tom_bit_masks != 0);
  for(int i=0; i<5; i++){
    _tom_collocation_count[i] = 0;
  }
}

int v_map_use::_get_tom_cube_id(unsigned long vaddr){//this is an access address
  int cube_id = 0;
  int pid = get_pid();
  int shifting_by = int(log2(_frame_size * 1024));
  unsigned long pg_num = (vaddr >> 11);
  int num_bits_ele = int(log2(8));//8 Byte max
  int num_ele_in_cache_blk = int(log2(8));
  _tom_LSB = num_ele_in_cache_blk + num_bits_ele;
  _tom_MSB = 10;
  unsigned long after_offset_pg = vaddr >> _tom_LSB;
  unsigned long bit_mask = _tom_bit_masks;
  unsigned long mask_to_apply_on = after_offset_pg & 0x1F;//5 LSB bits taken
  
  assert(bit_mask != 0);

  if(_tom_training){
    if(_tom_epoch%1000==0){//time to reevaluate as the change in epoch
        _tom_bit_masks = _possible_masks[_pmi%5];
        _pmi++;
    }
    if(_tom_epoch%5000==0){
      _tom_training = false; 
      _tom_select_mask();
    }   
  }
  else{
    if(_tom_epoch%100000==0){
      _tom_training = true;
    }
  }

  int nm = 0;
  for(int i=0; i<(_tom_MSB+1 - _tom_LSB); i++){
    nm = ((bit_mask & 0x1)==1) ? pow(2,i) : 0;
    bit_mask = bit_mask >> 1;
    if(nm > 0){
      cube_id = ((mask_to_apply_on & nm)>0) ? (cube_id<<1 | 0x1) : (cube_id<<1);
    }
  }
  //assert(cube_id != -1);
  //cout<<"page: "<<(vaddr>>11)<<" cube: "<<cube_id<<" masking on: "<<mask_to_apply_on<<" mask: "<<(unsigned long)_tom_bit_masks<<endl; 
  return cube_id;
}


pair<int, int> v_map_use::addr_to_nodeNum_conv(bool dest, unsigned long vaddr, unsigned long long addr, unsigned long op_id){
  
  int shifting_by = int(log2(_frame_size * 1024));
  unsigned long frame_num = (addr >> shifting_by);
  unsigned long page = vaddr >> shifting_by; 
  int org_cube_id = (frame_num - (frame_num % _tot_frame_per_cube))/(_tot_frame_per_cube+1);
  int new_cube_id = -1; 

  if(org_cube_id<0 || org_cube_id >=_x_dim*_y_dim){
    //cout<<"[VMAP] *** Wrong cube id: "<<org_cube_id<<" ***"<<endl;
    //cout<<"[VMAP] addr: "<<addr<<" frame: "<<frame_num<<" cube: "<<org_cube_id<<" tot_frame_per_cube:"
    //  <<_tot_frame_per_cube<<"  frame size: "<<_frame_size<<" shifting by: "<<shifting_by<<endl;
    org_cube_id = org_cube_id % (_x_dim * _y_dim);
    if(org_cube_id < 0)org_cube_id = 0;
    vmap_wrong_cube_incident++;
  }
  
  if(_tom_enable==true){
    _tom_epoch++;
    if(!_tom_training){
      new_cube_id = _tom_training_for_epoch(vaddr, op_id); 
      if(new_cube_id == -1){
        new_cube_id = org_cube_id;
      }
    }
    else{
      //while training, check if the op_id==_prev_op_id
      //then check if the current mapping colocating them
      //if yes, then increment colocationting counter for that mapping
      //each of the mapping will spend some cycles
      //once all the mapping is being trained then we will select the best one 
      //for the next epoch
      _tom_training_for_epoch(vaddr, op_id);
      new_cube_id = org_cube_id;
    }
  }
  else if(!_disable_training){
    auto it = compute_migration_cand_list[get_pid()].find(page); 
    if(!_disable_training && it!=compute_migration_cand_list[get_pid()].end()){//TODO: fix the size of the queue
      //cout<<"[VMAP] ++++++++++++++ found page entry for computation migr: "<<page<<endl;
      if(dest && it->second==3){
        int num_neighbors = adj[org_cube_id].size();
        new_cube_id = adj[org_cube_id][rand()%num_neighbors];
        //cout<<"[VMAP][dest-->node 1]<<"<<"page: "<<page <<" orifinal cube id: "<<org_cube_id<<" new id:"
          //<<new_cube_id<<"----------------------------------------------------"<<endl;
      }
      else if(dest && it->second==4){//far computation migration --> shooting for diagonally opposite quadrants
        if(_x_dim * _y_dim == 16){
          new_cube_id = (org_cube_id & 0x1) | (org_cube_id & 0x2)==0?1:0 
            | (org_cube_id & 0x4) | (org_cube_id & 0x8)==0?1:0; 
          assert(new_cube_id < _x_dim * _y_dim);
        }
        else{
          new_cube_id = rand()%(_x_dim * _y_dim);//TODO:: Need to fixit with correct formula
        } 
      }  
      //else if(dest && it->second==4){
      //  cout<<"[VMAP] looking for page (inference based): "<<page<<endl; 
      //  assert(0);
      //  //cout<<"[VMAP]<<"<<"page: "<<page <<" orifinal cube id: "<<org_cube_id<<" new id: "<<it->second<<endl;
      //  //new_cube_id = it->second;  
      //  //new_cube_id = rand()%(_x_dim*_y_dim);
      //  py::function get_cube = py::reinterpret_borrow<py::function>(py::module::import("aimmuagent.infer_cube").attr("find_cube").attr("get_cube"));
      //
      //  trace_buffer.clear();
      //  trace_buffer.push_back(stats_pim_full_rate_hist);//pim table occupancy [cube_no]
      //  trace_buffer.push_back(stats_row_buffer_missrate_hist);//row buffer missrate [cube no]
      //  trace_buffer.push_back(stats_mcq_occ_hist);//memory controller queue [cube no]
      //  
      //  if(possible_migration_cand_list.size() > 0){//initial entry skippied
      //    vector<long double>pg_vec;
      //    page_info_map[page]->return_consolidated_pg_info(pg_vec); 
      //    trace_buffer.push_back(pg_vec);//memory controller queue [cube no]
      //  }
      //
      //  py::object cube_num = get_cube(trace_buffer);
      //  new_cube_id = cube_num.cast<int>();
      //  if(new_cube_id >= _x_dim*_y_dim){
      //    new_cube_id = org_cube_id;
      //  }
      //  else{
      //    new_cube_id = it->second;
      //  }
      //  //cout<<"[VMAP][dest-->node 2]<<"<<"page: "<<page <<" orifinal cube id: "<<org_cube_id<<" new id: "<<new_cube_id<<endl;
      //}
      else{
        new_cube_id = org_cube_id;
      }
    }
    else{
      new_cube_id = org_cube_id;
    }
  }
  else{
      new_cube_id = org_cube_id;
  }
  assert(new_cube_id>=0); 
  assert(new_cube_id < _x_dim*_y_dim);
  assert(org_cube_id>=0); 
  assert(org_cube_id < _x_dim*_y_dim);

  return make_pair(org_cube_id, new_cube_id);
}
////////////////////////////////////////////////////////

void v_map_use::create_input_n2v(){
  int start_index = 0;//TODO need to have a logic to select this index
  _g->prep_input_for_node2vec(start_index);
}

bool v_map_use::pim_operation_trace(int pid, unsigned long op_id, 
    unsigned long dest, unsigned long src1, unsigned long src2){
    
  // cout<<"[VAMP] entered in VMAP pim operation trace"<<endl; 
  /*
   * Operation Latency Breakdown Tracker
   */
  if(!multi_program){
    auto op_it = op_lat_breakdown_tracker[pid].find(op_id);
    if(op_it == op_lat_breakdown_tracker[pid].end()){
      // cout<<"[VMAP] Making entry for OP: "<<op_id<<endl;
      op_lat_breakdown_tracker[pid].insert(make_pair(op_id,vector<unsigned long>()));
      op_lat_breakdown_tracker[pid][op_id].push_back(global_clock);
      // cout<<"[VMAP-1] Push back for OP: "<<op_id<<" global clock: "<<global_clock<<endl;
      //assert(op_lat_breakdown_tracker[op_id][0] == global_clock); 
    }
  }
  assert(pid<=num_proc_submission);

  bool success = false;
  unsigned long long pa_src2 = 0;
  int node_src2 = 0;
  unsigned long long pa_dest = _l_v2p->GetPhyAddr(pid,dest);
  auto node_p_d = addr_to_nodeNum_conv(true, dest, pa_dest, op_id);
  int node_dest = node_p_d.second;//take new 
  assert(node_dest!=-1);
  unsigned long long pa_src1 = _l_v2p->GetPhyAddr(pid,src1);
  auto node_p_s1 = addr_to_nodeNum_conv(false, src1, pa_src1, op_id);
  int node_src1 = node_p_s1.first;//take original
  //assert(node_p_s1.second==-1);
  if(src2!=0){//in case of 2 operand pim src2 will be zero
    pa_src2 = _l_v2p->GetPhyAddr(pid,src2);
    auto node_p_s2 = addr_to_nodeNum_conv(false, src2, pa_src2, op_id);
    //assert(node_p_s2.second==-1);
    node_src2 = node_p_s2.first;//take original
  }

  int port = get_port(pa_dest);
  cmp_net *l_cmp_net = cmp_net::GetInstance();
  
  if(l_cmp_net->can_make_entry(pid, port)){
    // cout << "Can make entry" << endl;
    stats_pim_operations++;
    packet *l_pkt = new packet;
    l_pkt->pid = pid;//recording process id
    l_pkt->opid = op_id;//recording operation id
    l_pkt->c_time = global_clock;
    //l_pkt->src = rand()%4;//_port[port];
    l_pkt->src = port; 
    int src1_cube = -1;
    int src2_cube = -1;
    pair<int, int> p_temp;
   
    int case_num = -1;
    if(flip_dest_node){
      case_num = 1;
      flip_dest_node = false;
    } 
    else{
      case_num = 0;
    }

    if(_baseline_load_bal==true){
      case_num = 1;//flip source to dest
    }

    switch(case_num){
      case 0:
        l_pkt->dest = node_dest;//node numbers
        l_pkt->dest_pa = pa_dest;//holding physical addresses
        l_pkt->dest_va = dest;

        l_pkt->pim_src1 = node_src1;//node numbers
        if(src2!=0){//by default the value is l_pkt->pim_src2 == -1
          l_pkt->pim_src2 = node_src2;
        }  
        l_pkt->pim_src1_pa = pa_src1;//holding physical addresses
        l_pkt->pim_src2_pa = pa_src2;//holding physical addresses
        l_pkt->src1_va = src1;
        l_pkt->src2_va = src2;
        break;
      
      case 1:
        p_temp = addr_to_nodeNum_conv(true, src1, pa_src1, op_id);
        src1_cube = p_temp.second;
        assert(p_temp.second!=-1);
        l_pkt->forward_dest = p_temp.first;
        l_pkt->dest = src1_cube;//node numbers
        l_pkt->dest_pa = pa_src1;//holding physical addresses
        l_pkt->dest_va = src1;
        
        p_temp = addr_to_nodeNum_conv(true, dest, pa_dest, op_id);
        node_dest = p_temp.first; 
        l_pkt->pim_src1 = node_dest;//node numbers
        if(src2!=0){//by default the value is l_pkt->pim_src2 == -1
          l_pkt->pim_src2 = node_src2;
        }  
        l_pkt->pim_src1_pa = pa_dest;//holding physical addresses
        l_pkt->pim_src2_pa = pa_src2;//holding physical addresses
        l_pkt->src1_va = dest;
        l_pkt->src2_va = src2;
        break;
      
      case 2:
        p_temp = addr_to_nodeNum_conv(false, dest, pa_dest, op_id);
        node_dest = p_temp.first; 
        l_pkt->forward_dest = p_temp.first;
        l_pkt->pim_src2 = node_dest;
        l_pkt->pim_src1 = node_src1;//node numbers
        l_pkt->pim_src1_pa = pa_src1;//holding physical addresses
        l_pkt->pim_src2_pa = pa_dest;//holding physical addresses
        l_pkt->src1_va = src1;
        l_pkt->src2_va = dest;
        if(src2!=0){//by default the value is l_pkt->pim_src2 == -1
            p_temp = addr_to_nodeNum_conv(true, src2, pa_src2, op_id);
            src2_cube = p_temp.second;
            l_pkt->dest = src2_cube;//node numbers
            l_pkt->dest_pa = pa_src2;//holding physical addresses
            l_pkt->dest_va = src2;
            break;
          }


      default:
        l_pkt->dest = node_dest;//node numbers
        l_pkt->dest_pa = pa_dest;//holding physical addresses
        l_pkt->dest_va = dest;

        l_pkt->pim_src1 = node_src1;//node numbers
        if(src2!=0){//by default the value is l_pkt->pim_src2 == -1
          l_pkt->pim_src2 = node_src2;
        }  
        l_pkt->pim_src1_pa = pa_src1;//holding physical addresses
        l_pkt->pim_src2_pa = pa_src2;//holding physical addresses
        l_pkt->src1_va = src1;
        l_pkt->src2_va = src2;
    }
    l_pkt->pa = pa_dest;
    l_pkt->pkt_size = _num_req_flits;
    l_pkt->needs_resp = true;
    l_pkt->rd_req = true;
    l_pkt->pim_operation = true;
    l_pkt->make_entry_pim_table = true;
    stats_total_pkts++;
    stats_total_memory_acc++;

    if (src1) {
      page_access_count_epochwise[src1 >> 11]++;
      page_access_count_global[src1 >> 11]++;
    }
    if (src2) {
      page_access_count_epochwise[src2 >> 11]++;
      page_access_count_global[src2 >> 11]++;
    }
    if (dest) {
      page_access_count_epochwise[dest >> 11]++;
      page_access_count_global[dest >> 11]++;
    }

    unsigned long epoch = global_clock / 10000;
    if (src1) {
      last_access_epoch[src1 >> 11] = epoch;
    }
    if (src2) {
      last_access_epoch[src2 >> 11] = epoch;
    }
    if (dest) {
      last_access_epoch[dest >> 11] = epoch;
    }


    
 
    l_cmp_net->make_entry(pid, port, l_pkt);
    success = true;
    
    // cout<<"[VMAP] "<<"id: "<<l_pkt->id<<" vaddr: "<<dest<<" phyaddr: "<<pa_dest<<" dest cube: "<<node_dest<<endl;

    //for stats analysis only
    if(!multi_program){
      assert(l_pkt->opid!=0);
      op_lat_breakdown_tracker[pid][op_id].push_back(global_clock);//recording cmp entry making time
    //cout<<"[VMAP-2] Push back for OP: "<<op_id<<" global clock: "<<global_clock<<endl;
    }

    /***** Recording Start *****/
    if(!_disable_training){  
      /*
       * Record the operation stats to create the page relation graph
       */
      assert(node_dest < _x_dim * _y_dim);
      assert(node_src1 < _x_dim * _y_dim);
      assert(node_src2 < _x_dim * _y_dim);

      //int tab_curr_index = _g->get_index(dest); //current subgraph index, -1 if not avaiable
      //int tab_s1_index = _g->get_index(src1); //s1 subgraph index, -1 if not avaiable
      //int tab_s2_index = _g->get_index(src2); //s2 subgraph index, -1 if not avaiable

      //_g->set_root_info(tab_curr_index, pid, node_dest, dest>>11);
      //_g->set_root_info(tab_s1_index, pid, node_src1, src1>>11);
      //_g->set_root_info(tab_s2_index, pid, node_src2, src2>>11);
      
      record_page_acc(dest>>11, pid, node_dest, op_id); 
      record_page_acc(src1>>11, pid, node_src1, op_id); 
      record_page_acc(src2>>11, pid, node_src2, op_id); 

      //_g->update_table(tab_curr_index, tab_s1_index, node_src1, tab_s2_index, node_src2);//counters update
      //_g->update_table(tab_s1_index, tab_curr_index, node_dest, tab_s2_index, node_src2);//counters update
      //_g->update_table(tab_s2_index, tab_curr_index, node_dest, tab_s1_index, node_src1);//counters update

      //_g->graph_display();
    }
    /***** Recording End *****/
  }
  else{
    // cout<<"can't make entry ... @cycle"<<global_clock<<" for pid: "<<pid<<endl;
    success = false;//stall the processor
  }

  //cout<<"[VMAP] processed one PIM operation"<<endl;
#ifdef DEBUG_PRINT_VMAP
  //fprintf(debugfile,"%lu | pim_operation: mcq[%d].size: %lu, packet %lu, src %d, dest %d, pim_src1 %d pim_src2 %d\n",
  //    global_clock, port, _mcq[port].size(), l_pkt->id, l_pkt->src, dest_node, l_pkt->pim_src1, l_pkt->pim_src2);
#endif
return success;
   
}

bool v_map_use::pim_operation_new(int pid, unsigned long op_id, vm* dest, int idx_dest, vm* src1, int idx_src1, vm* src2,  int idx_src2){
  bool success = false;
  assert(dest != NULL && src1 != NULL && src2 != NULL && "*** accessing null pointer in pim_operation() ***");
  unsigned long l_dest = dest->address_at(idx_dest);
  unsigned long l_src1 = src1->address_at(idx_src1);
  unsigned long l_src2 = src2->address_at(idx_src2);
  return pim_operation_trace(pid, op_id, l_dest, l_src1, l_src2);
}

bool v_map_use::pim_operation(vm* dest, int idx_dest, vm* src1, int idx_src1, vm* src2,  int idx_src2)
{
  bool success = false;
  assert(dest != NULL && src1 != NULL && src2 != NULL && "*** accessing null pointer in pim_operation() ***");
  long addr_dest = dest->address_at(idx_dest);
  long addr_src1 = src1->address_at(idx_src1);
  long addr_src2 = src2->address_at(idx_src2);
  int pid = dest->get_pid();
  
  assert(pid<=num_proc_submission);
  unsigned long long pa_dest = _l_v2p->GetPhyAddr(pid,addr_dest);
  unsigned long long pa_src1 = _l_v2p->GetPhyAddr(pid,addr_src1);
  unsigned long long pa_src2 = _l_v2p->GetPhyAddr(pid,addr_src2);

  assert(pa_dest!=0 && pa_src1!=0 && pa_src2!=0);
   

  int port = get_port(pa_dest);
  cmp_net *l_cmp_net = cmp_net::GetInstance();
  
  if(l_cmp_net->can_make_entry(pid, port)){
    stats_pim_operations++;
    stats_total_memory_acc+=3;
    packet *l_pkt = new packet;
    l_pkt->pid = pid;//recording process id
    //cout<<"pid noted: "<<pid<<endl;
    l_pkt->c_time = global_clock;
    l_pkt->src = rand()%4;//_port[port];
    l_pkt->dest = pa_dest;//this will be converted into node numbers
    //l_pkt->id = _packet_id;
    l_pkt->pa = pa_dest;
    l_pkt->pkt_size = _num_req_flits;
    l_pkt->needs_resp = true;
    l_pkt->rd_req = true;
    l_pkt->pim_operation = true;
    l_pkt->pim_src1 = pa_src1;//this will be converted into node numbers
    l_pkt->pim_src2 = pa_src2;//this wil be converted into node numbers

    l_pkt->pim_src1_pa = pa_src1;//holding physical addresses
    l_pkt->pim_src2_pa = pa_src2;//holding physical addresses
    l_pkt->dest_pa = pa_dest;//holding physical addresses
    l_pkt->dest_va = addr_dest;
    l_pkt->src1_va = addr_src1;
    l_pkt->src2_va = addr_src2;
    //cout<<" "<<l_pkt->id<<" @vmap:: pa_src1: "<<l_pkt->pim_src1<<" pa_src2: "<<l_pkt->pim_src2<<endl;
    //_packet_id++;
    stats_total_pkts++;
    stats_total_memory_acc++;
 
    l_cmp_net->make_entry(pid, port, l_pkt);
    success = true;
  }
  else{
    success = false;
  }
#ifdef DEBUG_PRINT_VMAP
  //fprintf(debugfile,"%lu | pim_operation: mcq[%d].size: %lu, packet %lu, src %d, dest %d, pim_src1 %d pim_src2 %d\n",
  //    global_clock, port, _mcq[port].size(), l_pkt->id, l_pkt->src, dest_node, l_pkt->pim_src1, l_pkt->pim_src2);
#endif
return success;
}

int v_map_use::get_port(unsigned long dest_pa)
{
  //cout<<hex<<" "<<((dest_pa>>12 & 0x0fff) )<<" "<<_port[((dest_pa>>12 & 0x0fff) ) % _num_port]<<endl;
  //return ((dest_pa>>11 & 0x0fff)) % _num_port;//_port[((dest_pa>>12 & 0x0fff)) % _num_port];
  return rand() % _num_port;//_port[((dest_pa>>12 & 0x0fff)) % _num_port];
}

//void v_map_use::create_stats_vars(){
//  g_stats.create_stats("total_memory_acc");
//  g_stats.create_stats("total_non_memory_inst");
//}
//
void v_map_use::print_stats(string benchname)
{
  //l_v2p.print_stats(this->process_name);
// 
//  g_stats.update("total_memory_acc",stats_total_memory_acc);
//  g_stats.update("total_non_memory_inst",stats_total_non_memory_inst);
//  
//  g_stats.display_stats(benchname + ".stats");
}

void v_map_use::clear_memory(int pid){
  _l_v2p->FreeAllPageFrames();
  tot_rollovers_apps[pid]++;
  bool rollover = true;
  for(int i=0; i<num_proc_submission; i++){
    if(tot_rollovers_apps[i] <= stats_tot_rollovers){
      rollover = false;
      break;
    }
  }
  if(rollover==true){ 
    set_done();
    stats_tot_rollovers++;
    cout<<"\n[VMAP]  ALL PROCESS ROLLED OVER (at least) == == == "<<stats_tot_rollovers<<" TIMES"<<endl;
  }
  if(!_disable_training){  
    _g->ClearStructures();
    possible_migration_cand_list[pid].clear();
  }
  
  if(rollover == true){
    cout<<"\n[VMAP] --- print stats for rollover = true"<<endl;
    print_stats_for_rollover = true;
  }
}

unsigned long v_map_use::count_active_pages() {
  return _l_v2p->CountActivePages();
}
