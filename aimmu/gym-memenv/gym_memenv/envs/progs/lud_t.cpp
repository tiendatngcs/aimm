#include "lud_t.hpp"
#include "Registrar.hpp"

using namespace std;

static Registrar<lud_t> registrar("lud_trace");


lud_t::lud_t(const Configuration& config, const string name) : Process(config, name){
  //we don't need to initialize the memory as we are using traces
  string folder_path = config.GetStr("trace_folder");
  cout<<"lud trace path: "<<folder_path<<endl;
  _process_name = "lud";
  int trace_limit = config.GetInt("lud_trace_limit");
  bool _no_roll_over = config.GetInt("no_roll_over")>0;
  _op_id = 1;
  _read_next = true;

  tr_help.get_path(folder_path, _process_name, trace_limit, _no_roll_over);
  
  cout<<"++++++++++++++++++lud_t  PID: "<<_vmu->get_pid()<<endl;
  set_process_id(_vmu->get_pid());
}

bool lud_t::OneCycle(){
  while(true){
    if(_read_next==true){
      tr_entry = tr_help.get_memop();
      _op_id++;
    }
    if(tr_entry._reset==true){
      cout<<"[lud] application restart | resetting all the pages and releasing all the page frames for lud ..." <<endl;
      //_vmu->clear_memory();
      _vmu->clear_memory(get_process_id());
      tr_entry._reset = false;
    }

    _read_next = _vmu->pim_operation_trace(get_process_id(), _op_id, tr_entry._dest, tr_entry._src1, tr_entry._src2);  
    return _read_next; 
  } 
}
