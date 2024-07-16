#ifndef _SIM_HPP_
#define _SIM_HPP_

#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <assert.h>
#include <vector>
#include <map>
#include <math.h>
#include "stats.hpp"
#include "extern_vars.hpp"
#include "config_utils.hpp"
#include "Registrar.hpp"
#include "reg_stats.hpp"
#include "perceptron_ckt.hpp"
#include "data_collect.hpp"
#include <unordered_map>

using namespace std;

/*
 * Processor class: abstract model for a processor
 */
class processors{
  public:
    processors(){
      _core_id = -1;
      _proc_id = -1;
      _vm_allocated = -1.0;
      pt = nullptr;
      _sleep_timer = -1;

    }
    int _core_id;
    int _proc_id;
    int _sleep_timer;
    double _vm_allocated;
    Process *pt;//in general one process per core  // TODO: NEEEDED TO BE DELETED IN THE DESTRUCTOR
};

class Sim
{
  public:
    Sim(const Configuration& config);
    ~Sim();

    /*******************************************/
    /*        Creating cores                   */
    /*******************************************/
    vector<processors>cores; 
    void create_processors(int num){cores.resize(num);}
    
    /*******************************************/
    /*        Creating processes               */
    /*******************************************/
    deque<Process *>process_queue;
    void make_entry_process_queue(Process *p){process_queue.push_back(p);};
    /*************************************************/
    
    /*************************************************/
    /*              Stats Functions                  */
    /*************************************************/
    void print_stats(string benchname);
    void print_status(long, long, long, long);
    void print_last_access(string benchname);
    /*
     * @brief draining remaining packets in the network. It should be called at the end of the simulation.
     */
    void drain_remaining_packets();
    vector<double> run_gen(long);
    void _prep_epoch_params(long, long);
    
    void load_pg_usage_map(string, int, int);//page access info into a global map
    void collect_individual_stats(int);
    void print_page_access_summary(string file_path);

  private:

    /*************************************************/
    /*              Initialization                   */
    /*************************************************/
    void create_stats_vars();
    void read_runlist(string);
    /*
     * @brief creates those many number of objects choosing random range of addresses
     * and put them in shared_obj_list.
     */

    /*************************************************/
    /*              Physical Memory and Page          */
    /*************************************************/
    int _mem_size;
    int _page_size;
    double _size_per_node;
    int _x_dim;
    int _y_dim;

    /*************************************************/
    /*              Memory Network                   */
    /*************************************************/
    string _topology;
    string _r_algo;
    int _num_port;

    int _req_pkt_size;//in bytes
    int _resp_pkt_size;//in bytes
    int _link_width;//in bytes
    int _num_req_flits;
    int _num_write_flits;
    vector<int>_num_ops_per_proc;
    bool _disable_migration; 
    bool _disable_training;
    bool _periodic_break;
    int _max_hop_avg;
    unsigned long _max_tot_mem_acc_for_sim;//maximum operation to simulate
    bool _no_roll_over;
    unsigned long _max_num_of_rollovers; 
    unsigned long _warmup_cycle;//for warming up the system before training starts
    int _injection_rate;//rate at which we shedule pim operations

    vector<int> _port;//holds the ports
    map<string,string>proc_runlist;
    int _num_proc;
    int _total_processes;
    reg_stats rst;
    string _trace_folder;
    vector<double>_a_epoch_params;
    string _bench;
    // string _page_access_count_folder;

    DataCollectHook *_dch_1;//collecting avg ipc
    DataCollectHook *_dch_2;//collecting avg hop
    DataCollectHook *_dch_3;//collecting per page migration

    DirectMemoryAccess *_dma;
    MemoryManagementUnit * _mmu;    
    MemoryController * _mc;
    MemoryNetwork * _m_net;
    cmp_net * _c_net;
    AdaptiveEpoch *_a_epoch;
};
#endif
