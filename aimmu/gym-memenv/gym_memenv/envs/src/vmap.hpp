#ifndef _VMAP_HPP_
#define _VMAP_HPP_

#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <assert.h>
#include <vector>
#include <map>
#include <math.h>

#include "v2p/v2p.hpp"
#include "cmp_net.hpp"
#include "stats.hpp"
#include "extern_vars.hpp"
#include "config_utils.hpp"
#include "graph_creation_engine.hpp"

using namespace std;

/*
 * Memory allocation class: takes care of the memory allocations 
 */
class vm
{
  public:
    /**
     * @param sa    start address
     * @param ea    end address
     * @param se    size of element
     * @param pid   owner of this pid
     */
    vm(unsigned long sa, unsigned long ea, unsigned long se, int pid);

    /**
     * @brief calculte the target address of index
     *
     * @param index
     * @return long _s_addr + index * _size_element
     */
    long address_at(int index);
    int get_pid();

  private:
    unsigned long _s_addr;
    unsigned long _e_addr;
    unsigned long _size_element;
    int _pid;
};

/*
 * It is wrapper class for the whole simulation
 */
class v_map_use
{
  public:
    v_map_use(){
      _l_v2p = NULL;
    }
    v_map_use(const Configuration& config);
    ~v_map_use();

    string process_name;
   
    vector<int> _port;//holds the ports
    int get_port(unsigned long);
    
    /*************************************************/
    /*              Heap Functions                   */
    /*************************************************/
    /**
      * @param  n     the number of elements
      * @param  s     the size of each element
      * @return vm*
      */
    vm* allocate_heap(unsigned long n, unsigned long s, int pid);

    /*************************************************/
    /*              Stack Functions                  */
    /*************************************************/
    /*
    vm* allocate_stack(unsigned long number_of_elements, unsigned long sizeof_each_element);
    bool deallocate_stack(vm &vmem_ref);
    void update_running_base_stack(long size);  //size can be -ve if called from deallocate
    */

    /*************************************************/
    /*            Memory operation functions         */
    /*************************************************/
    bool read_memory(vm* varray, int index);
    bool write_memory(vm* varray, int index);
    bool pim_operation(vm* dest, int idx_dest, vm* src1, int idx_src1, vm* src2,  int idx_src2);
    bool pim_operation_new(int pid, unsigned long op_id, vm* dest, int idx_dest, vm* src1, int idx_src1, vm* src2,  int idx_src2);
    bool pim_operation_trace(int pid, unsigned long op_id, unsigned long dest, 
                             unsigned long src1, unsigned long src2);
    bool pim_op_names(vm* dest, int idx_dest, string dest_name, 
                      vm* src1, int idx_src1, string src1_name, 
                      vm* src2,  int idx_src2, string src2_name);
    void non_memory(const double ratio);
    bool get_phy_addr(vm*, int);
    pair<int, int> addr_to_nodeNum_conv(bool dest, unsigned long page, unsigned long long paddr, unsigned long op_id); 
    void clear_memory(int pid);
    void set_done(){
      _done = true;
      cout<<"[VMAP] set done = "<<_done<<endl;
    }
    bool is_done(){
      bool l_d = _done;
      _done = false;
      return l_d;
    }

    
    /*************************************************/
    /*              Page Info Collect                */
    /*************************************************/
    void record_page_acc(unsigned long addr, int pid, int cube, unsigned long opid){
      auto it = page_info_map[pid].find(addr);
      if(it!=page_info_map[pid].end()){
        it->second->incr_num_acc_done();
        it->second->add_opid(opid);
        if(it->second->get_acc_ratio() >= 0.2){//TODO: should use a better way to get the threshold
          if(it->second->get_migr_status()==false /*&& it->second->get_migr_cnt() <= 16*/){//TODO: threshold source 
             possible_migration_cand_list[pid].push_back(make_tuple(pid, cube, addr));
             it->second->set_migr_in_progress(true);
          } 
        }
      }
      else{
        //TODO: some pages are missing on the rollouts ... 
        //*****[ERROR] this is the reason for pointer error at the end ****...

        //cout<<"pg: "<<addr<<" not found ..."<<endl;
        //cout<<"size of page_info_map: "<<page_info_map.size()<<endl;
        //assert(0 && "*** at this point PageInfo pointers should have been initiated ***");
      }
    }
    
    /*************************************************/
    /*              Stats Functions                  */
    /*************************************************/
    void print_stats(string benchname);
    void connect_to_v2p(vir2phy *v2p){_l_v2p = v2p;}
    void set_pid(int pid){_pid = pid;}
    int get_pid(){return _pid;}
    void create_input_n2v();

  private:

    /*************************************************/
    /*              Initialization                   */
    /*************************************************/
    void create_stats_vars();

    /*
     * @brief creates those many number of objects choosing random range of addresses
     * and put them in shared_obj_list.
     */
    //void create_shared_objs(int number_of_objs);

  private:
    int _pid;
    int _num_port;

    int _req_pkt_size;//in bytes
    int _resp_pkt_size;//in bytes
    int _link_width;//in bytes
    int _num_req_flits;
    int _num_write_flits;
    
    int _x_dim;
    int _y_dim;
    double _size_per_node;
    int _frame_size;
    unsigned long _tot_frame_per_cube;
    bool _disable_training;  
    
    bool _done;//once the application is done --> one episode
   
    /********************************************/
    /*            TOM Implementation            */
    /********************************************/
    bool _tom_enable;
    unsigned long _pmi;
    unsigned long _tom_epoch;
    int _tom_LSB;//lowest bit in the range
    int _tom_MSB;//highest bit in the range
    int _tom_granularity;//cache block --> original "warp" granularity 
    bool _tom_training;
    unsigned long _prev_op_id;
    int _prev_cube;
    unsigned char _tom_bit_masks;//one mask per process and update for each kernel/epoch
    vector<unsigned long>_tom_collocation_count;//for each process  
    vector<unsigned char>_possible_masks; 

    int _get_tom_cube_id(unsigned long);
    int _tom_training_for_epoch(unsigned long, unsigned long);
    void _tom_create_possible_masks();
    void _tom_select_mask();

    /*************************************************/
    /*              Virtual Memory                   */
    /*************************************************/
    unsigned long _text_base;        // base virtual address for text
    unsigned long _heap_base;        // base virtual address for heap
    unsigned long _stack_base;       // base virtual address for stack
    unsigned long _shared_base;      // base virtual address for shared objects

    unsigned long _text_upperbound;    //base virtual address for text
    //unsigned long _heap_upperbound;    //base virtual address for heap
    //unsigned long _stack_upperbound;   //base virtual address for stack
    unsigned long _shared_upperbound;  //base virtual address for shared objects

    unsigned long _r_heap_base;        //this virtual address will change every time we add in heap
    unsigned long _r_stack_base;       //this virtual address will change every time we add in stack

    bool _baseline_load_bal;

    vir2phy *_l_v2p;              //virtual to physical converter
    Graph *_g;
};
#endif
