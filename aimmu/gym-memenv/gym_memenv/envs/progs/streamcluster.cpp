#include "streamcluster.hpp"
#include "Registrar.hpp"

using namespace std;

static Registrar<streamcluster> registrar("streamcluster");


streamcluster::streamcluster(const Configuration& config, const string name) : Process(config, name){
  //we don't need to initialize the memory as we are using traces
  string folder_path = config.GetStr("trace_folder");
  cout<<"streamcluster trace path: "<<folder_path<<endl;
  _process_name = "streamcluster";
  int trace_limit = config.GetInt("streamcluster_trace_limit");
  _no_roll_over = (config.GetInt("no_roll_over")>0);
  _read_next = true;
  _op_id = 1;

  tr_help.get_path(folder_path, _process_name, trace_limit, _no_roll_over);
  set_process_id(_vmu->get_pid());
  cout<<"++++++++++++++++++ streamcluster  PID: "<<get_process_id()<<endl;
}

bool streamcluster::OneCycle(){
  while(true){
    if(_read_next==true){
      tr_entry = tr_help.get_memop();
      _op_id++;
    }
    if(tr_entry._reset==true){
      cout<<"[streamcluster] application restart | resetting all the pages and releasing all the page frames for streamcluster ..." <<endl;
      _vmu->clear_memory(get_process_id());
      tr_entry._reset = false;
    }
    
    _read_next = _vmu->pim_operation_trace(get_process_id(), _op_id, tr_entry._dest, tr_entry._src1, tr_entry._src2);  
    return _read_next; 
  } 
}
