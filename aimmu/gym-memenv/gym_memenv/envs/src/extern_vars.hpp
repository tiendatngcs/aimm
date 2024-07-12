//#define DEBUG_PRINT_V2P
//#define DEBUG_PRINT_VMAP
//#define DEBUG_PRINT_MNET

#ifndef EXT_FILE_H
#define EXT_FILE_H

#include "iostream"
#include "fstream"
#include "stats.hpp"
#include <vector>
#include <deque>
#include "data_collect.hpp"
#include <unordered_map>

extern bool multi_program;
extern bool is_done;
extern int num_proc_finished;
extern int num_proc_submission;
extern unsigned long global_clock;
extern FILE *debugfile;
extern int watch_packet;
extern bool watch_all_packets;
extern int router_stages;//in terms of cycles
extern int mem_acc_lat;//in terms of cycles
extern unsigned long inflight_ntw_pkt;//keeps track of in-flight packets in network
extern unsigned long inflight_mem_pkt;//keeps track of in-flight packets in memory
extern unsigned long gpacket_id;
extern int req_pkt_size;
extern int resp_pkt_size;
extern int link_width;
extern bool page_miss_sig;
extern int perceptron_id;
extern bool print_stats_for_rollover;
extern bool print_stats_at_end_of_trace;

extern int reward;
extern int missed_pid;//pid which got a miss and now calling agent for frame decision
extern unsigned long missed_vaddr;//virtual address of the missed page ... the page number!!
extern unsigned long vmap_wrong_cube_incident;
extern double injection_rate;
extern int pg_hist_len;

extern vector<vector<int> >adj; 
extern vector<vector<long double> >trace_buffer;
extern vector<vector<unsigned long> >page_fault_state;
extern vector<vector<unsigned long> >sys_migration_list;
extern vector<pair<unsigned long, unsigned long> >sys_hop_count_monitor;//this tracks episode wise for eachh process
extern vector<pair<unsigned long, unsigned long> >hop_count_monitor;//this tracks episode wise for eachh process
extern vector<double>avg_hop_monitor;
//extern vector<vector<tuple<double,double,double> > >hop_profile;
extern vector<int>sleep_process;
extern vector<int>migration_completion_check;//per process ++ while injection -- in retire
//address, <number of times acc, total acc latency>
extern DataCollectHook * dch_mig_freq; 
extern DataCollectHook * dch_pimtab; 
extern DataCollectHook * dch_active_pages; 
extern vector<map<unsigned long, PageInfo *> >page_info_map;
extern unordered_map<unsigned long, unsigned long>page_info_cache;
/////////////////////// STATS VARIABLES DECLARATIONS ////////////////////////
extern stats<long double>g_stats;
extern stats<vector<long double> >g_stats_vec;

extern long double stats_total_memory_acc;
extern long double stats_total_pkts;
extern long double stats_retired_mem_acc;
extern long double stats_retired_mem_acc_with_migr;
extern long double stats_total_non_memory_inst;
/*
 * MC
 */ 
extern long double stats_reading_from_mcq;
extern long double stats_avg_mcq_queuing_delay;
extern long double stats_tot_mcq_queuing_delay;
extern long double stats_avg_ipc;
extern long double stats_total_instructions_exec;
/*
 * VMAP
 */ 
extern long double stats_total_non_memory_inst;
/*
 * M_NET 
 */ 
extern long double stats_total_hops;
extern long double stats_total_retry_cycles;
extern long double stats_retry_ntw_cycles_per_pkt;
extern long double stats_mem_queuing_delay;
extern long double stats_mem_queuing_delay_per_acc;
extern long double stats_total_mem_s2ret_lat;
extern long double stats_total_DRAM_acc_lat;
extern long double stats_avg_mem_acc_lat;
extern long double stats_avg_DRAM_acc_lat;
extern long double stats_total_network_lat;
extern long double stats_avg_pkt_lat;
extern long double stats_avg_num_hops;
extern long double stats_total_resp_pkts;
extern long double stats_failed_to_create_resp_pkts;
extern long double stats_mem_row_hit;
extern long double stats_mem_row_miss;
extern long double stats_retired_pkt;
/*
 * V2P 
 */ 
extern long double stats_page_hit;
extern long double stats_page_fault;
extern long double stats_page_fault_conflict;
extern long double stats_page_miss_make_entry;
extern long double stats_page_replacement;
extern long double stats_v2p_mem_acc;
extern long double stats_num_pages_swapped_out;
extern long double stats_swapped_out_and_in_again;
extern long double stats_total_L0_tables_created;
extern long double stats_total_L1_tables_created;
extern long double stats_total_L2_tables_created;
extern long double stats_total_L3_tables_created;
extern long double stats_tlb_hit;
extern long double stats_tlb_miss;
extern long double stats_tlb_repl;
extern unsigned long stats_total_active_pages;
extern unsigned long stats_num_pages_accessed_last_epoch;
/*
 * Memory
 */
extern long double stats_frames_used;
extern long double stats_memory_requested;
extern long double stats_memory_allocated;//bytes

//extern vector<pair<string, long> >track_page_frames;//this will track data strcutures 
                                                    //that are using page frames
                                                    // pair<pid_arrayname, #of usage>
                                                    
/*
 * Pim stats
 */
extern long double stats_pim_hops; //total number of hops taken for pim operations
extern long double stats_pim_packets; //total number of pim packets
extern long double stats_pim_operations; //total number of pim operations
extern long double stats_pim_acc_lat;
extern long double stats_pim_op_ret;

extern long double rlmmu_pim_operations;
extern long double rlmmu_pim_operations_cycles;

extern unsigned long num_pim_op_in_stream;
extern unsigned long stats_trace_buffer_fill;
extern unsigned long stats_pkt_not_ready_to_retire;
extern unsigned long stats_mem_cube_empty;
extern unsigned long stats_mem_cube_size_zero;
extern unsigned long stats_pim_tab_full;
extern long double stats_free_frames_in_list;

extern long double stats_numofpg_acc_after_mig;//number of migrated pages accessed
extern long double stats_num_of_acc_to_mig_pg;//total number of accesses to all the migrated pages
extern long double stats_tot_pg_migrated;//total number of pages migrated
extern unsigned long stats_tot_rollovers;//counting total number of rollovers
extern unsigned long stats_total_migr_q_size;//migration queue size

extern unsigned long stats_can_not_inject_pim_packet;
extern unsigned long stats_has_no_pim_packet;

extern unsigned long stats_can_not_inject_response_packet;
extern unsigned long stats_has_no_response_packet;

extern vector<long double> stats_tot_sleep_time;//counts cycles --> vector stats
extern vector<long double> stats_migration_not_complete_time;//counts cycles --> vbector stats
extern vector<unsigned long>action_hist;
extern vector<unsigned long>tot_rollovers_apps;//counting total number of rollovers

extern bool increase_training_rate;
extern bool decrease_training_rate;
extern bool flip_dest_node;

extern unsigned long stats_processor_stall_cycles;

extern unsigned stats_tot_pim_tab_in_use;//hold the max number of pim table acc in one cycle
extern unsigned long forward_queue_size;
extern unsigned long pim_req_q_size;
extern unsigned long stats_mem_cube_size;
extern unsigned long stats_resp_queue_size;
extern unsigned long stats_retiring_forward_pkt; 
extern unsigned long stats_forward_pkt_created; 
/*
 * fomat: <src-node, dest-node, weight>
 */
extern vector<tuple<double, double, double> > input_for_node2vec;
extern vector<vector<tuple<int, int, unsigned long> > >possible_migration_cand_list;
//candidate transferred from possible_migration_cand_list to here
extern vector<map<unsigned long, int> >compute_migration_cand_list;
extern vector<double>app_wise_opc;

extern unordered_map<unsigned long, unsigned long>active_pages_epochwise;
extern unordered_map<unsigned long, unsigned long>page_access_count_epochwise;
extern unordered_map<unsigned long, unsigned long>page_access_count_global;
extern string _page_access_count_folder;

extern unsigned long stats_possible_cand_list_empty;
extern unsigned long stats_roundtrip_pimq_size;
extern unsigned long stats_l2_cache_hits;
extern unsigned long stats_l2_cache_access;
extern unsigned long stats_l2_cache_occupancy;
extern unsigned long stats_num_info_pkts;

//state counters
extern vector<vector<long double> >num_frame_occ_percube_per_app;//occupancy of cube per app
extern vector<vector<long double> >gap_of_acc_per_app;//frequency vector per cube per app  

extern vector<map<unsigned long, vector<unsigned long> > > op_lat_breakdown_tracker;
extern vector<long double> stats_op_latency_breakdown;//vector stats
extern vector<long double> stats_types_of_pkts_retire;//vector stats

extern vector<long double> stats_pim_tab_full_hist;//it will keep track of pim tab full incident
extern vector<long double> stats_pim_dest_hist;//total try = failed + succeed
extern vector<long double> stats_pim_full_rate_hist;//total try = failed + succeed
extern vector<long double> stats_tot_row_hit_hist;
extern vector<long double> stats_tot_row_miss_hist;
extern vector<long double> stats_row_buffer_missrate_hist;
extern vector<long double> stats_mcq_occ_hist;
extern vector<long double> stats_retired_mem_acc_apps;
/////////////////////// STATS VARIABLES DECLARATIONS ////////////////////////

#endif
