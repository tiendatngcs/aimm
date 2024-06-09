#include <iostream>
#include "Motivation.hpp"
#include "Registrar.hpp"
#include <typeinfo>
using namespace std;

Motivation::Motivation(const Configuration& config, const string name) : Process(config, name)
{
  cout << "Initialized memory for Process: " << typeid(this).name() << endl;
  int _memSize = config.GetInt("mem_size");
  int _xDim = config.GetInt("x_dim");
  int _yDim = config.GetInt("y_dim");
  _numCubes = _xDim * _yDim;
  
  assert(((unsigned long)_memSize*1024*1024*1024) % _numCubes== 0);

  cout << "total element: " << dec << ((unsigned long)_memSize*1024*1024*1024) << endl;
  _elementsPerCube = ((unsigned long)_memSize*1024*1024*1024)/(_numCubes);   //4294967295ul; //4*1024*1024*1024;
  cout << "_numCubes: " << _numCubes << ", _elementsPerCube: " << _elementsPerCube << endl;
  _varray = new vm* [_numCubes];
  _idx = new int[_numCubes];

  for (int i = 0; i < _numCubes; i++) {
    _varray[i] = _vmu->allocate_heap(_elementsPerCube, 1, _pid);
    _idx[i] = 0;
    cout << "base address of " << dec << i << "'th array: " << hex << _varray[i]->address_at(0) << endl;
  }
}

Motivation::~Motivation()
{
  delete [] _varray;
}
