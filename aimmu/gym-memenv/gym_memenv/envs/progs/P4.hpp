#ifndef _P4_H_
#define _P4_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class P4: public Process
{
  public:
    P4(const Configuration& config, const string name);
    ~P4();
    
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
