#ifndef _P2_H_
#define _P2_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class P2: public Process{
  public:
    P2(const Configuration& config, const string name);
    ~P2();
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
