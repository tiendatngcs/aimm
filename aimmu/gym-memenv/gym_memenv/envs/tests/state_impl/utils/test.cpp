#include<iostream>
#include<stdlib.h>
#include "graph_creation_engine.hpp"
#include "../../configs/synth_config.hpp"

int main(int argc, char **argv){
  
  int num_op = atoi(argv[1]);
  string config_file = argv[2];

  SynthConfig *config = new SynthConfig();
  config->ParseFile(config_file);
  
  int num_cubes = config->GetInt("x_dim") * config->GetInt("y_dim"); 
  int num_entries_tab = config->GetInt("num_state_subgraph_entries");

  Graph g(*config);
  
  for(int i=0; i<num_op; i++){
    unsigned long va_d = rand();
    unsigned long va_s1 = rand();
    unsigned long va_s2 = rand();
    
    int s1_cube = va_s1 % num_cubes;
    int s2_cube = va_s2 % num_cubes; 

    int tab_curr_index = g.get_index(va_d); //current subgraph index, -1 if not avaiable
    int tab_s1_index = g.get_index(va_s1); //s1 subgraph index, -1 if not avaiable
    int tab_s2_index = g.get_index(va_s2); //s2 subgraph index, -1 if not avaiable
    
    if(tab_curr_index==-1){
      int victim_index = g.findVictim();//also need to invalidate/zeroed the counters
      tab_curr_index = victim_index;
    }  
    //update the tree 
    g.update_table(tab_curr_index, tab_s1_index, s1_cube, tab_s2_index, s2_cube);//only increasing the counters
  }
  g.graph_display();//only collect info regarding subgraph/subtree (coarse level )
  return 0;
}

