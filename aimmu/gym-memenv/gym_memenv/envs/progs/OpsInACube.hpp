#ifndef _OpsInACube_H_
#define _OpsInACube_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Motivation.hpp"

class OpsInACube: public Motivation
{
  public:
    OpsInACube(const Configuration& config, const string name);
    ~OpsInACube();
    
    virtual bool OneCycle();

  private:
};

#endif
