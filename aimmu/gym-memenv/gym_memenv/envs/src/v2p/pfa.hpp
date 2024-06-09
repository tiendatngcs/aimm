#ifndef _PAGE_FRAME_ALLOCATOR_H_
#define _PAGE_FRAME_ALLOCATOR_H_

#include <iostream>
#include <deque>
#include <map>
#include <math.h>
#include <numeric>
#include "../extern_vars.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../configs/config_utils.hpp"

namespace py = pybind11;

using namespace std;

class mem_cube{
  public:
    mem_cube(){
      _unallocated_frames = 0;
      _cube_id = -1;
      _cube_size = -1;
    }
    mem_cube(unsigned long tot_fr, int id, int size):_unallocated_frames(tot_fr),
      _cube_id(id), _cube_size(size){
        _frame_per_cube = _unallocated_frames;
        cout<<"|||||||||||||||||||||||| _unallocated_frames: "<<_unallocated_frames<<endl;
      }
    
    unsigned long _frame_per_cube;
    unsigned long _unallocated_frames;
    int _cube_id;
    long double _cube_size;//in GB
    
    deque<unsigned long>_free_frame_list;

    unsigned long gen_frame_num();
    void add_to_free_list(unsigned long frame){_free_frame_list.push_back(frame);}
};

class PageFrameAllocator {
  public:
    void operator=(PageFrameAllocator const&)=delete;

    static PageFrameAllocator * GetInstance(){
      if(_pm == NULL){
        _pm = new PageFrameAllocator;
      }
      return _pm;
    }
    unsigned long long get_available_frame(int);//returns the frame number
    unsigned long long get_available_frame_migration(int);//returns the frame number
    void create_page_frames(const Configuration&);
    void update_free_list(unsigned long);
    unsigned long tot_num_frames_percube(){return _tot_frame_per_cube;}
    int return_page_frame_size(){return _page_frame_size;} 

  private:
    PageFrameAllocator();
    ~PageFrameAllocator();
    static PageFrameAllocator *_pm;
    int _x_dim;
    int _y_dim;
    int _mem_size;
    int _page_frame_size;
    int _num_nodes;
    long double _size_per_node;
    unsigned long _tot_frame_per_cube;
    vector<mem_cube>_m_cube;
    bool _run_baseline;
    string _policy;
};

#endif
