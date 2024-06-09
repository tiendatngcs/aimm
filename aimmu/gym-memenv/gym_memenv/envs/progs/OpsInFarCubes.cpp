#include <iostream>
#include "OpsInFarCubes.hpp"
#include "Registrar.hpp"
using namespace std;

static Registrar<OpsInFarCubes> registrar("OpsInFarCubes");
static bool finished = false;

OpsInFarCubes::OpsInFarCubes(const Configuration& config, const string name) : Motivation(config, name)
{
}

OpsInFarCubes::~OpsInFarCubes()
{
}

bool OpsInFarCubes::OneCycle(){
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
  
  // operation between far cubes
  for (; j < _elementsPerCube;) {
    if (_vmu->pim_operation(
          _varray[0], 0,
          _varray[_numCubes-2], 0,
          _varray[_numCubes-1], j)) {
      j++;
    }
    return true;
  }

  if (!finished) {
    finished = true;
    cout<<"Process OpsInFarCubes finished ..."<<endl;  
    num_proc_finished++;
    _vmu->print_stats("OpsInFarCubes");
  }
}
/*
void foo() {
  byte arr[_numCubes][_elementsPerCube];

  // init
  for (int i = 0; i < _numCubes; i++)
    for (int j = 0; j < _elementsPerCube; j++)
      arr[i][j] = 0;

  for (int j = 0; j < _numCubes; j++)
      arr[0][0] = arr[_numCubes-2][0] + arr[_numCubes-1][j]; // pim_operation(arr[0], 0, arr[_numCubes-2], 0, arr[_numCubes-2], j);

}
*/
