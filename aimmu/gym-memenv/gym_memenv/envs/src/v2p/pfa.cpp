#include "pfa.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PageFrameAllocator * PageFrameAllocator::_pm = NULL;

PageFrameAllocator::PageFrameAllocator(){
}

PageFrameAllocator::~PageFrameAllocator(){
}

unsigned long mem_cube::gen_frame_num(){
  unsigned long fr_num = 0;
  stats_frames_used++;
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"unallocated frames: %lu in cube: %d\n",_unallocated_frames, _cube_id);
#endif
  if(_unallocated_frames>1){
    fr_num =  (_unallocated_frames--) + _cube_size * _cube_id;
    //cout<<"[PFA] cube number: "<<_cube_id<<" unallocated frame: "<<_unallocated_frames<<" + "<<_cube_size*_cube_id
    //  <<" final frame: "<<fr_num<<endl;
  }
  else if(_free_frame_list.size()>0){//all the frames are allocated 
    fr_num = _free_frame_list.front();
    _free_frame_list.pop_front();
    stats_free_frames_in_list--;
  }
  //cout<<"[PFA] cube id ---------------------------> "<<_cube_id<<" address frame num: "<<fr_num<<endl;
  return fr_num; 
}

void PageFrameAllocator::update_free_list(unsigned long free_frame){
  free_frame = free_frame % _tot_frame_per_cube;
  int cube_id = (free_frame - (free_frame % _tot_frame_per_cube))/(_tot_frame_per_cube+1); 
  //cube_id = cube_id % (_x_dim*_y_dim);
  _m_cube[cube_id].add_to_free_list(free_frame);
  stats_free_frames_in_list++;
}

unsigned long long PageFrameAllocator::get_available_frame(int pid){
  
  assert(pid<=num_proc_submission);
  
  unsigned long frame_num = 0;
  unsigned long tot_acc = 0;
  int cube = -1;
  
  if(_policy.compare("allalloc")==0){//returns ZERO "0" on match
    cube = rand()%(_x_dim*_y_dim);
  }
  else if(_policy.compare("noalloc")==0){
    for(int c=(_num_nodes-1); c>=0; c--){
      if(_m_cube[c]._unallocated_frames > 0){
        cube = c;
        break;
      }
    }
  }
  else if(_policy.compare("aimmu")==0){
    py::function get_cube = py::reinterpret_borrow<py::function>(py::module::import("aimmuagent.infer_cube").attr("find_cube").attr("get_cube"));
    
    trace_buffer.clear();

    trace_buffer.push_back(stats_pim_full_rate_hist);//pim table occupancy [cube_no]
    trace_buffer.push_back(stats_row_buffer_missrate_hist);//row buffer missrate [cube no]
    trace_buffer.push_back(stats_mcq_occ_hist);//memory controller queue [cube no]
    
    py::object cube_num = get_cube(trace_buffer);
    cube = cube_num.cast<int>();
    if(cube >= _num_nodes){
      cube /= 2;
    }
    //trace_buffer.clear();
  }
  else if(_policy.compare("hoard")==0){
    cube = pid; 
  }
  else{
      cout<<"\n***[PFA] no valid allocation mentioned ***\n"<<endl;
      assert(0 && "--- please mention the correct page allocation poliocy ---");
  }

  frame_num = _m_cube[cube].gen_frame_num();

#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"generated frame: %lu in cube: %d\n", frame_num, cube);
#endif
  return frame_num;
}

unsigned long long PageFrameAllocator::get_available_frame_migration(int cube){
  unsigned long frame_num = 0;
  frame_num = _m_cube[cube].gen_frame_num();
#ifdef DEBUG_PRINT_V2P
    fprintf(debugfile,"generated frame: %lu in cube: %d\n", frame_num, cube);
#endif
  return frame_num;
}

void PageFrameAllocator::create_page_frames(const Configuration& config){
  
  _x_dim = config.GetInt("x_dim");
  _y_dim = config.GetInt("y_dim");
  _mem_size = config.GetInt("mem_size");
  _page_frame_size = config.GetInt("page_size");
  _num_nodes = _x_dim * _y_dim;
  _size_per_node = config.GetInt("mem_size")/(double)_num_nodes;//in 
  _tot_frame_per_cube = (_size_per_node * 1024 * 1024)/_page_frame_size;
  
  _policy = config.GetStr("pg_allocation_policy");

  cout<<"<<<<<<<<<<<<<<<<<<< creating physical memory frames >>>>>>>>>>>>>>>>>>>"<<endl;
  cout<<" x_dim: "<<_x_dim
      <<" y_dim: "<<_y_dim
      <<" _mem_size: "<<_mem_size
      <<" _page_frame_size: "<<_page_frame_size
      <<" _num_nodes: "<<_num_nodes
      <<" _size_per_node: "<<_size_per_node
      <<" _tot_frame_per_cube: "<<_tot_frame_per_cube
      <<endl;  

  for(int i=0; i<_num_nodes; i++){
    mem_cube m(_tot_frame_per_cube, i, _tot_frame_per_cube);
    _m_cube.push_back(m);  
  }
}
