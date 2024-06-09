#ifndef _OpsInFarCubes_H_
#define _OpsInFarCubes_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Motivation.hpp"

class OpsInFarCubes: public Motivation
{
  public:
    OpsInFarCubes(const Configuration& config, const string name);
    ~OpsInFarCubes();
    
    virtual bool OneCycle();

  private:
};

#endif
