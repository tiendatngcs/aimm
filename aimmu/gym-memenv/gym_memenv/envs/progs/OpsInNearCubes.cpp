#include <iostream>
#include "OpsInNearCubes.hpp"
#include "Registrar.hpp"
using namespace std;

static Registrar<OpsInNearCubes> registrar("OpsInNearCubes");
static bool finished = false;

OpsInNearCubes::OpsInNearCubes(const Configuration& config, const string name) : Motivation(config, name)
{
}

OpsInNearCubes::~OpsInNearCubes()
{
}

bool OpsInNearCubes::OneCycle(){
  static int i = 0;
  static int j = 0;

  // init
  for (; i < _numCubes;) {
    if (_vmu->read_memory(_varray[i], _idx[i])) {
      _idx[i]++;
      if (_idx[i] == _elementsPerCube) i++;
    }
    return true;
  }
  
  // operation between near cubes
  for (; j < _elementsPerCube;) {
    if (_vmu->pim_operation(
          _varray[0], 0,
          _varray[1], 0,
          _varray[_xDim], j)) {
      j++;
    }
    return true;
  }

  if (!finished) {
    finished = true;
    cout<<"Process OpsInNearCubes finished ..."<<endl;  
    num_proc_finished++;
    _vmu->print_stats("OpsInNearCubes");
  }
}

/*
void foo() {
  byte arr[_numCubes][_elementsPerCube];

  // init
  for (int i = 0; i < _numCubes; i++)
    for (int j = 0; j < _elementsPerCube; j++)
      arr[i]j = 0;

  for (int j = 0; j < _numCubes; j++)
      arr[0][0] = arr[1][1] + arr[_xDim][j]; // pim_operation(arr[0], 0, arr[1], 1, arr[_xDim], j);

}
*/
