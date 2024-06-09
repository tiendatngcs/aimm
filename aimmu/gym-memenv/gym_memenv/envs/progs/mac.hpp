#ifndef _MAC_H_
#define _MAC_H_

#include "tr_helper.hpp"
#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class mac: public Process
{
  public:
    mac(const Configuration& config, const string name);
    
    virtual bool OneCycle();
    int get_process_id(){return process_id;}
    void set_process_id(int pid){process_id=pid;}
    string get_process_name(){return _process_name;}
  
    trace_helper tr_help;
    trace_entry tr_entry; 
  
  private:
    int process_id;
    string _process_name;
    bool _no_roll_over;
    bool _read_next;
    unsigned long _op_id;
};

#endif
