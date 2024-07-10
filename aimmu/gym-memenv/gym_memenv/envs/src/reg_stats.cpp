#include "reg_stats.hpp"

void reg_stats::create_stats(){
  
  /*
   * MC
   */
  g_stats.create_stats("avg_ipc", "Average system IPC");
  g_stats.create_stats("total_instructions_exec", "Total number of instructions executed");
  g_stats.create_stats("reading_from_mcq","Number of requests read from memory controller");
  g_stats.create_stats("avg_mcq_queuing_delay", "Average Number of cycles stayed in memory controller");
  g_stats.create_stats("tot_mcq_queuing_delay", "Total Number of cycles stayed in memory controller");

  /*
   * M_NET
   */
  g_stats.create_stats("retired_mem_acc","Total number of memory accesses finished");
  g_stats.create_stats("num_cycles", "Total number of cycles simulated");
  g_stats.create_stats("total_hops", "Total number of hops traversed");
  g_stats.create_stats("total_retry_cycles", "Total number of cycles retried to send to the next");
  g_stats.create_stats("retry_ntw_cycles_per_pkt","Total number of cycles a packet retried to send to next node");
  g_stats.create_stats("mem_queuing_delay","Total queuing delay in memory");
  g_stats.create_stats("mem_queuing_delay_per_acc","Total queuing delay in memory access");
  g_stats.create_stats("total_mem_s2ret_lat","Total memory latency from start to retire");
  g_stats.create_stats("total_DRAM_lat","Total memory latency in DRAM only");
  g_stats.create_stats("avg_mem_acc_lat","total_mem_s2ret_lat / retired_mem_acc");
  g_stats.create_stats("avg_DRAM_acc_lat","total_mem_DRAM_lat / retired_mem_acc");
  g_stats.create_stats("total_network_lat","Total number of cycles consumed in network");
  g_stats.create_stats("avg_pkt_lat", "Average packet latency");
  g_stats.create_stats("avg_num_hops","Average number of hops per packet");
  g_stats.create_stats("total_resp_pkts","Total number of response packets");
  g_stats.create_stats("total_pkts","Total number of packets generated");
  g_stats.create_stats("total_retired_pkts","Total number of packets generated");
  g_stats.create_stats("failed_to_create_resp_pkts","Number of times system failed to create a resp packet"); 
  g_stats.create_stats("mem_row_hit","calculating total row buffer (size = page size) hits");
  g_stats.create_stats("mem_row_miss","calculating total row buffer misses");

  //pim stats
  g_stats.create_stats("num_pim_req","total number of pim requests/pim operations");
  g_stats.create_stats("avg_pim_op_latency","average latency for one pim operation");
  g_stats.create_stats("num_pim_hops","total number of hops required for all pim operations");
  g_stats.create_stats("pim_op_ret","total number of pim operation retired");
  g_stats.create_stats("pim_acc_lat","total pim access latency");

  /*
   * VMAP
   */  
  g_stats.create_stats("total_memory_acc","Total number of memory access");
  g_stats.create_stats("total_non_memory_inst","Total number of non memory instructions");

  /*
   * V2P
   */
  g_stats.create_stats("page_hit","Total number of page hits");
  g_stats.create_stats("page_fault","Total number of page faults");
  g_stats.create_stats("page_fault_conflict");
  g_stats.create_stats("page_miss_make_entry","Number of times making new page entry");
  g_stats.create_stats("page_replacement","Total number of page replacements");
  g_stats.create_stats("v2p_mem_acc","Total number of virtual to physical address translation");
  g_stats.create_stats("total_L0_tables_created", "1 table, maps 256TB, 2^9 entries");
  g_stats.create_stats("total_L1_tables_created", "2^9 table, maps 512GB/ea, 2^18 entries");
  g_stats.create_stats("total_L2_tables_created", "2^18 table, maps 1GB/ea, 2^27 entries");
  g_stats.create_stats("total_L3_tables_created", "2^27 table, maps 2MB/ea, 2^36 extries");
  g_stats.create_stats("total_active_pages", "Number of active/valid pages in 4-level page table");
  g_stats.create_stats("num_pages_accessed_last_epoch", "Number of pages that got accessed in the last epoch");
  /*
   * Memory
   */
  g_stats.create_stats("frames_used","Total number of physical memory frame used");
  g_stats.create_stats("memory_requested(MB)","Total memory consumptions");
  g_stats.create_stats("memory_allocated(MB)","Total memory allocated");
  g_stats.create_stats("free_frames_in_list","Total number of frames in the free list");
  
  g_stats.create_stats("num_of_migpg_acc", "Total number of migrated page access");
  g_stats.create_stats("tot_acc_mig_pages","Total number of accesses to migrated pages");
  g_stats.create_stats("tot_pg_migrated", "Total number of pages migrated");
  
  g_stats.create_stats("tot_rollovers", "Total number of benchmark rollovers");
  g_stats.create_stats("proc_stall_cyc", "Total number of cycles processor stall");
  g_stats.create_stats("tot_mig_pkts", "Total number of migration packets");
  g_stats.create_stats("vmap_wrong_cube_incident", "garbage address fed --> wrong cube id (not harmful)");
  g_stats.create_stats("l2_access", "total l2 accesses");
  g_stats.create_stats("l2_hits", "total l2 hits");
  g_stats.create_stats("l2_hits_rate", "total l2 hits rate");
  
  g_stats.create_stats("tot_info_pkts", "Total number of info packets created");

  /////////////////////////////////////////////////////////////////////////////////////////
  //////////// ====================  vector stats ===================//////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////
   
  g_stats_vec.create_stats_vec(num_proc_submission, "retired_mem_acc_apps","Total number of memory accesses finished per applications");
  g_stats_vec.create_stats_vec(num_proc_submission, "mig_not_complete_delay", "Total number of cycles delayed for migration not completed");
  g_stats_vec.create_stats_vec(num_proc_submission, "sleep_time", "Total number of cycles delayed for process in sleep");
  g_stats_vec.create_stats_vec(5, "op_latency_breakdown", "Breakdown for operation latency by tracking each individually");
  g_stats_vec.create_stats_vec(3, "pkt_type_retired", "Breakdown for type of packets retire");
  g_stats_vec.create_stats_vec(16, "pim_tab_full", "Histogram for PIM table full per cube");//TODO: take the size from
  g_stats_vec.create_stats_vec(16, "pim_req_dest", "Histogram for PIM destination cube");//TODO: take the size from
  g_stats_vec.create_stats_vec(16, "row_hit", "Histogram for total row hit per cube");//TODO: take the size from
  g_stats_vec.create_stats_vec(16, "row_miss", "Histogram for total row miss per cube");//TODO: take the size from
  g_stats_vec.create_stats_vec(16, "row_missrate", "Histogram for PIM table full per cube");//TODO: take the size from
  g_stats_vec.create_stats_vec(16, "mcq_occ", "Histogram for Memory Controller Queue");//TODO: take the size from
                                                                                            //the config 
}

void reg_stats::update_stats(){
  /*
   * MC
   */
  g_stats.update("reading_from_mcq",stats_reading_from_mcq);
  g_stats.update("avg_ipc",stats_avg_ipc);
  g_stats.update("total_instructions_exec",stats_total_instructions_exec);
  g_stats.update("tot_mcq_queuing_delay", stats_tot_mcq_queuing_delay);
  stats_avg_mcq_queuing_delay = stats_tot_mcq_queuing_delay / stats_total_memory_acc; 
  g_stats.update("avg_mcq_queuing_delay",stats_avg_mcq_queuing_delay);
  /*
   * M_NET
   */
  g_stats.update("retired_mem_acc",stats_retired_mem_acc);
  g_stats.update("num_cycles",global_clock); 
  g_stats.update("total_hops",stats_total_hops);
  g_stats.update("total_retry_cycles",stats_total_retry_cycles); 
  g_stats.update("retry_ntw_cycles_per_pkt",stats_retry_ntw_cycles_per_pkt);
  g_stats.update("mem_queuing_delay",stats_mem_queuing_delay);
  g_stats.update("mem_queuing_delay_per_acc",stats_mem_queuing_delay_per_acc);
  g_stats.update("total_mem_s2ret_lat",stats_total_mem_s2ret_lat);
  g_stats.update("total_DRAM_lat",stats_total_DRAM_acc_lat);
  g_stats.update("avg_mem_acc_lat",stats_avg_mem_acc_lat);
  g_stats.update("avg_DRAM_acc_lat",stats_avg_DRAM_acc_lat);
  g_stats.update("total_network_lat",stats_total_network_lat);
  g_stats.update("avg_pkt_lat",stats_avg_pkt_lat);
  g_stats.update("avg_num_hops",stats_avg_num_hops);
  g_stats.update("total_resp_pkts",stats_total_resp_pkts);
  g_stats.update("total_pkts",stats_total_pkts);
  g_stats.update("total_retired_pkts",stats_retired_pkt);
  g_stats.update("failed_to_create_resp_pkts",stats_failed_to_create_resp_pkts); 
  g_stats.update("mem_row_hit",stats_mem_row_hit);
  g_stats.update("mem_row_miss",stats_mem_row_miss);
  /*
   * VMAP
   */  
  g_stats.update("total_memory_acc",stats_total_memory_acc);
  g_stats.update("total_non_memory_inst",stats_total_non_memory_inst);
  /*
   * V2P
   */
  g_stats.update("page_hit",stats_page_hit);
  g_stats.update("page_fault",stats_page_fault);
  g_stats.update("page_fault_conflict",stats_page_fault_conflict);
  g_stats.update("page_miss_make_entry",stats_page_miss_make_entry);
  g_stats.update("page_replacement",stats_page_replacement);
  g_stats.update("v2p_mem_acc",stats_v2p_mem_acc);
  g_stats.update("total_L0_tables_created",stats_total_L0_tables_created);
  g_stats.update("total_L1_tables_created",stats_total_L1_tables_created);
  g_stats.update("total_L2_tables_created",stats_total_L2_tables_created);
  g_stats.update("total_L3_tables_created",stats_total_L3_tables_created);
  g_stats.update("total_active_pages", stats_total_active_pages);
  g_stats.update("num_pages_accessed_last_epoch", stats_num_pages_accessed_last_epoch);
  /*
   * Memory
   */  
  g_stats.update("frames_used",stats_frames_used);
  stats_memory_requested= (stats_memory_requested / 1024) / 1024; 
  g_stats.update("memory_requested(MB)",stats_memory_requested);
  stats_memory_allocated = (stats_memory_allocated / 1024) / 1024; 
  g_stats.update("memory_allocated(MB)",stats_memory_allocated);
  
  /*
   * Pim 
   */
  g_stats.update("num_pim_hops",stats_pim_hops);
  g_stats.update("num_pim_req",stats_pim_operations);
  g_stats.update("pim_op_ret",stats_pim_op_ret);
  g_stats.update("pim_acc_lat",stats_pim_acc_lat);
  double avg_lat = stats_pim_acc_lat/stats_pim_op_ret;
  g_stats.update("avg_pim_op_latency",avg_lat);
  
  g_stats.update("free_frames_in_list",stats_free_frames_in_list);
  
  /*
   * Page Migration 
   */ 
  g_stats.update("num_of_migpg_acc", stats_numofpg_acc_after_mig);
  g_stats.update("tot_acc_mig_pages", stats_num_of_acc_to_mig_pg);
  g_stats.update("tot_pg_migrated", stats_tot_pg_migrated);
  
  g_stats.update("tot_rollovers", stats_tot_rollovers);
  g_stats.update("proc_stall_cyc", stats_processor_stall_cycles);

  g_stats.update("tot_mig_pkts", stats_types_of_pkts_retire[2]);
  g_stats.update("vmap_wrong_cube_incident", vmap_wrong_cube_incident);
  g_stats.update("l2_access", stats_l2_cache_access);
  g_stats.update("l2_hits", stats_l2_cache_hits);
  double hitrate = (stats_l2_cache_hits*1.0)/stats_l2_cache_access;
  g_stats.update("l2_hits_rate", hitrate);
  g_stats.update("tot_info_pkts", stats_num_info_pkts);
  /////////////////////////////////////////////////////////////////////////////////////////
  //////////// ====================  vector stats ===================//////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////
  
  g_stats_vec.update_vec("mig_not_complete_delay", stats_migration_not_complete_time);
  g_stats_vec.update_vec("sleep_time", stats_tot_sleep_time);
  g_stats_vec.update_vec("op_latency_breakdown", stats_op_latency_breakdown);
  g_stats_vec.update_vec("pkt_type_retired", stats_types_of_pkts_retire);
  g_stats_vec.update_vec("pim_tab_full", stats_pim_tab_full_hist);//TODO: take the size from
  g_stats_vec.update_vec("pim_req_dest", stats_pim_dest_hist);//TODO: take the size from
  g_stats_vec.update_vec("row_hit", stats_tot_row_hit_hist);//TODO: take the size from
  g_stats_vec.update_vec("row_miss", stats_tot_row_miss_hist);//TODO: take the size from
  g_stats_vec.update_vec("row_missrate", stats_row_buffer_missrate_hist);//TODO: take the size from
  g_stats_vec.update_vec("mcq_occ", stats_mcq_occ_hist);//TODO: take the size from
  g_stats_vec.update_vec("retired_mem_acc_apps",stats_retired_mem_acc_apps);
  
}

void reg_stats::clear_stats(){
  g_stats.clear();
  create_stats();
}
