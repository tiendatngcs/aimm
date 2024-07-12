#ifndef RLMMU_WRAPPER_HPP
#define RLMMU_WRAPPER_HPP

#include "sim.hpp"
#include "synth_config.hpp"
#include "config_utils.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "extern_vars.hpp"
#include "mmu.hpp"
#include "graph_creation_engine.hpp"
#include <tuple>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace py = pybind11;

class wrapper_mmu{
  public:
    wrapper_mmu(string const &configfile);
    vector<double> run();//returns number of operation / cycle
    vector<vector<long double> > get_system_state();
    vector<tuple<double, double, double> >get_input_node2vec();
    void clear_trace_buffer();
    unsigned long get_frames_per_cube();
    void print_stats_sim();
    void collect_individual_stats_sim();
    vector<vector<tuple<double, double, double> > >get_migration_list(vector<vector<unsigned long> >&);
    vector<vector<tuple<double, double, double> > >get_hop_profile();
    int get_num_proc();
    void recv_action(vector<int>& action);
    int decide_new_cube(int, int);
    int isDirectoryExists(const char *path);    
    string stats_folder(); 
    bool finished(){return is_done;}
    void reset_finished(){is_done = false;}

  private:
    int _x_dim;
    int _y_dim;
    int _mem_size;
    int _page_frame_size;
    int _num_nodes;
    long double _size_per_node;
    unsigned long _frames_per_cube;
    string _output_folder;

    SynthConfig * _config;  
    Sim *S;
    MemoryManagementUnit *_mmu;
}; 
#endif
