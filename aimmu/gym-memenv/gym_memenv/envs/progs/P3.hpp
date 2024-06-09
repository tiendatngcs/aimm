#ifndef _P3_H_
#define _P3_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class P3: public Process
{
  public:
    P3(const Configuration& config, const string name);
    ~P3();
    
    virtual bool OneCycle();

  private:
    vm *matrix;
    vm *lower;
    vm *upper;
    int n;
};

#endif
