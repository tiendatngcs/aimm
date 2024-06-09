#include <iostream>
#include "OpsInACube.hpp"
#include "Registrar.hpp"
using namespace std;

static Registrar<OpsInACube> registrar("OpsInACube");
static bool finished = false;

OpsInACube::OpsInACube(const Configuration& config, const string name) : Motivation(config, name)
{
}

OpsInACube::~OpsInACube()
{
}

bool OpsInACube::OneCycle(){
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

  // operation in a same cube
  for (; j < _elementsPerCube;) {
    if (_vmu->pim_operation(
          _varray[0], 0,
          _varray[0], 1,
          _varray[0], j)) {
      j++;
    }
    return true;
  }

  if (!finished) {
    finished = true;
    cout<<"Process OpsInACube finished ..."<<endl;  
    num_proc_finished++;
    _vmu->print_stats("OpsInACube");
  }
}

/*
# mem acc = _numCubes * _elementsPerCube + _elementsPerCube
void foo() {
  byte arr[_numCubes][_elementsPerCube];

  // init
  for (int i = 0; i < _numCubes; i++)
    for (int j = 0; j < _elementsPerCube; j++)
      arr[i]j = 0;

  for (int j = 0; j < _elementsPerCube; j++)
      arr[0][0] = arr[0][1] + arr[0][j]; // pim_operation(arr[0], 0, arr[0], 1, arr[0], j);

}
*/
