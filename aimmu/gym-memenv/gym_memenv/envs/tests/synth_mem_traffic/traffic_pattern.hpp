#ifndef _TRAFFIC_PATTERN_HPP
#define _TRAFFIC_PATTERN_HPP

#include<iostream>
#include<deque>
#include<vector>
#include "../../configs/config_utils.hpp"

/*
 * We are aiming to create access type blocks, which are parameterisable 
 * ----------------------------------
 * Define Memory Access Instructions:
 * ----------------------------------
 *  [Mainly they need to define the dependency with each other, and also
 *  should not be commited until they are resolved.]
 *
 *  ++ Regular memory instructions (cacheable)
 *  ++ Processing In Memory (PIM)
 *  ++ Non-memory instructions
 *
 * -------------------------
 * Instruction Dependencies:
 * -------------------------
 *  ++ Dependent instructions
 *    -- Depedent instruction won't be retired until 
 *       the dependecy is being resolved
 *  ++ Independent instructions
 *    -- Will issued after checking the available slot
 *  
 * --------------
 * Address Types:
 * --------------
 *  [Can be implemented by returning address range whenever invoked for allocation.
 *  The address range needs to be freed whenever deallocated (will happen frequently
 *  for stack). A single variable should be covered by stack. Each variable creation
 *  should be part of the initialization (exception stack variables).]
 *
 *  ++ non-memory
 *  ++ local (stack) 
 *    -- grows and shrinks about the same addresss range
 *    -- may use same physical allocation to avoid hard page faults (only soft page fault)                   
 *  ++ global
 *    -- should be allocated in the beginning and mostly never change 
 *  ++ heap 
 *    -- Can be allocated at any point of execution
 *    -- Can be deallocated at any point of execution 
 * 
 * -------------
 * Access Types:
 * -------------
 *  [Given an address range and address type pick an address 
 *   following the access type. In some access type there might be a 
 *   relation between the previous access(es) and the current access.
 *   Each instance will maintain thier own sequence, order, and repetition.]
 *
 *  ++ Discrete
 *    -- Any address type uncorelated with each other 
 *  ++ Contiguous
 *    -- Array accesses
 *  ++ Stride
 *    -- Array accesses
 *  ++ Burst
 *  ++ Chain
 *  ++ Random
 *
 * -------------------
 * Access Block Types:
 * -------------------
 *  [There should be a set of blocks of each type. We need to have a 
 *  parameterized get_block(...) method, which will return the instance 
 *  of the right kind of block after creation. That block object pointer
 *  will be attached with the CFG node. All types of blocks must use same
 *  functions, where their definitions are different. Those block functions
 *  will be invoked by the CFG node methods.]
 *
 *  ++ Pure (any of the access types)
 *  ++ Mixed (any percentage of types) 
 *    -- Fixed
 *    -- Random
 *    -- Adaptive
 *
 * ----------------
 * Traffic Pattern:
 * ----------------
 * [Need to build CFG with the provision of registering at least one
 *  block (possibly multiple) in each node. The CFG can be taken as input
 *  in Adjecency List format. In case of fixed pattern, the block must be
 *  selected in the same list. For random, no need to register the blocks with
 *  the nodes. However, blocks must be described. At the end of each block there 
 *  must be a ratio of choosing that path. Along with that, there should be number
 *  of occurences for each of the blocks, or it could be random within a range, or
 *  it can be conditional as well.]
 *
 *  ++ Fixed
 *  ++ Random
 *  ++ Adaptive [will think about it later]
 * 
 * ---------------
 * Injection Rate:
 * ---------------
 *  ++ to reflect the distance between accesses
 *  ++ to reflect the hit in caches for cacheble data
 */


class SyntheticTraffic{
  public:
    SyntheticTraffic();
    ~SyntheticTraffic();
    void OneCycle(); 
};

#endif
