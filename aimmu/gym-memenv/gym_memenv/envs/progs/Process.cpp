#include "Process.hpp"

int Process::_pid = 0;
Process::Process(const Configuration& config, const string name)
  : _launch_cyc(0)
{
  MemoryManagementUnit * _local_mmu = MemoryManagementUnit::GetInstance(config); 
  int l_pid = GetPid();
  _vmu = _local_mmu->get_vmu(l_pid);
  //_vmu = new v_map_use(config);
  _vmu->connect_to_v2p(_local_mmu->get_v2p(l_pid));
  _vmu->process_name = name;
  _vmu->set_pid(l_pid);
}

Process::~Process()
{
  assert(_vmu != 0);
  delete _vmu;
}

int Process::GetPid() const {return _pid++;}

int Process::get_process_id(){return _pid;}

string Process::get_process_name(){return _name;}

void Process::SetLaunchCycle(const long cycle) { _launch_cyc = cycle; }
    
bool Process::ReadyToRun(const unsigned long global_clock) const
{
  bool is_ready = false;
  //cout<<"[Process] Ready to run? clk: "<<global_clock<<endl;
 
  if (_launch_cyc < 0){
    is_ready = false;
  } 
  else if( _launch_cyc >= 0 && _launch_cyc <= global_clock){
    is_ready = true;
  }
  else{
    //cout<<"[PROCESS] need to wait to be launched ..."<<endl;
    //need to wait to be launched 
  }
  //cout<<"[Process] ready to run? "<<is_ready<<endl;
  return is_ready;
}
