#include "traffic_pattern.hpp"

SyntheticTraffic::SyntheticTraffic(){
  /*
   * block descriptions: 
   *  <number_of_blocks of this type> {<access_type, adddr_type, %>, <...> ...}
   */

  /*
   * Define dependency types for the instructions (within basic block)
   *  ++ intra-loop
   *    -- N2M
   *    -- N2N 
   *  ++ inter-loop
   *    -- N2M
   *    -- N2N 
   */
    
  /*
   * 
   */

  _blks = create_blocks();
  _cfg = build_cfg(blks);
}

SyntheticTraffic::~SyntheticTraffic(){
}

void SyntheticTraffic::OneCycle(){
  block_type = get_blocktype_from_cfg();//it may return exact block type and number
  blk = null_ptr;
  switch(block_type){
    case 0:
      blk = get_one_type_0_block(...);
      ...
    default:
  }
  Inst = blk->get_inst();//one block has to be executed completely, then only jump to other
  switch(Inst->type){
    case 0: //non-memory
      dependency_queue.push_back(Inst);
    case 1: //memory
      request_memory(...);//this will send the request to the backend!!!
  }
  if dependency_queue.front() === ready //finished all pipelined stage & resoled all the dependecies
    dependency_queue.pop_front();
}
