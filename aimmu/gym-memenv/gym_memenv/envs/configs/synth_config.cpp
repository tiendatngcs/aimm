// $Id$

/*
 Copyright (c) 2007-2015, Trustees of The Leland Stanford Junior University
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "booksim.hpp"
#include "synth_config.hpp"

SynthConfig::SynthConfig()
{
  /*************************************************/
  /*              virtual memory map               */
  /*************************************************/
  _addr_map["text_s"] =  0x000000400000;
  _addr_map["text_e"] =  0x0000004ec000;
  _addr_map["heap_s"] =  0x0000006ee000;
  _addr_map["stack_s"] = 0x7ffffffdd000 ;
  _addr_map["shared_s"] =  0x7ffdd2931000;
  _addr_map["shared_e"] =  0x7ffff7ffd000;


  /*************************************************/
  /*              memory network                   */
  /*************************************************/
  _int_map["num_proc"] = 4;
  _int_map["mem_size"] = 16;
  _int_map["tlb_size"] = 256;
  _int_map["page_size"] = 4;
  _int_map["x_dim"] = 4;
  _int_map["y_dim"] = 4;
  _int_map["num_cmp_tiles"] = 16;
  _str_map["topology"] ="init/topo/mesh_4x4";//also need to change the max hop average
  _str_map["r_algo"] = "XY";
  _int_map["router_stages"] = 3;
  _int_map["max_mcq_size"] = 16;
  _int_map["max_cmpq_size"] = 32;
  _int_map["mem_acc_lat"] = 100;
  _int_map["mem_row_hit_lat"] = 40;
  _int_map["mem_row_miss_lat"] = 120;
  _int_map["mem_queue_size"] = 512;
  _int_map["num_vaults"] = 32;
  _int_map["banks_per_vault"] = 8;
  _int_map["resp_q_size"] = 256;
  _int_map["num_vcs_5_sides"] = 25;
  _int_map["pkt_buffer_per_vc"] = 16;
  _int_map["req_pkt_size"] = 16;//in Bytes
  _int_map["resp_pkt_size"] = 72;//in Bytes
  _int_map["link_width"] = 16;//in Bytes
  _str_map["port_positions"] = "{0,3,12,15}";//read as IntArray
  _int_map["num_port"] = 4;
  _int_map["router_radix"] = 6;
  _int_map["num_vnet"] = 5;
  _int_map["pim_tab_size"] = 512;
  _int_map["migr_q_size"] = 128;
  _int_map["pg_hist_len"] = 16;
  _int_map["multi_program"] = 0;
  _int_map["max_forward_queue_size"] = 256;
  _int_map["page_info_cache_size"] = 128;
  _int_map["l2_cache_size"] = 256;//number of entries (NOT Byte)
  _str_map["l2_repl_policy"] = "FIFO";//LRU, FIFO->NRU
  _int_map["pei_enabled"] = 0;//by default disabled
  _int_map["max_mshr_entry"] = 64;//coveruing all cmps
  _int_map["warmup_cycle"] = 10000;//for warming up the system before training starts
  _int_map["injection_rate"] = 1;//rate at which we shedule pim operations
  _int_map["info_pkt_qlen_node"] = 256;  
  
  /*************************************************/
  /*              Representation Learning          */
  /*************************************************/
  _int_map["num_state_subgraph_entries"] = 32;

  /*************************************************/
  /*              Neural Circuit Configs           */
  /*************************************************/
  _int_map["perceptron_size"] = 16;//number of inputs and weights
  _int_map["neural_circuit_size"] = 5;//number of perceptrons in the neural circuit 
  _float_map["neural_cache_threshold"] = -0.05;//need to fix emperically  
  _int_map["num_neural_ckt"] = 32;//this value also needs to be tuned: Power of 2
  _int_map["max_hop_avg"] = 8;
  _float_map["theta"] = 0.5;

  /*************************************************/
  /*              stats and debugging              */
  /*************************************************/
  _int_map["watch_packet"] = -1;
  _str_map["stats_file"] = "stats";
  
  /*************************************************/
  /*              Simulation Type                  */
  /*************************************************/
  _int_map["dis_migr"] = 0;//by default migration is on
  _int_map["dis_migr_sim"] = 0;//if enabled, then actual packets won't be sent
  _int_map["dis_training"] = 0;//by default training is on
  _int_map["dis_results_forward_sim"] = 0;//overhead of computation migration
  _str_map["pg_allocation_policy"] = "baseline";
  _int_map["tom_phy_cube_remap"] = 0;//by default "tom" is off

  _str_map["sim_type"] = "medium";//small, medium, large, extralarge, xxl, max
  _int_map["dim"] = 100;
  _addr_map["max_tot_mem_acc"] = 10000000000;  
  _addr_map["max_tot_rollovers"] = 0x5;  
  
  _int_map["load_balance_baseline"] = 0;//load balance off by default
  /*************************************************/
  /*              Program Params                   */
  /*************************************************/
  //dnn parameters
  _int_map["N"] = 10000;//number of ifmaps
  _int_map["H"] = 16;//height of the ifmap
  _int_map["W"] = 16;//width of the ifmap
  _int_map["C"] = 3;//depth of the if map and filters
  _int_map["M"] = 15;//number of filters
  _int_map["R"] = 3;//height of the filters
  _int_map["S"] = 3;//width of the filters
  _int_map["U"] = 2;//stride 

  _int_map["no_roll_over"] = 0;
  _str_map["runfile"] = "runlists/runlist";
  _str_map["trace_folder"] = "../../../../trace_folder/benchmarks/naive-art";
  _int_map["join_flow_freq"] = 1000; //join flow after each thousands instructions

  //_int_map["backprop_trace_limit"] = 3354;
  _int_map["backprop_trace_limit"] = 350;
  _int_map["lud_trace_limit"] = 629;
  _int_map["pagerank_trace_limit"] =  595;
  _int_map["rbm_trace_limit"] = 1133;
  _int_map["spmv_trace_limit"] = 502;
  _int_map["mac_trace_limit"] = 640;
  _int_map["reduce_trace_limit"] = 640;
  _int_map["kmeans_trace_limit"] = 350;//we have around 10000
  _int_map["sgemm_trace_limit"] = 350;//we have around 10000
  _int_map["svm_trace_limit"] = 350;//we have around 10000
  _int_map["streamcluster_trace_limit"] = 350;//we have around 10000

  /*************************************************/
  /*              Time Series Plot --> graphs      */
  /*************************************************/
  _str_map["adaptive_epoch_folder"] = "adaptive_epoch_";//folder to store the adaptive epoch series
  _str_map["adaptive_epoch_series"] = "adaptive_epoch_";//file/series name
  _str_map["hop_avg_folder"] = "hop_avg_";//folder name
  _str_map["hop_avg_series"] = "hop_avg_";//file/series name
  _int_map["hop_avg_freq"] = 100;
  _str_map["opc_avg_folder"] = "opc_avg_";//folder name
  _str_map["opc_avg_series"] = "opc_avg_";//file/series name
  _int_map["opc_avg_freq"] = 100;
  _str_map["mig_freq_avg_folder"] = "mig_freq_avg_";//folder name
  _str_map["mig_freq_avg_series"] = "mig_freq_avg_";//file/series name
  _int_map["mig_freq_avg_freq"] = 100;//at the end one entry for each page
  
  /*************************************************/
  /*         Page Profiled/Prediction info         */
  /*************************************************/
  _str_map["pg_profile_folder"] = "tests/pg_reuse_pred/input/"; 
}
