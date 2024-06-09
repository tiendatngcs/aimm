#include "graph_creation_engine.hpp"

vector<tuple<int, int, double> >_input_for_node2vec;

Link::Link(){
}

Link::~Link(){
}

void Link::build_links(int  num){
  _weightedEdge.resize(num);
}

GraphEntry::GraphEntry(){
  _num_entries = -1;
  _num_entries = -1;
  _tot_access = 0;//counter to keep track of total access per tab entry
  _tot_acc_per_epoch = 0;
  _max_hops = 7;
  _root_cube = -1;
}

GraphEntry::~GraphEntry(){
}

void GraphEntry::build_entry(int num_entries, int num_cubes, int max_hops){
  _num_entries = num_entries;
  _num_cubes = num_cubes;
  _max_hops = max_hops;

  _outLinkList.resize(num_cubes);
  _inLinkList.resize(num_cubes);
  _totAccCnt.resize(num_cubes);

  for(int i=0; i<_num_cubes; i++){
    _outLinkList[i].build_links(_num_entries);
  }

}

void GraphEntry::update_forward_acc_cnt(int s_cube, int tab_s_index){
  _outLinkList[s_cube]._weightedEdge[tab_s_index]++;
  _tot_access++;
  _tot_acc_per_epoch++;
}

void GraphEntry::update_link(int s_cube, int tab_s_index){
  _inLinkList[s_cube]._weightedEdge.push_back(tab_s_index);
}

Graph::Graph(const Configuration config){
  int x_dim = config.GetInt("x_dim");
  int y_dim = config.GetInt("y_dim");
  _num_entries = config.GetInt("num_state_subgraph_entries");
  _num_cubes = x_dim * y_dim;
  _max_hops = _num_cubes/2 - 1;//TODO: may not be valid for network other than mesh 
  _cir_q_fifo_ptr = 0;
  _threshold = 100;//TODO have to get it from config

  _tab.resize(_num_entries);
  _edge_w.resize(_num_entries);
  _num_edges.resize(_num_entries);

  for(int i=0; i<_num_entries; i++){
    _tab[i].build_entry(_num_entries, _num_cubes, _max_hops);
    _edge_w[i].resize(_num_entries);
    _num_edges[i].resize(_num_entries);
  }

  for(int i=0; i<_num_entries; i++){
    for(int j=0; j<_num_entries; j++){
      _edge_w[i][j] = 0;
      _num_edges[i][j] = 0;
    }
  }
}

Graph::~Graph(){
}

int Graph::get_index(unsigned long va){
  return (va>>11) % _num_entries;//TODO neeed to have a more meaningful hash map
}

//we follow the FIFO replacement policy, implemented usign circular queue!
//so that we don't have physically move elements
int Graph::findVictim(){//returns victim index
  if(_cir_q_fifo_ptr==_num_entries){
    _cir_q_fifo_ptr = 0;
  }
  return _cir_q_fifo_ptr++;
}

//this method will take care of both links and access counts
void Graph::add_forward_link(int tab_s_index, int s_cube, int tab_curr_index){
  _tab[tab_curr_index].update_forward_acc_cnt(s_cube, tab_s_index);
}

//only records the link
void Graph::add_back_link(int s_cube, int tab_s_index, int tab_curr_index){
  _tab[tab_curr_index].update_link(s_cube, tab_s_index);
}

void Graph::update_table(int curr_index, int s1_index, int s1_cube, int s2_index, int s2_cube){
      
    //cout<<"S1 cube: "<<s1_cube<<" S1 index: "<<s1_index<<endl; 
    //adding forward links
    if(s1_index!=-1){
      _tab[curr_index].update_tot_acc_cnt(s1_cube);    
      add_forward_link(s1_index, s1_cube, curr_index);
    }

    if(s2_index!=-1){
      _tab[curr_index].update_tot_acc_cnt(s2_cube);    
      add_forward_link(s2_index, s2_cube, curr_index);
    } 

    //adding back links
    if(s1_index!=-1){
      add_back_link(s1_cube, s1_index, curr_index);
    }

    if(s2_index!=-1){
      add_back_link(s2_cube, s2_index, curr_index);
    }

    //check whether this entry is ready to checkout?
    if(ready2gen(curr_index)){
      createInpForNode2Vec(curr_index);
      prepState(curr_index);
      prep_input_for_node2vec(0);
      node2vec_inp_display(0);
    }
} 

int GraphEntry::hop_count(int c1, int c2){
  return (int)(abs(c1/_num_cubes - c2/_num_cubes) +
                abs(c1%_num_cubes - c2%_num_cubes)); 
}

void GraphEntry::get_acc_frac(vector<double>& vec){
  for(int i=0; i<_num_cubes; i++){
    int hop = hop_count(_root_cube, i);
    float hop_frac = (hop*1.0) / _max_hops;
    vec.push_back(((double)((_totAccCnt[i])/_tot_access))*hop_frac);
  }
}

/*
 * Graph travesal and state creation logic
 */

int GraphEntry::get_dest_node(int cube, int entry){
  return _outLinkList[cube]._weightedEdge[entry];
}

void Graph::prep_input_for_node2vec(int start_index){
  
  int src = -1;
  int dest = -1;
  double l_w = 0;
  
  _input_for_node2vec.clear();

  for(int i=0; i<_num_entries; i++){
    src = i;
    for(int j=0; j<_num_cubes; j++){
      dest = j;
      _edge_w[i][j] = 0;
      _num_edges[i][j] = 0;

      for(int k=0; k<_num_entries; k++){
        int ew = _tab[i].get_dest_node(j,k);
        unsigned long tot_acc = _tab[i].get_tot_acc_per_cube(j);
        if(tot_acc>0){ 
          l_w = (ew*1.0) / tot_acc;
        }
        else{
          l_w = 0;//to avoid nan!!!
        }       
        _edge_w[src][dest] += l_w;//tot acc can be 0 in case the edge doesn't exist
        if(l_w > 0){
          _num_edges[src][dest] += 1;
        }
      }
      if(_num_edges[src][dest]>0){
        _edge_w[src][dest] /= _num_edges[src][dest];
      }

      //vector<double> tmp = {src*1.0, dest*1.0, _edge_w[src][dest]};
      _input_for_node2vec.push_back(make_tuple(src, dest, _edge_w[src][dest]));
      //_input_for_node2vec.push_back(tmp);
    }
  }  
}

void Graph::node2vec_inp_display(int index){
  for(int i=0; i<_num_entries; i++){
    cout<<"entry: "<<i<<" [";
    for(int j=0; j<_num_entries; j++){
      cout<<" "<<_edge_w[i][j];
    }
    cout<<" ]"<<endl;
  }
}

/*
 * Input for node2vec is list of tupples as follows. 
 * [(src node, dest node, weight), (src node, dest node, weight) ... ]
 * list of tupples --> we will finally push it to the buffer for the Agent
 * to pick it up, whenever possible !!!!
 */
void Graph::createInpForNode2Vec(int index){
}

void Graph::prepState(int index){
 vector<double>temp;
 _tab[index].get_acc_frac(temp);//passing the vector
 _state_buffer.push_back(temp);  
}

bool Graph::ready2gen(int i){
  //TODO we need to plug-in proper selection logic (Neural Circuit based prediction)
  if(_tab[i].get_tot_acc() >= _threshold){
    _tab[i].reset_acc_per_epoch();
    return true;
  }
  else{
    return false;
  }
}

void GraphEntry::subgraph_diaplay(int entry){
  for(int i=0; i<_num_cubes; i++){
    cout<<"[tabentry: "<<entry<<" / cube: "<<i<<"] ";
    cout<<"outlinks [";
    for(int j=0; j<_outLinkList[i]._weightedEdge.size(); j++){
      cout<<" "<<_outLinkList[i]._weightedEdge[j];
    }
    cout<<" ]"<<endl;
    cout<<"inLinks ("<<_inLinkList[i]._weightedEdge.size()<<") [";
    for(int j=0; j<_inLinkList[i]._weightedEdge.size(); j++){
      cout<<" "<<_inLinkList[i]._weightedEdge[j];
    }
    cout<<" ]"<<endl;
  }
  cout<<"totalAcc [";
  for(int k=0; k<_totAccCnt.size(); k++){
    cout<<" "<<_totAccCnt[k];
  }
  cout<<" ]"<<endl;
}

void Graph::graph_display(){
  for(int i=0; i<_num_entries; i++){
    _tab[i].subgraph_diaplay(i);
    cout<<endl;
  }
}
