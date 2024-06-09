#include "ProcessFactory.hpp"
#include <memory>
using namespace std;

ProcessFactory* ProcessFactory::GetInstance()
{
  static ProcessFactory factory;
  return &factory;
}

void ProcessFactory::RegisterProgram(
    const string name,
    function<Process *(const Configuration& config, const string& name)> constructor)
{
  _programConstructors[name] = constructor;
}

Process* ProcessFactory::CreateProcess(const Configuration& config, const string& name) const
{
  auto it = _programConstructors.find(name);
  if (it != _programConstructors.end())
    return it->second(config, name);
  return 0;
}

vector<string> ProcessFactory::GetProgramNames() const
{
  vector<string> names;
  for(auto it = _programConstructors.begin(); it != _programConstructors.end(); it++)
    names.push_back(it->first);
  return names;
}
