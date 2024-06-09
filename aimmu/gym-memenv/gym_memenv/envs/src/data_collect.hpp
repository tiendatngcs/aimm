#ifndef DATA_COLLECT_HPP
#define DATA_COLLECT_HPP

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <assert.h>
#include <vector>
#include <deque>

using namespace std;

class DataCollectHook{
  public:
    DataCollectHook(){}
    DataCollectHook(string category, string series_name, unsigned long recording_intv);    
    ~DataCollectHook(){}
    
    void collect(string data);
    void set_params(string category, string series_name, unsigned long recording_intv);
    
    void print();

  private:
    string _c;//category name
    string _s;//series name
    unsigned long _r_intv;//recording interval
    unsigned long _intv;
    ofstream _file;//file pointer
    string _full_path;
    vector<string>_data_list;//temporary data buffer
};

class PageInfo{
  public:
    PageInfo(){
      _num_max_acc = 0;
      _num_acc_done = 0;
      _num_migr = 0;
      _num_pim_ops = 0;
      _history_len = 16;
      _num_diff_byte_addr = 0;
      _predicted_remaining_access = 0;
      _migr_start = 0;
      _migr_end = 0;
      _migr_in_progress = false;
    }
    ~PageInfo(){}
    
    unsigned long get_num_max_acc(){return _num_max_acc;} 
    unsigned long get_acc_done(){return _num_acc_done;}
    unsigned long get_migr_cnt(){return _num_migr;}
    unsigned long get_num_pim_ops(){return _num_pim_ops;}
    deque<unsigned long> get_opid(){return _opid;}
    deque<pair<unsigned long, unsigned long> > get_op_hist(){return _op_hist;}
    deque<unsigned long> get_migr_hist(){return _migr_hist;}
    int get_num_diff_byte_addr_acc(){return _num_diff_byte_addr;}
    unsigned long get_pred_remaining_acc(){return _predicted_remaining_access;}
    unsigned long get_migration_delay(){return _migr_end - _migr_start;}
    bool get_migr_status(){return _migr_in_progress;}
    float get_acc_ratio(){return (_num_acc_done*1.0)/_num_max_acc;}

    void set_num_max_acc(unsigned long max_acc){_num_max_acc = max_acc;}
    void incr_num_acc_done(){_num_acc_done++;}
    void incr_num_migr(){_num_migr++;}
    void incr_num_pim_ops(){_num_pim_ops++;}
    void set_history_len(int len){_history_len = len;}
    void incr_num_diff_byte_addr(){_num_diff_byte_addr++;}
    void set_pred_remaining_acc(unsigned long r_acc){_predicted_remaining_access = r_acc;}
    void set_migr_start_time(unsigned long time){_migr_start = time;} 
    void set_migr_end_time(unsigned long time){_migr_end = time;} 
    void set_migr_in_progress(bool status){_migr_in_progress = status;}

    void add_op_hist(unsigned long dist, unsigned long lat){
      if(_op_hist.size() >= _history_len){
        _op_hist.pop_front();  
      }
      _op_hist.push_back(make_pair(dist,lat));
    }
    
    void add_migr_hist(unsigned long lat){
      if(_migr_hist.size() >= _history_len){
        _migr_hist.pop_front();  
      }
      _migr_hist.push_back(lat);
    }
    
    void add_action_hist(int action){
      if(_action_hist.size() >= _history_len){
        _action_hist.pop_front();  
      }
      _action_hist.push_back(action);
    }
    
    void add_opid(unsigned long op_id){
      if(_opid.size() >= _history_len){
        _opid.pop_front();
      }
      _opid.push_back(op_id);
    }
    
    void return_consolidated_pg_info(vector<long double>&);
    
    void clear_info(unsigned long pg);//while evicting page from page info cache

  private:
    unsigned long _num_max_acc;
    unsigned long _num_acc_done;
    unsigned long _num_migr;
    unsigned long _num_pim_ops;
    unsigned long _migr_start;
    unsigned long _migr_end;
    bool _migr_in_progress;
    int _history_len;//take it from config
    int _num_diff_byte_addr;//number of elements accessed
    unsigned long _predicted_remaining_access;//TODO: need to implement prediction logic 
    
    deque<pair<unsigned long, unsigned long> >_op_hist;//len of this == history_len, distance, lat 
    deque<unsigned long>_migr_hist;//action and migration accomplishment time
    deque<int>_action_hist;
    deque<unsigned long>_opid;
};
#endif
