#ifndef GRAPH_WRAPPER_HPP
#define GRAPH_WRAPPER_HPP

#include<iostream>
#include<stdlib.h>
#include<vector>
#include<tuple>
#include "graph_creation_engine.hpp"
#include "../../../configs/synth_config.hpp"

class graph_wrapper{
  public:
   graph_wrapper(string const &config_file_name);
   ~graph_wrapper(){}
   void run_ops(int num_op);
   void display_raw_data();  
   vector<tuple<int, int, double> > inp_n2v();

  private:
   int _num_cubes;
   int _num_entries_tab;
   Graph *_g;
};

#endif
