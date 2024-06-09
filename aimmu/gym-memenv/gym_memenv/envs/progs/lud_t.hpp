#ifndef _LUD_T_H_
#define _LUD_T_H_

#include "tr_helper.hpp"
#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class lud_t: public Process
{
  public:
    lud_t(const Configuration& config, const string name);
    
    virtual bool OneCycle();
    int get_process_id(){return _process_id;}
    string get_process_name(){return _process_name;}
    void set_process_id(int pid){_process_id=pid;}
    trace_helper tr_help;
    trace_entry tr_entry; 
  private:
    int _process_id;
    string _process_name;
    bool _read_next;
    unsigned long _op_id;
};

#endif
