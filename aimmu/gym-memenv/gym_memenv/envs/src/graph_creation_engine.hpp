#ifndef _GRAPH_CREATION_ENGINE_HPP_
#define _GRAPH_CREATION_ENGINE_HPP_

#include <iostream>
#include <deque>
#include <vector>
#include <tuple>
#include "../configs/config_utils.hpp"
#include "extern_vars.hpp"

using namespace std;

class Link{
  public:
    Link();
    ~Link();
    
    void build_links(int num);

    int _num;

    /* we can have table entry number of entries in each queue and directly access using index
     * TODO we later need to find out minimum number entries 
     */
    deque<unsigned long>_weightedEdge; //target node number-->index, #accesses >= 1 OR, 0 -> if no link
};

class GraphEntry{
  
  public:
    GraphEntry();
    ~GraphEntry();
    void build_entry(int, int, int);
    void update_tot_acc_cnt(int cube){_totAccCnt[cube]++;}
    void update_forward_acc_cnt(int s_cube, int tab_s_index);
    void update_link(int s_cube, int tab_s_index);
    int get_tot_acc(){return _tot_acc_per_epoch;}
    unsigned long get_tot_acc_per_cube(int c){return _totAccCnt[c];}
    void reset_acc_per_epoch(){_tot_acc_per_epoch = 0;}
    void get_acc_frac(vector<double>&);//passing vector by reference
    int hop_count(int, int);//return the hop difference
    int get_dest_node(int, int);
    void subgraph_diaplay(int);
    bool isvalid(){return _valid;}
    void makevalid(){_valid = true;}
    void set_root_id_va_node(int pid, int node, unsigned long page){
      _pid = pid;
      _cube_root = node;
      _page_root = page;
    }
    unsigned long get_page_root(){return _page_root;}
    unsigned long get_cube_root(){return _cube_root;}
    unsigned long get_pid(){return _pid;}

  private:
    bool _valid;
    int _pid;
    int _cube_root;
    int _max_hops;
    int _num_entries;
    int _num_cubes;
    unsigned long _page_root;
    unsigned long _tot_access;//this will never been reset
    unsigned long _tot_acc_per_epoch;//it will be reset once taken
    //TODO the number of entries inLinkedList for each cube should be FIXED 
    deque<Link>_inLinkList; //records the links that are coming in
    deque<Link>_outLinkList; //records the links that going out
    deque<unsigned long>_totAccCnt; //records the total number of accesses per cube
};

class Graph{
  public:
    Graph(const Configuration config);
    ~Graph();
    int get_index(unsigned long);
    void update_table(int, int, int, int, int);
    int findVictim(); //finds the victim in the _tab 
    void add_forward_link(int, int, int);
    void add_back_link(int, int, int);
    void prep_input_for_node2vec(int); 
    void graph_display();
    void node2vec_inp_display(int);//@arg: which input? 
    void set_root_info(int, int, int, unsigned long);
    //vector<tuple<int, int, double> >  return_n2v_inp(){return input_for_node2vec;} 
    /*
     * After making/updating an entry in the table, this method is invoked 
     * to decide if the entry is good enough to create the state of the Agent
     * with the help of prediction based on the Neural Circuit !!!
     */
    bool ready2gen(int);//providing current entry index
    
    /*
     * Going to generate input for node2vec from the entry in the table 
     * @argument:
     *  -> current_entry_index 
     */
    void createInpForNode2Vec(int);
    
    /*
     * Prepare state
     */
    void prepState(int);
    
    void ClearStructures();
  private:
    int _num_entries;
    int _num_cubes;
    int _max_hops;
    int _threshold;
    int _cir_q_fifo_ptr;//always points to entry location, once Q is filled, then wraps around 
    deque<GraphEntry>_tab;//table to hold subgraphs with partial page address
    vector<vector<double> >_state_buffer;
    vector<vector<double> >_edge_w;
    vector<vector<double> >_num_edges;
};

#endif
