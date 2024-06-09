#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <map>
#include "Process.hpp"
#include "config_utils.hpp"
using namespace std;

class ProcessFactory
{
  public:
    static ProcessFactory* GetInstance();
    Process* CreateProcess(const Configuration& config, const string& name) const;
    void RegisterProgram(const string name, function<Process* (const Configuration& config, const string& name)> constructor);
    vector<string> GetProgramNames() const;

  private:
    map<string, function<Process* (const Configuration& config, const string& name)> > _programConstructors;
};

#endif
