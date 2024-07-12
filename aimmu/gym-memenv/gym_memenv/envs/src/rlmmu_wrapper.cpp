#include "rlmmu_wrapper.hpp"
#include <iostream>

using namespace std;

wrapper_mmu::wrapper_mmu(string const &configfile){
 
  _config = new SynthConfig();
  _config->ParseFile(configfile);
  ifstream in(configfile);
  cout << "BEGIN Configuration File: " << configfile << endl;
  while (!in.eof()) {
    char c;
    in.get(c);
    cout << c ;
  }
  cout << "END Configuration File: " << configfile << endl;


  cout<<"\n[WRAPPER] Reding config file: "<<configfile<<endl;

  S = new Sim(*_config);
 
  _x_dim = _config->GetInt("x_dim");
  _y_dim = _config->GetInt("y_dim");
  _mem_size = _config->GetInt("mem_size");
  _page_frame_size = _config->GetInt("page_size");
  _num_nodes = _x_dim * _y_dim;
  _size_per_node = _config->GetInt("mem_size")/(double)_num_nodes;//in 
  _frames_per_cube = (_size_per_node * 1024 * 1024)/_page_frame_size;
  _mmu = MemoryManagementUnit::GetInstance(*_config);
  
  _output_folder = _config->GetStr("stats_file");
  _output_folder = "stats/" + _output_folder; 
  bool check = mkdir(_output_folder.c_str(), 0777);  
  if(!check){
    cout<<"--- created stat drectory ----> stats/"<<_output_folder<<endl;
  }
  else{
    cout<<"*** unable to create stats directory ***"<<endl;
    if(isDirectoryExists(_output_folder.c_str())){
      cout<<"+++ WARNING: directory is already there | results will be overwritten +++"<<endl;
    }
    else{
      cout<<"*** ||| ERROR: Dirctory cna't be created | Program exit(1) ||| ***"<<endl;
      exit(1);
    }
  }
  cout<<"[WRAPPER MMU] ------ Constructor Finished -------- "<<endl;
}

string wrapper_mmu::stats_folder(){
  return _output_folder; 
} 

int wrapper_mmu::isDirectoryExists(const char *path){
  struct stat stats;

  stat(path, &stats);

  // Check for file existence
  if (S_ISDIR(stats.st_mode))
    return 1;

  return 0;
}

//TODO: need to make it general or need to pass the memory network topology 
// We are assuming mesh by default as of now
int wrapper_mmu::decide_new_cube(int cube, int action){
  int return_cube = -1;
  int nb_action = action % 5;//total 5 actions for each entry
  switch(nb_action){
    case 0:
      return_cube = cube;//no action
      break;
    case 1: //go left
      return_cube = cube -1;
      break;
    case 2: //go right
      return_cube = cube + 1;
      break;
    case 3: // go up
      return_cube = cube + _x_dim;
      break;  
    case 4: // go down
      return_cube = cube - _x_dim;
      break;
    //case 5: // go left up
    //  return_cube = cube + _x_dim - 1;
    //  break;
    //case 6: // go right up
    //  return_cube = cube + _x_dim + 1;
    //  break;
    //case 7: // go left down
    //  return_cube = cube - _x_dim - 1;
    //  break;
    //case 8: // go right down
    //  return_cube = cube - _x_dim + 1;
    //  break;
    default:
      assert(0 && "*** action out of range ***");
  }
  if(return_cube < 0 || return_cube >= _num_nodes){
    return_cube = cube;
  }
  return return_cube; 
}

/*
 * Associate the action with a migration event !!!!
 */

void wrapper_mmu::recv_action(vector<int> &action){//getting an action list
  
  assert(action.size() <= num_proc_submission); 
  
  for(int i=0; i<action.size(); i++){
    
    if(possible_migration_cand_list[i].size() > 0){//initial entry skippied
      //cout<<"\n\n\n\n\nIN--------- |||||||||||||||||||||||||||   size of the migration list:"
      //  <<possible_migration_cand_list.size()<<"\n\n\n\n\n\n"<<endl;
      bool no_migr = false;
      
      tuple<int, int, unsigned long> p = possible_migration_cand_list[i][0];
      int pid = get<0>(p);
      int cube = get<1>(p);
      unsigned long page = get<2>(p);

      action_hist[action[i]]++; 
      //cout<<"[RLMMU_WRAPPER] action: "<<action<<endl;

      if(action[i] == 0){
        //no migration
        no_migr = true;
        //cout<<"[RLMMU_WRAPPER-0] no migration: "<<page<<endl;
      }
      else if(action[i] <= 2){
        //cout<<"[MMU_WRAPPER] index: "<<index<<" cube: "<<cube<<" page: "<<page<<endl;
        if(pid < num_proc_submission){
          //int new_cube = decide_new_cube(cube, action);
         int new_cube = -1;
         if(action[i]==1){//near to source/dest
            int num_nei = adj[cube].size();
            new_cube = adj[cube][rand()%num_nei];
            //cout<<"[RLMMU_WRAPPER-3] making entry for: "<<page<<endl;
         }
         else if(action[i] == 2){//far from source/dest
            //int curr_row = cube%_x_dim;
            //int curr_col = cube/_y_dim;
            //new_cube = rand()%(_x_dim *_y_dim);//TODO: need to fix it for the far node!!!!       
            //cout<<"[RLMMU_WRAPPER-4] making entry for: "<<page<<endl;
            new_cube = _x_dim*_y_dim - cube - 1;//TODO: can associate with the operations 
         }
         else{
          assert(0 && "*** Out of bound actions ***");
         }
         if(new_cube!=cube){
           page_info_map[pid][page]->add_action_hist(action[i]); 
           _mmu->add_mig_cand(pid, page, new_cube);//actions are [no, left, right, up, down, lu_diag, ld_diag, ru_diag, rd_diag]
          }
        }
      }
      else{
        if(action[i] == 7){
          flip_dest_node = true;
          //no_migr = true;
        }
        else if(action[i] > 4){
          //no_migr = true;
          if(action[i]==5){
            increase_training_rate = true;//it is threshold 
          } 
          else{
            decrease_training_rate = true; 
          }
        }
       
        //if(action == 3){
          auto it = compute_migration_cand_list[i].find(page);
          page_info_map[pid][page]->set_migr_in_progress(false);
          if(it != compute_migration_cand_list[i].end()){
            if(it->second!=4){
              it->second = 3;//updating compute location
              //cout<<"[RLMMU_WRAPPER-1] update entry for: "<<page<<endl;
            }
          }
          else{
            //cout<<"[RLMMU_WRAPPER-2] making entry for: "<<page<<endl;
            if(action[i]==3 || action[i]==4){
              compute_migration_cand_list[i][page] = action[i];//TODO: change if there are multiple options on vmap
            }
          }
          //assert(action < 5 && "*** Out of bound action ***");
        //}
      }
      if(!no_migr){
        possible_migration_cand_list[i].erase(possible_migration_cand_list[i].begin());
      }
    }
    else{
      stats_possible_cand_list_empty++;
    }
  }
}

unsigned long wrapper_mmu::get_frames_per_cube(){
  return _frames_per_cube; 
}

vector<double> wrapper_mmu::run(){
  return S->run_gen(256);
  //S->drain_remaining_packets();
}

vector<tuple<double, double, double> > wrapper_mmu::get_input_node2vec(){
  int pid = 0; //TODO need to supply the pid here (**Important)
  _mmu->create_input_n2v(pid);
  return input_for_node2vec;    
}

vector<vector<long double> > wrapper_mmu::get_system_state(){
  trace_buffer.clear();
  
  trace_buffer.push_back(stats_pim_full_rate_hist);//pim table occupancy [cube_no]
  trace_buffer.push_back(stats_row_buffer_missrate_hist);//row buffer missrate [cube no]
  trace_buffer.push_back(stats_mcq_occ_hist);//memory controller queue [cube no]
  vector<long double>temp; 
  for(int i=0; i<action_hist.size(); i++){
    if(action_hist[i]!=0){
      temp.push_back(action_hist[i]);
    }
    else{
      break;
    }
  }
  trace_buffer.push_back(temp);

  for(int i=0; i<num_proc_submission; i++){ 
   if(possible_migration_cand_list[i].size() > 0){//initial entry skippied
      vector<long double>pg_vec;
      tuple<int, int, unsigned long> p = possible_migration_cand_list[i][0];
      int pid = get<0>(p);
      int cube = get<1>(p);
      unsigned long page = get<2>(p);
      page_info_map[pid][page]->return_consolidated_pg_info(pg_vec); 
      trace_buffer.push_back(pg_vec);//memory controller queue [cube no]
    }
  }
  
  return trace_buffer;
}

void wrapper_mmu::clear_trace_buffer(){
  trace_buffer.clear();
}

vector<vector<tuple<double, double, double> > >wrapper_mmu::get_hop_profile(){
  //cout<<"\n\n\n++++++++++++++++++++++++++ size of hop profiler: "<<hop_profile[0].size()<<"\n\n\n"<<endl; 
//  return hop_profile;
}

vector<vector<tuple<double, double, double> > >wrapper_mmu::get_migration_list(vector<vector<unsigned long> > &migration_list){
//  sys_migration_list.clear();
//  sys_migration_list.resize(migration_list.size());
//  assert(_mmu!=nullptr);
//  for(int i=0; i<migration_list.size(); i++){
//    int pid = migration_list[i][0];
//    unsigned long vaddr = migration_list[i][1];
//    unsigned long new_frame = migration_list[i][3];
//    assert(pid<num_proc_submission);
//    _mmu->add_mig_cand(pid, vaddr, new_frame);
//  }
//  return hop_profile;
}

void wrapper_mmu::print_stats_sim(){
  S->print_stats(_config->GetStr("stats_file"));  
}

void wrapper_mmu::collect_individual_stats_sim(){
  S->collect_individual_stats(0);  
}

int wrapper_mmu::get_num_proc(){
  return num_proc_submission; 
}
