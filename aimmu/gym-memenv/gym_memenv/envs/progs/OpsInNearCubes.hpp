#ifndef _OpsInNearCubes_H_
#define _OpsInNearCubes_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Motivation.hpp"

class OpsInNearCubes: public Motivation
{
  public:
    OpsInNearCubes(const Configuration& config, const string name);
    ~OpsInNearCubes();
    
    virtual bool OneCycle();

  private:
};

#endif
