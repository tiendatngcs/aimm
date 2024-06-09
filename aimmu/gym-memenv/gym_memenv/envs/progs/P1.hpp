#ifndef _P1_H_
#define _P1_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class P1: public Process
{
  public:
    P1(const Configuration& config, const string name);
    ~P1();
    
    virtual bool OneCycle();

  private:
    vm *a;
    vm *b;
    vm *c;
    int dim;  
    int k;
    int j;
    int i;
};

#endif
