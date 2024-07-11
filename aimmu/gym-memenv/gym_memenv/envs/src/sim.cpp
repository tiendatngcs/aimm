#include "sim.hpp"
#include <bits/stdc++.h>

const int EPOCH=10000;
unsigned long PrevGlobalClock = 0;
unsigned long LastEpochCycle = 0;
long num_epoch = 1;
unsigned long call_learning = 0;
float training_rate = 0.01;
/*************************************************/
/*              Extern variables*/
/*************************************************/
double injection_rate = -1.0;
bool multi_program = false;
bool is_done = false;
int num_proc_finished = 0;
int num_proc_submission = 0;
unsigned long global_clock = 0;
FILE * debugfile = fopen("debug.log","w");
stats<long double>g_stats; //global stats for whole tool
stats<vector<long double> >g_stats_vec; //global stats for whole tool
int watch_packet = -1;
bool watch_all_packets = true;
int router_stages=3;
int mem_acc_lat=100;
unsigned long inflight_ntw_pkt=0;
unsigned long inflight_mem_pkt=0;
unsigned long gpacket_id=0;
int req_pkt_size=-1;
int resp_pkt_size=-1;
int link_width=-1;
bool page_miss_sig = false;
int perceptron_id = 0;
int reward = 0;
int missed_pid = -1;//pid which got a miss and now calling agent for frame decision
unsigned long missed_vaddr = 0;//virtual address of the missed page ... the page number!!
unsigned long vmap_wrong_cube_incident = 0;
bool print_stats_for_rollover = false;
bool print_stats_at_end_of_trace = false;
bool increase_training_rate = false;
bool decrease_training_rate = false;
bool flip_dest_node = false;
unsigned long stats_possible_cand_list_empty = 0;
unsigned long stats_roundtrip_pimq_size = 0;
unsigned long stats_l2_cache_hits = 0;
unsigned long stats_l2_cache_access = 0;
unsigned long stats_l2_cache_occupancy = 0;
int pg_hist_len = 0;

vector<vector<int> >adj; 
vector<vector<long double> >trace_buffer;
vector<vector<unsigned long> >page_fault_state;
vector<vector<unsigned long> >sys_migration_list;
vector<pair<unsigned long, unsigned long> >hop_count_monitor;
vector<pair<unsigned long, unsigned long> >sys_hop_count_monitor;
vector<double>avg_hop_monitor;
//vector<vector<tuple<double, double, double> > >hop_profile;
vector<pair<unsigned long, unsigned long> >migrations_monitor;
vector<int>sleep_process;
vector<int>migration_completion_check;
//state counters
vector<vector<long double> >num_frame_occ_percube_per_app;//occupancy of per cube per app
vector<vector<long double> >gap_of_acc_per_app;//frequency vector per cube per app  
vector<tuple<double, double, double> > input_for_node2vec;
vector<vector<tuple<int, int, unsigned long> > >possible_migration_cand_list;
vector<map<unsigned long, int> >compute_migration_cand_list;
vector<double>app_wise_opc;

vector<map<unsigned long, vector<unsigned long> > >op_lat_breakdown_tracker;
vector<long double> stats_pim_tab_full_hist;//it will keep track of pim tab full incident
vector<long double> stats_pim_full_rate_hist;//it will keep track of pim tab full incident
vector<long double> stats_pim_dest_hist;
vector<long double> stats_tot_row_hit_hist;
vector<long double> stats_tot_row_miss_hist;
vector<long double> stats_row_buffer_missrate_hist;
vector<long double> stats_mcq_occ_hist;
DataCollectHook * dch_mig_freq = new DataCollectHook();//TODO: need to set the values later
DataCollectHook * dch_pimtab = new DataCollectHook();
DataCollectHook * dch_active_pages = new DataCollectHook();
unordered_map<unsigned long, unsigned long>active_pages_epochwise;
unordered_map<unsigned long, unsigned long>page_access_count_epochwise;
vector<map<unsigned long, PageInfo *> >page_info_map;//stores all the data regarding pages
unordered_map<unsigned long, unsigned long>page_info_cache;//only meta data for replacment policy

////////////////////// STATS VARIABLES INITIALIZATION //////////////////////
long double stats_total_memory_acc=0;
long double stats_total_pkts=0;
long double stats_retired_mem_acc=0;
long double stats_retired_mem_acc_with_migr=0;
/*
 * MC
 */
long double stats_reading_from_mcq=0;
long double stats_avg_mcq_queuing_delay=0;
long double stats_tot_mcq_queuing_delay=0;
long double stats_avg_ipc=0;
long double stats_total_instructions_exec=0;
/*
 * VMAP
 */
long double stats_total_non_memory_inst = 0.0;
/*
 * M_NET
 */
long double stats_total_hops=0;
long double stats_total_retry_cycles=0;
long double stats_retry_ntw_cycles_per_pkt=0;
long double stats_mem_queuing_delay=0;
long double stats_mem_queuing_delay_per_acc=0;
long double stats_total_mem_s2ret_lat=0;
long double stats_total_DRAM_acc_lat=0;
long double stats_avg_mem_acc_lat=0;
long double stats_avg_DRAM_acc_lat=0;
long double stats_total_network_lat=0;
long double stats_avg_pkt_lat=0;
long double stats_avg_num_hops=0;
long double stats_total_resp_pkts=0;
long double stats_failed_to_create_resp_pkts=0;
long double stats_mem_row_hit=0;
long double stats_mem_row_miss=0;
long double stats_retired_pkt=0;
/*
 * V2P
 */
long double stats_page_hit=0;
long double stats_page_fault=0;
long double stats_page_fault_conflict=0;
long double stats_page_miss_make_entry=0;
long double stats_page_replacement=0;
long double stats_v2p_mem_acc=0;
long double stats_num_pages_swapped_out=0;
long double stats_swapped_out_and_in_again=0;
long double stats_total_L0_tables_created=0;
long double stats_total_L1_tables_created=0;
long double stats_total_L2_tables_created=0;
long double stats_total_L3_tables_created=0;
long double stats_tlb_hit=0;
long double stats_tlb_miss=0;
long double stats_tlb_repl=0;
unsigned long stats_total_active_pages = 0;
unsigned long stats_num_pages_accessed_last_epoch = 0;

/*
 * Memory
 */
long double stats_memory_requested=0;
long double stats_memory_allocated=0;
long double stats_frames_used=0;


long double stats_pim_hops = 0;
long double stats_pim_operations = 0;
long double stats_pim_acc_lat = 0;
long double stats_pim_op_ret = 0;
long double rlmmu_pim_operations_cycles=0;
long double rlmmu_pim_operations=0;
unsigned long num_pim_op_in_stream = 100000;
unsigned long stats_trace_buffer_fill = 0;
unsigned long stats_pkt_not_ready_to_retire = 0;
unsigned long stats_mem_cube_empty = 0;
unsigned long stats_mem_cube_size_zero = 0;
unsigned long stats_pim_tab_full = 0;
long double stats_free_frames_in_list = 0;
long double stats_numofpg_acc_after_mig = 0;//number of migrated pages accessed
long double stats_num_of_acc_to_mig_pg = 0;//total number of accesses to all the migrated pages
long double stats_tot_pg_migrated=0;//total number of pages migrated
unsigned long stats_tot_rollovers = 0;//counting total number of rollovers
unsigned long stats_processor_stall_cycles = 0;
unsigned long stats_total_migr_q_size = 0;//migration queue size
unsigned long stats_can_not_inject_pim_packet = 0;
unsigned long stats_has_no_pim_packet = 0;
unsigned long stats_can_not_inject_response_packet = 0;
unsigned long stats_has_no_response_packet = 0;
unsigned stats_tot_pim_tab_in_use = 0;//hold the max number of pim table acc in one cycle
unsigned long forward_queue_size =0;
unsigned long pim_req_q_size = 0;
unsigned long stats_mem_cube_size = 0;
unsigned long stats_resp_queue_size = 0;
unsigned long stats_retiring_forward_pkt = 0;
unsigned long stats_forward_pkt_created = 0;
unsigned long stats_num_info_pkts = 0;
/*
 * Vector Stats
 */
vector<long double> stats_tot_sleep_time;//counts cycles
vector<long double> stats_migration_not_complete_time;//counts cycles
vector<long double> stats_op_latency_breakdown;//vector stats
vector<long double> stats_types_of_pkts_retire;//vector stats
vector<unsigned long>action_hist;
vector<unsigned long>tot_rollovers_apps;//counting total number of rollovers
vector<long double> stats_retired_mem_acc_apps;

//////////////////////END STATS VARIABLES INITIALIZATION //////////////////////
/*************************************************/
/*              Extern variables END */
/*************************************************/
long EPOCH_NUM = 0;//keeping track of number of epochs
long CALLED_AGENT = 0;

Sim::Sim(const Configuration& config)
{
  multi_program = config.GetInt("multi_program")>0;
  /*************************************************/
  /*              Build memory network             */
  /*************************************************/
  _mem_size = config.GetInt("mem_size");
  _page_size = config.GetInt("page_size");
  _x_dim = config.GetInt("x_dim");
  _y_dim = config.GetInt("y_dim");
  _size_per_node = _mem_size / (double)(_x_dim * _y_dim);
  _topology = config.GetStr("topology");
  _r_algo = config.GetStr("r_algo");
  _num_port = config.GetIntArray("port_positions").size();
  _port = config.GetIntArray("port_positions"); 
  //_num_proc = config.GetInt("num_proc");  // depricated
  _trace_folder = config.GetStr("trace_folder");  
  _disable_migration = (config.GetInt("dis_migr")>=1);  
  _disable_training = (config.GetInt("dis_training")>=1); 
  _periodic_break = (config.GetInt("periodic_break")>=1);
  _max_hop_avg = config.GetInt("max_hop_avg");

  string sim_type = config.GetStr("sim_type");
  string runfile = config.GetStr("runfile");

  _bench = config.GetStr("stats_file");
  _max_tot_mem_acc_for_sim = config.GetAddr("max_tot_mem_acc");
  _no_roll_over = (config.GetInt("no_roll_over")>0);
  _max_num_of_rollovers = config.GetAddr("max_tot_rollovers");

  /*************************************************/
  /*              Time Series Plot --> graphs      */
  /*************************************************/
  string opc_avg_folder = config.GetStr("stats_file");// + "/" + config.GetStr("adaptive_epoch_folder");
  string opc_avg_series = config.GetStr("opc_avg_series");
  int opc_avg_freq = config.GetInt("opc_avg_freq");
  
  string hop_avg_folder = config.GetStr("stats_file");// + "/" + config.GetStr("hop_avg_folder");  
  string hop_avg_series = config.GetStr("hop_avg_series");
  int hop_avg_freq = config.GetInt("hop_avg_freq");
  
  string mig_freq_avg_folder = config.GetStr("stats_file");// + "/" + config.GetStr("hop_avg_folder");  
  string mig_freq_avg_series = config.GetStr("mig_freq_avg_series");
  int mig_freq_avg_freq = config.GetInt("mig_freq_avg_freq");

  string page_profile_info_folder_path = config.GetStr("pg_profile_folder");
    
  pg_hist_len = config.GetInt("pg_hist_len");
  
  _warmup_cycle = config.GetInt("warmup_cycle");//for warming up the system before training starts
  _injection_rate = config.GetInt("injection_rate");//rate at which we shedule pim operations in %
  
  stats_mcq_occ_hist.resize(_x_dim*_y_dim);  
  adj.resize(_x_dim*_y_dim*_x_dim*_y_dim);//global adjacency matrix
  action_hist.resize(256);
  
  read_runlist(runfile);
  vector<string> process_names = ProcessFactory::GetInstance()->GetProgramNames();
  
  possible_migration_cand_list.resize(_x_dim*_y_dim);
  compute_migration_cand_list.resize(_x_dim*_y_dim);

  cout << "Ports are: ";
  for (int i = 0; i < _port.size(); i++)
    cout << dec << " "<< _port[i];
  cout << endl;
  watch_packet = config.GetInt("watch_packet");
  if (watch_packet == -911) {
    watch_all_packets = true;
  }


  cout << "mem_size: " << _mem_size << "\npage_size: "
    << _page_size << "\nsize_per_node: " << _size_per_node
    << "\nx_dim: " << _x_dim << "\ny_dim: " << _y_dim << endl;

  fprintf(debugfile,"\n===== Build Memory Network =====\n");
  fprintf(debugfile,"Memory Size: %d\n", _mem_size);
  fprintf(debugfile,"Page Size: %d\n", _page_size);
  fprintf(debugfile,"Memory size per node: %lf\n", _size_per_node);
  fprintf(debugfile,"X Dim: %d\n", _x_dim);
  fprintf(debugfile,"Y Dim: %d\n",_y_dim);
  fprintf(debugfile,"Topology: %s\n", _topology.c_str());
  fprintf(debugfile,"Routing Algo: %s\n", _r_algo.c_str());
  fprintf(debugfile,"Number of memory ports: %d\n", _num_port);

  _total_processes = 0;

  for(int i = 0; i < process_names.size(); i++) {
    string proc_name = process_names[i];
    cout<<" "<<proc_name<<" ========================== launch cycle: "<<proc_runlist[proc_name]<<endl;
    int launch_cycle = stoi(proc_runlist[proc_name]);
    if (launch_cycle >= 0) {
      cout << "Process: " << proc_name << " will be launched @" << launch_cycle << " cycle(s)" << endl;
      Process * proc = ProcessFactory::GetInstance()->CreateProcess(config, process_names[i]);
      proc->SetLaunchCycle(launch_cycle);
      _total_processes++;
      num_proc_submission++;
      make_entry_process_queue(proc);
    }
  }
  
  tot_rollovers_apps.resize(_total_processes);
  stats_retired_mem_acc_apps.resize(_total_processes);
  page_info_map.resize(_total_processes);  
  op_lat_breakdown_tracker.resize(_total_processes);
  app_wise_opc.resize(_total_processes);
  //for(int i=0; i< _total_processes; i++){
  //  page_info_map.push_back(map<unsigned long, PageInfo *>());
  //}

  _dch_1 = new DataCollectHook(opc_avg_folder, opc_avg_series, opc_avg_freq);
  _dch_2 = new DataCollectHook(hop_avg_folder, hop_avg_series, hop_avg_freq);
  _dch_3 = new DataCollectHook(hop_avg_folder, "migr_count_", hop_avg_freq);
  dch_mig_freq->set_params(mig_freq_avg_folder, mig_freq_avg_series, mig_freq_avg_freq);
  dch_pimtab->set_params(hop_avg_folder, "pim_tab_occ_count_", 10000);
  dch_active_pages->set_params(hop_avg_folder, "active_pages_", 10000);
  
  _a_epoch = new AdaptiveEpoch(config); 
  _mc = MemoryController::GetInstance(config);
  _dma = DirectMemoryAccess::GetInstance(); 
  _mmu = MemoryManagementUnit::GetInstance(config);
  _m_net = MemoryNetwork::GetInstance(config);
  _c_net = cmp_net::GetInstance(); 

  _mc->set_up(config); 
  _dma->set_up(config);
  _mmu->set_up();
  _c_net->setup_cmp(config);
  
  migration_completion_check.resize(num_proc_submission);
  _num_ops_per_proc.resize(_total_processes);
  sleep_process.resize(_total_processes); 

  stats_tot_sleep_time.resize(_total_processes);//per process stats
  stats_migration_not_complete_time.resize(_total_processes);//per process stats 
  stats_op_latency_breakdown.resize(6);//vector stats
  stats_types_of_pkts_retire.resize(3);//vector stats

  //state param
  num_frame_occ_percube_per_app.resize(_total_processes);//occupancy of per cube per app
  gap_of_acc_per_app.resize(_total_processes);//frequency vector per cube per app  
  
  for(int i=0; i<_total_processes; i++){
    num_frame_occ_percube_per_app[i].resize(_x_dim*_y_dim);//Sys Param: each process will keep a list of cubes
    gap_of_acc_per_app[i].resize(_x_dim*_y_dim);//App Param: each process will keep a list of cubes
  }

  page_fault_state.resize(_total_processes);
  
  sys_hop_count_monitor.resize(_total_processes);
  hop_count_monitor.resize(_total_processes);
  avg_hop_monitor.resize(_total_processes);
  //hop_profile.resize(_total_processes);
  stats_pim_tab_full_hist.resize(_x_dim*_y_dim);//it will keep track of pim tab full incident
  stats_pim_full_rate_hist.resize(_x_dim*_y_dim);//it will keep track of pim tab full incident
  stats_pim_dest_hist.resize(_x_dim*_y_dim);
  stats_tot_row_hit_hist.resize(_x_dim*_y_dim);
  stats_tot_row_miss_hist.resize(_x_dim*_y_dim);
  stats_row_buffer_missrate_hist.resize(_x_dim*_y_dim);

  create_processors(_total_processes);//returns an array of processes

  for(int i=0;i<_total_processes;i++){//map a process to a processor
    cores[i].pt = process_queue.front();
    process_queue.pop_front();
    page_fault_state[i].resize(_x_dim*_y_dim+1);
    
    int pid = cores[i].pt->get_process_id();
    string proc_name = cores[i].pt->get_process_name();
    string profile_path = page_profile_info_folder_path + proc_name + ".pg";
    load_pg_usage_map(profile_path, pid, pg_hist_len);//loading page usage info from profiled data
    
    for(int j=0;j<_x_dim*_y_dim+1;j++){
      page_fault_state[i][j]=0;
    }
  }
  
  for(int i=0; i<_total_processes; i++){
    sys_hop_count_monitor[i].first=0;
    sys_hop_count_monitor[i].second=0;
    hop_count_monitor[i].first=0;
    hop_count_monitor[i].second=0;
    avg_hop_monitor[i] = -1.0;
    sleep_process[i] = 0;
  }
  create_stats_vars();
  cout<<"[SIM] ------ Initilization Done in SIM ------ "<<endl;

}

Sim::~Sim(){}

/*
 * simulate the whole pipeline for that process on the processor
 * cmp side network is not cycle accurate. The latency for reaching memory controller
 * must be added in the packet while creation. After that time the packet must be added
 * in the mcq. So for each processor first we have to check whether there is anything to
 * to push in mcq. Then it will see if it has any new request. Basically we are integrating
 * NI vcs with the network latency on the CMP side. Hence the buffer should be as large as
 * multiple of number of hops and total buffer in each of the nodes on the way.
 */

vector<double> Sim::run_gen(long epoch_length){
  // cout << "Call run gen" << endl;
  long num_op = 0;
  long mig_not_completed = 0;
  long sleep_time = 0;
  long mig_num = 0;
  long elapsed_clk = 0;
  int pid = -1;
  do{
    // cout << "Cycle loop" << endl;
    /*
     * Simulating processor pipeline for all the cores for one cycle
     */
    //#pragma omp parallel for num_threads(8) collapse(2)
    for(int p = 0; p < _total_processes; p++) {
      bool sleep = false;
      pid =  cores[p].pt->get_process_id();
      //cout<<"[SIM] core under processing cycle: "<<p<<endl;
      /*
       * process sleeps on a page fault and 
       * wakes up, when the fault is handled
       */
      if(cores[p]._sleep_timer < 0){
          if(sleep_process[pid]==1){
            sleep = true;
            cores[p]._sleep_timer = 200;
          }
      }
      else{
        if(cores[p]._sleep_timer>=0){
          cores[p]._sleep_timer--;//final value will be "-1"
          if(cores[p]._sleep_timer == 0){//wakeup the process
            sleep_process[pid] = 0;
            //_mmu->clear_mig_queue(pid);
          }
        }
      }
      /*
       * If the process is awake, then simulate one cycle for that process
       */  
      if(!sleep && migration_completion_check[p]==0){
        assert(cores[p].pt!=nullptr);
        if(cores[p].pt!=nullptr && cores[p].pt->ReadyToRun(global_clock) && rand()%_injection_rate==0){
          // cout<<"[SIM] going to execute one cycle ... for: "<<cores[p].pt->get_process_name()<<endl;
          if(cores[p].pt->OneCycle()){//true means scheduled
            num_op++;
            _num_ops_per_proc[p]++;
            // cout<<"[SIM] one cycle done for process: "<<p<<endl;
          }
          else{
            cout<<"[SIM] stall time ... "<<endl;
            stats_processor_stall_cycles++;//counting processor stall cycles
          }
        }
        else{
          cout<<"[SIM] core is not ready or null ..."<<endl;
        }
      }
      else if(migration_completion_check[p]!=0){
        mig_not_completed++;
        cout<<"Migration not complete ... "<<mig_not_completed<<endl;
        stats_migration_not_complete_time[p]++;
        mig_num = migration_completion_check[p];
      }
      else if(sleep){
        sleep_time++;
        stats_tot_sleep_time[p]++;
        cout<<"Process "<<pid<<" sleeing ... "<<stats_tot_sleep_time[p]<<endl;
      }
    }
    // cout<<"[SIM] cycle done for the processors ..."<<endl;
    /* 
     * One step for the cmp network
     */ 
    _c_net->step();
    /*
     * one step for Memory Management Unit
     */
    is_done = is_done || _mmu->step();//returns true when the application ends
    // cout<<"[SIM] one step mmu doen ..."<<endl; 
    /*
     * one step for Direct Memory Access
     */
    _dma->step();
    // cout<<"[SIM] one step dma doen ..."<<endl; 
    /*
     * one step for Memory Controller and then
     */
    _mc->step();
    // cout<<"[SIM] one step mc doen ..."<<endl; 
    /*
     * One step in the Memory Network
     */
    _m_net->step();
    // cout<<"[SIM] one step m_net doen ..."<<endl; 

    /*
     * Global clock should be incremented only in one place
     */
    global_clock++;//this must be the only place to increment clock
    elapsed_clk++;
    assert(PrevGlobalClock == (global_clock - 1));//to make sure that the 
                                                  //not being tampered !!
    PrevGlobalClock = global_clock;
    
    if(global_clock%EPOCH==0){
        print_status(num_op, sleep_time, mig_not_completed, mig_num);
    }
   
    
    if(_max_tot_mem_acc_for_sim <= stats_total_memory_acc){
        cout<<"\n*** Maximum number of operations simulated ****\n"<<endl;
        cout<<"\n[SIM] END OF SIMULATION \n"<<endl;
        //drain_remaining_packets();
        print_stats(_bench);
        if(!multi_program)
          collect_individual_stats(pid);
        assert(0 && "We should drain here ...");
        exit(0);//destructors won't be invoked //FIXIT //TODO
    }
    
    if(_max_num_of_rollovers <= stats_tot_rollovers){
        cout<<"\n*** Maximum number of RollOvers simulated ****\n"<<endl;
        cout<<"\n[SIM] END OF SIMULATION \n"<<endl;
        //drain_remaining_packets();
        print_stats(_bench);
        if(!multi_program)
          collect_individual_stats(pid);
        //TODO: Need to fixit, even though I am not using destructor for any critical puposes!!!! 
        assert(0 && "We should drain here ...");
        exit(0);//destructors won't be invoked //FIXIT //TODO
    }
    
    if(!_disable_training){ 
      if(elapsed_clk > epoch_length){
        num_epoch++;
        if(_warmup_cycle < global_clock){
          break;
        }
      }
      //TODO:: need to make it application wise using pid
      if(!multi_program && possible_migration_cand_list[0].size()>0){//need to make it application-wise and less frequent 
        if(increase_training_rate && training_rate < 0.01){
          training_rate += 0.002;
          increase_training_rate = false;
        }
        if(decrease_training_rate && training_rate >= 0.004){
          training_rate -= 0.002;
          decrease_training_rate = false;
        }

        if(((call_learning*1.0)/global_clock) < training_rate){
          if(_warmup_cycle < global_clock){
            break;
          }
        }
      }
      if(is_done && _warmup_cycle < global_clock){
        cout<<"[SIM] breaking as it is done ... "<<endl;
        break;
      }
    } else if (_periodic_break) {
      if(elapsed_clk >= epoch_length){
        num_epoch++;
        break;
      }
      if(print_stats_at_end_of_trace) {
        collect_individual_stats(pid);
      }
    }
    else{
      // runs until the end
      if(print_stats_for_rollover){
          collect_individual_stats(pid);
        cout<<"[SIM] Printing stats after rollover ..."<<endl;
        print_stats_for_rollover = false;
        //print_stats(_bench);
        //collect_individual_stats(pid);
        if(_no_roll_over){
          //drain_remaining_packets();
          cout<<"\n[SIM] END OF SIMULATION (Trace Fnished | No rollover)\n"<<endl;
          print_stats(_bench);
          assert(0 && "We should drain here ...");
          exit(0);//destructors won't be invoked //FIXIT //TODO
        }
      }
    }
    // cout << "Ran one step" << endl;
  }while(num_proc_finished < _total_processes && global_clock<100000000000);//processes will be done !!!
  
  if (!_disable_training) call_learning++; 
  // cout<<"========================================================== returning to agent "<<endl;
  return app_wise_opc;
}


void Sim::collect_individual_stats(int pid){
  //collecting per page migration stats as log
  if(!_disable_training){ 
    for(auto it=page_info_map[pid].begin(); it!=page_info_map[pid].end(); ++it){
      string pg = to_string(it->first);
      string migr_count = to_string(it->second->get_migr_cnt());
      string collect_migr = pg + "|" + migr_count;
      _dch_3->collect(collect_migr);
    }
  }
  _dch_2->print();
  _dch_1->print();
  dch_mig_freq->print();
  dch_pimtab->print();
  dch_active_pages->print();
}

void Sim::_prep_epoch_params(long elapsed_clk, long num_op){
  //hop avg, page miss rate, op per cycle, avg training cost
  double tot_hop_avg = 0;
  unsigned long num_pkts = 0; 
  unsigned long num_hops = 0;
  double hop_avg = 0;
  double pg_miss_rate = 0;
  double op_per_cycle = 0;
  double avg_training_cost = 0; 
  
  for(int i=0; i<_total_processes; i++){
    num_pkts = hop_count_monitor[i].first;
    num_hops = hop_count_monitor[i].second;
    if(num_hops > 0 && num_pkts > 0){
      tot_hop_avg += ((num_hops*1.0)/_max_hop_avg)/num_pkts;
      //cout<<" "<<num_hops<<" "<<num_pkts<<endl;
      //cout<<"tot_hop_avg: "<<tot_hop_avg<<endl;
      assert(tot_hop_avg >= 0);
    }
  }
  
  hop_avg = (tot_hop_avg*1.0) / _total_processes;//param-1
  
  if(stats_total_memory_acc > 0){
    pg_miss_rate = (stats_page_fault*1.0) / stats_total_memory_acc;//param-2
  }
  if(elapsed_clk > 0){
    op_per_cycle = (num_op*1.0) / elapsed_clk;//param-3
  }
  if(input_for_node2vec.size()){
    avg_training_cost = (stats_page_fault*1.0) / input_for_node2vec.size();//param-4 //TODO //FIXIT
  }

  assert(hop_avg >= 0 && hop_avg <= 1);
  assert(pg_miss_rate >= 0 && pg_miss_rate <= 1);
  assert(op_per_cycle >= 0 && op_per_cycle <= 1);
  assert(tot_hop_avg >= 0 && tot_hop_avg <= 1);

  _a_epoch_params.assign({hop_avg, pg_miss_rate, op_per_cycle, avg_training_cost});
  
  _a_epoch->get_input(global_clock, _a_epoch_params); 
}


void Sim::print_status(long num_op, long sleep_time, long mig_not_completed, long mig_num){
  unsigned long total_nmem_inst = stats_total_non_memory_inst;
  unsigned long total_mem_acc = stats_total_memory_acc;
  cout << "\nclk: " << dec << global_clock
    << " NumOP: "<<num_op
    << " PimOpRet: "<<stats_pim_op_ret 
    << " SleepTime: "<<sleep_time
    << " MigIncompTime: "<<mig_not_completed<<" ("<<mig_num<<")"
    << " MigrQSize: "<<stats_total_migr_q_size
    << " Mem:" << total_mem_acc
    << " RetMemAcc: "<<stats_retired_mem_acc
    << " failedresp: "<<stats_failed_to_create_resp_pkts
    << " Retiredpkt: "<<stats_retired_pkt
    //<< " pkt_not_ready_retire: "<<stats_pkt_not_ready_to_retire
    << " m_cube_empry: "<<stats_mem_cube_empty
    << " m_cube_zero: "<<stats_mem_cube_size_zero
    << " pim_tab_full: "<<stats_pim_tab_full
    << " hops: "<<stats_total_hops
    << " retry_cyc: "<<stats_total_retry_cycles
    << " pg miss: "<<stats_frames_used 
    << " tr_buff_fill: "<<stats_trace_buffer_fill
    << " freelist entries: "<<stats_free_frames_in_list   
    << " proc stall: "<<stats_processor_stall_cycles
    << " can't inject pim: "<<stats_can_not_inject_pim_packet
    << " has no pim: "<<stats_has_no_pim_packet 
    << " can't inject resp: "<<stats_can_not_inject_response_packet
    << " has no resp: "<<stats_has_no_response_packet
    //<< " possible_migr_size: "<<possible_migration_cand_list.size()
    << " compute_migr_size: "<<compute_migration_cand_list.size()
    << " call_learning_r: "<<(call_learning*1.0)/global_clock 
    << " possible_cand_list_empty: "<<(stats_possible_cand_list_empty*1.0)/call_learning
    << " pim_op_lat: "<<stats_pim_acc_lat/stats_pim_op_ret
    << " injection rate: "<<injection_rate
    << " forward_q_size: "<<forward_queue_size
    << " pim_req_q_size: "<<pim_req_q_size
    << " mem_cube_size: "<<stats_mem_cube_size
    << " resp_queue_size: "<<stats_resp_queue_size
    << " inflight forward_pkt: "<<stats_forward_pkt_created - stats_retiring_forward_pkt 
    << " inflight pkts: "<<stats_total_pkts - stats_retired_pkt 
    << " pim_r-tripq_size: "<<stats_roundtrip_pimq_size
    << " l2cache_hitrate: "<<(stats_l2_cache_hits*1.0)/stats_l2_cache_access
    << " l2_access: "<<stats_l2_cache_access
    << " l2_occupancy: " <<stats_l2_cache_occupancy
    << " rollovers: "<<stats_tot_rollovers<<endl;   

  cout<<"action hist: [";
  for(int i=0; i < action_hist.size(); i++){
    if(action_hist[i]==0) break; 
    cout<<(action_hist[i]*1.0)/call_learning<<", ";
  }
  cout<<"]"<<endl;

  //#pragma omp parallel for num_threads(8) collapse(2)
  for(int i=0; i<_total_processes; i++){
    unsigned long sys_num_pkts = sys_hop_count_monitor[i].first;
    unsigned long sys_num_hops = sys_hop_count_monitor[i].second;
    unsigned long num_pkts = hop_count_monitor[i].first;
    unsigned long num_hops = hop_count_monitor[i].second;
    if(num_pkts>0){
      double episode_avg = (num_hops*1.0)/num_pkts;
      double sys_episode_avg = (sys_num_hops*1.0)/sys_num_pkts;
      if(avg_hop_monitor[i] == -1.0){
        avg_hop_monitor[i] = episode_avg;
      }
      else{
        avg_hop_monitor[i] = (avg_hop_monitor[i] + episode_avg) / 2;//running average
      }
      tuple<double,double,double> temp = make_tuple(sys_episode_avg, avg_hop_monitor[i], episode_avg);
      unsigned long cycle_taken = global_clock - LastEpochCycle;
      //double ipc = (_num_ops_per_proc[i]*1.0) / cycle_taken;
      //double ipc = (_num_ops_per_proc[i]*1.0) / global_clock;
      double ipc = (stats_retired_mem_acc_apps[i]*1.0) / global_clock;
      app_wise_opc[i] = ipc;
      //double ipc = stats_avg_ipc;
      cout<<"["<<i<<": "<<" "<<sys_episode_avg<<" | "<<avg_hop_monitor[i]<<" | "<<episode_avg<<" | "<<ipc<<" ] ";
      //hop_profile[i].push_back(temp);  
      LastEpochCycle = global_clock;
      //_num_ops_per_proc[i] = 0;
      if(!multi_program){
        _dch_1->collect(to_string(ipc));
        _dch_2->collect(to_string(sys_episode_avg));
      }
    }
    else{
      cout<<"["<<i<<": 0"<<"] "; 
      //assert(0); 
    }
    hop_count_monitor[i].first = 0;
    hop_count_monitor[i].second = 0;
  }
  cout<<endl;
}

void Sim::drain_remaining_packets()
{
  //while (inflight_ntw_pkt !=0 || inflight_mem_pkt != 0){
  while (stats_total_pkts - stats_retired_pkt > 0){
    _mc->step();
    global_clock++;//this must be the only place to increment clock
    if(global_clock % EPOCH == 0) {
      cout<<"[SIM] "<<dec<<""<<global_clock<<" | Drain reamining: "<<(stats_total_pkts - stats_retired_pkt)
        <<" total pkts: "<<stats_total_pkts<<" retired: "<<stats_retired_pkt<<endl;  
      //cout<<"inflight_mem_pkt: "<<inflight_mem_pkt<<" inflight_ntw_pkt: "<<inflight_ntw_pkt<<endl;
    }
  }
}

void Sim::create_stats_vars(){
  cout<<"[SIM] creating stats variables ..."<<endl;
  rst.create_stats();
}

void Sim::print_stats(string benchname){
  cout<<"[SIM] printing stats ..."<<endl;
  if (_periodic_break) {
    // only run one program at a time
    assert(_total_processes == 1);
    assert(cores[0].pt!=nullptr);
    stats_total_active_pages = cores[0].pt->count_active_pages();
    stats_num_pages_accessed_last_epoch = page_access_count_epochwise.size();
  }

  rst.update_stats();
  //rst.display_stats(benchname + ".stats");
  rst.display_stats(benchname);//used to create the benchmark folder

  if (_periodic_break) page_access_count_epochwise.clear();
}

void Sim::read_runlist(string filename) {
  cout << "[SIM] reading runlist ...from " << filename << endl;
  
  ifstream file(filename.c_str());
  string fline;
  //assert(file!=nullptr && "***[SIM] ERROR: Could not open the runfile ***");

  while(getline(file, fline)){
    stringstream line(fline);
    string proc, cycle;
    getline(line, proc, ':');
    getline(line, cycle, ':');
    cout<<"proc: "<<proc<<" cycle; "<<cycle<<endl;
    proc_runlist.insert(make_pair(proc, cycle));
  }
  cout<<"[SIM] Finished reading runlist successfully ..."<<endl;
}

void Sim::load_pg_usage_map(string file_path, int pid, int hist_len){
  ifstream ifstr(file_path, ifstream::in);
  string::size_type sz = 0;
  cout<<"[SIM] reading MAP from--> filepath = "<<file_path<<endl;
  while(!ifstr.eof()){
    string line = "";
    string delim = ",";
    getline(ifstr, line);
    vector<string> splits;
    string org_line = line;
    int pos = 0;
    string token;
    while((pos = line.find(delim)) != string::npos){
      token = line.substr(0, pos);
      splits.push_back(token);
      line.erase(0, pos + delim.length());
    } 
    splits.push_back(line);
    if(!splits[0].empty() && !splits[1].empty()){
      //cout<<"pgaddr: "<<splits[0]<<" count: "<<splits[1]<<endl;
      unsigned long pg_addr = stoull(splits[0], &sz, 0);
      unsigned long count = stoull(splits[1], &sz, 0);
      auto it = page_info_map[pid].find(pg_addr);
      if(it!=page_info_map[pid].end()){
        it->second->set_num_max_acc(count);
      }
      else{
        PageInfo *pi = new PageInfo();
        pi->set_num_max_acc(count);
        pi->set_history_len(hist_len);
        page_info_map[pid][pg_addr] = pi;
      }
    }
  }
  cout<<"[SIM] finished profile "<<file_path<<endl;
}
