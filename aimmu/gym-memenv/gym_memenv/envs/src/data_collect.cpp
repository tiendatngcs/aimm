#include "data_collect.hpp"

using namespace std;

DataCollectHook::DataCollectHook(string folder, string file_name, unsigned long rec_intv){
  _c = folder;
  _s = file_name;
  _r_intv = rec_intv;
  _intv = 0;
  
  time_t t = time(0);
  struct tm * now = now = localtime(&t);

  char buffer[80];
  strftime(buffer, 80,"%S-%M-%H-%d-%m-%Y", now);
  
  _s = _s + buffer + ".csv"; 

  cout<<"recoding data for: "<<_c<<endl;
  cout<<"the series name: "<<_s<<endl;
  
  string folder_path = "stats/" + _c;
  bool status = mkdir(folder_path.c_str(), 0777);
  _full_path = "stats/" + _c + "/" + _s;
}


void DataCollectHook::set_params(string folder, string file_name, unsigned long rec_intv){
  _c = folder;
  _s = file_name;
  _r_intv = rec_intv;
  _intv = 0;
  
  time_t t = time(0);
  struct tm * now = now = localtime(&t);

  char buffer[80];
  strftime(buffer, 80,"%S-%M-%H-%d-%m-%Y", now);
  
  _s = _s + buffer + ".csv"; 

  cout<<"recoding data for: "<<_c<<endl;
  cout<<"the series name: "<<_s<<endl;
  
  string folder_path = "stats/" + _c;
  bool status = mkdir(folder_path.c_str(), 0777);
  _full_path = "stats/" + _c + "/" + _s;

}

void DataCollectHook::collect(string data){
  _file.open(_full_path, ios_base::app); 
  if(_intv < _r_intv){
    _data_list.push_back(data);
    _intv++; 
  }
  else{
    if(!_file.is_open()){
      cout<<"file "<< _full_path <<"is not open ..."<<endl;
      assert(0);
    }
    else{
      for(int i=0; i<_data_list.size(); i++){
        _file<<_data_list[i];
        _file<<",";
      }
      _data_list.clear();
    }
  _intv=0;
  }
  _file.close();
}

/*
 * Only invoked at the end of the simulation !!!
 */
void DataCollectHook::print(){
  _file.open(_full_path, ios_base::app); 
  cout<<"data collect invoked ..."<<" data size: "<<_data_list.size()<<endl;
  if(!_file.is_open()){
    cout<<"file "<< _full_path <<"is not open ..."<<endl;
    assert(0);
  }
  else{
    for(int i=0; i<_data_list.size(); i++){
      _file<<_data_list[i];
      _file<<",";
    }
    _data_list.clear();
  }
  _file.close();
}

void PageInfo::return_consolidated_pg_info(vector<long double>& pginfo){
  long double norm_acc_done = (_num_acc_done*1.0)/_num_max_acc;
  long double norm_migr = (_num_migr*1.0)/_num_acc_done;
  long double norm_remaining_acc = ((_num_max_acc - _num_acc_done)*1.0)/_num_max_acc;
  pginfo.push_back(norm_acc_done);
  pginfo.push_back(norm_migr);
  pginfo.push_back(norm_remaining_acc);
  for(int i=0; i<_history_len; i++){
    if(_op_hist[i].first!=0){
      pginfo.push_back(1.0/_op_hist[i].first);
    }
    else{
      pginfo.push_back(0.0);
    }
    if(_op_hist[i].first!=0){
      pginfo.push_back(1.0/_op_hist[i].second);
    }
    else{
      pginfo.push_back(0.0);
    }
    if(_migr_hist[i]!=0){
      pginfo.push_back(1.0/_migr_hist[i]);
    }
    else{
      pginfo.push_back(0.0);
    }
    if(_action_hist[i]!=0){
      pginfo.push_back(1.0/_action_hist[i]);
    }
    else{
      pginfo.push_back(0.0);
    }
  }
}

void PageInfo::clear_info(unsigned long pg){
   //_num_max_acc = 0;
   _num_acc_done = 0;
   _num_migr = 0;
   _num_pim_ops = 0;
   _history_len = 16;
   _num_diff_byte_addr = 0;
   _predicted_remaining_access = 0;
   _migr_start = 0;
   _migr_end = 0;
   _migr_in_progress = false;
    
    _op_hist.clear();
    _migr_hist.clear();
    _action_hist.clear();
    _opid.clear();  
}
