#ifndef _MOTIVATION_H_
#define _MOTIVATION_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class Motivation: public Process
{
  public:
    Motivation(const Configuration& config, const string name);
    ~Motivation();
    
    virtual bool OneCycle() = 0;

  protected:
    vm **_varray;
    int *_idx;
    unsigned long _elementsPerCube;
    unsigned long _numCubes;
    unsigned long _xDim;
    unsigned long _yDim;
};

#endif
