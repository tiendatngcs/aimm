#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <memory>

#include "vmap.hpp"
#include "config_utils.hpp"
#include "mmu.hpp"

class Process
{
  public:
    Process(const Configuration& config, const string name);
    ~Process();
    

    virtual bool OneCycle() = 0;// this method executes one cycle and returns
    int GetPid() const;
    virtual int get_process_id();
    virtual string get_process_name();
    void SetLaunchCycle(const long cycle);
    bool ReadyToRun(const unsigned long global_clock) const;

  protected:
    v_map_use* _vmu;
    static int _pid;
    string _name;
    long _launch_cyc;
};
#endif
