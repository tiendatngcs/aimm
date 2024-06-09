#ifndef _REGISTRAR_H_
#define _REGISTRAR_H_

#include "ProcessFactory.hpp"
#include "config_utils.hpp"

template<class T>
class Registrar
{
  public: 
    Registrar(std::string proc_name)
    {
      ProcessFactory::GetInstance()->RegisterProgram(
          proc_name,
          [](const Configuration& config, const string name)->Process* { return new T(config, name);});
    }
};

#endif
