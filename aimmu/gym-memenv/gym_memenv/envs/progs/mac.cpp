#include "mac.hpp"
#include "Registrar.hpp"

using namespace std;

static Registrar<mac> registrar("mac");


mac::mac(const Configuration& config, const string name) : Process(config, name){
  //we don't need to initialize the memory as we are using traces
  string folder_path = config.GetStr("trace_folder");
  cout<<"mac trace path: "<<folder_path<<endl;
  _process_name = "naive_art_mac";
  int trace_limit = config.GetInt("mac_trace_limit");
  bool _no_roll_over = config.GetInt("no_roll_over")>0;
  _op_id = 1;
  _read_next = true;

  tr_help.get_path(folder_path, _process_name, trace_limit, _no_roll_over);
  
  cout<<"++++++++++++++++++mac  PID: "<<_vmu->get_pid()<<endl;
  set_process_id(_vmu->get_pid());
}

bool mac::OneCycle(){
  while(true){
    if(_read_next==true){
      tr_entry = tr_help.get_memop();
      _op_id++;
    }
    if(tr_entry._reset==true){
      cout<<"[mac] application restart | resetting all the pages and releasing all the page frames for lud ..." <<endl;
      //_vmu->clear_memory();
      _vmu->clear_memory(get_process_id());
      tr_entry._reset = false;
    }
    if (tr_entry._end_of_trace) {
      // _vmu->clear_memory(get_process_id());
      print_stats_at_end_of_trace = true;
    }
    
    _read_next = _vmu->pim_operation_trace(get_process_id(), _op_id, tr_entry._dest, tr_entry._src1, tr_entry._src2);  
    return _read_next;
  }
} 
