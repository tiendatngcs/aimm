#include <iostream>
#include "P1.hpp"
#include "Registrar.hpp"
using namespace std;

static Registrar<P1> registrar("one");
static bool finished = false;

P1::P1(const Configuration& config, const string name) : Process(config, name)
{
  cout << "Initialized memory for Process: one" << endl;
  string sim_type = config.GetStr("sim_type");
  if(sim_type == "toy"){
      dim = 10;
  } 
  else if(sim_type == "small"){
      dim = 100;
  }
  else if(sim_type == "medium"){
      dim = 1000;
  }
  else if(sim_type == "large"){
      dim = 10000;
  }
  else if(sim_type == "extralarge"){
      dim = 100000;
  }
  else if(sim_type == "xxl"){
      dim = 1000000;
  }
  else if(sim_type == "max"){
      dim = 10000000;
  }
  else{
      dim = 10;
  }
  i = 0;
  j = 0;
  k = 0;
  
  a = _vmu->allocate_heap(dim*dim, 16, _pid);  // why is 16? why not 32/64?
  cout << "base address of a: " << a << endl;

  b = _vmu->allocate_heap(dim*dim, 16, _pid);
  cout << "base address of b: " << b << endl;

  c = _vmu->allocate_heap(dim*dim, 16, _pid);
  cout << "base address of c: " << c << endl;
}

P1::~P1()
{
  assert(a != 0 && b != 0 && c != 0);
  delete a;
  delete b;
  delete c;
}

bool P1::OneCycle(){

  for (;k < dim*dim;) {
    _vmu->non_memory(0);    // just step with a probability
    if (_vmu->read_memory(c, k)) {
      k++;
    }
    return true;
  }
  
  for (;j < dim*dim;) {
    _vmu->non_memory(0);
    if (_vmu->read_memory(a, j)) {
      j++;
    }
    return true;
  }

  for (;i < dim*dim; ) {
    _vmu->non_memory(0);
    if(_vmu->read_memory(b, i)) {
      i++;
    }
    return true;
  }

  if (!finished) {
    finished = true;
    cout<<"Process one finished ..."<<endl;  
    num_proc_finished++;
    _vmu->print_stats("one");
  }
}
