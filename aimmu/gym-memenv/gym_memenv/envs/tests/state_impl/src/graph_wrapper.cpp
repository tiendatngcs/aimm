#include "graph_wrapper.hpp"

graph_wrapper::graph_wrapper(const string &config_file){
  
  SynthConfig *config = new SynthConfig();
  config->ParseFile(config_file);
  
  _num_cubes = config->GetInt("x_dim") * config->GetInt("y_dim"); 
  _num_entries_tab = config->GetInt("num_state_subgraph_entries");

  _g = new Graph(*config);
}

void graph_wrapper::run_ops(int num_op){
  
  for(int i=0; i<num_op; i++){
    unsigned long va_d = rand();
    unsigned long va_s1 = rand();
    unsigned long va_s2 = rand();
    
    int s1_cube = va_s1 % _num_cubes;
    int s2_cube = va_s2 % _num_cubes; 

    int tab_curr_index = _g->get_index(va_d); //current subgraph index, -1 if not avaiable
    int tab_s1_index = _g->get_index(va_s1); //s1 subgraph index, -1 if not avaiable
    int tab_s2_index = _g->get_index(va_s2); //s2 subgraph index, -1 if not avaiable
    
    if(tab_curr_index==-1){
      int victim_index = _g->findVictim();//also need to invalidate/zeroed the counters
      tab_curr_index = victim_index;
    }  
    //update the tree 
    _g->update_table(tab_curr_index, tab_s1_index, s1_cube, tab_s2_index, s2_cube);//only increasing the counters
  }
}

void graph_wrapper::display_raw_data(){
  _g->graph_display();//only collect info regarding subgraph/subtree (coarse level )
}

vector<tuple<int, int, double> > graph_wrapper::inp_n2v(){
  return _g->return_n2v_inp();
}
