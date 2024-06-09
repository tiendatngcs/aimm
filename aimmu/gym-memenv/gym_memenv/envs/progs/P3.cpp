#include <iostream>
#include "P3.hpp"
#include "Registrar.hpp"
using namespace std;

static Registrar<P3> registrar("lud");
static bool finished = false;

P3::P3(const Configuration& config, const string name) : Process(config, name)
{
  cout << "Initialized memory for Process: lud" << endl;
  //n = 4096;
  string sim_type = config.GetStr("sim_type");
  if(sim_type == "toy"){
      n = 8;
  } 
  else if(sim_type == "small"){
      n = 64;
  }
  else if(sim_type == "medium"){
      n = 512;
  }
  else if(sim_type == "large"){
      n = 4096;
  }
  else if(sim_type == "extralarge"){
      n = 65536;
  }
  else if(sim_type == "xxl"){
      n = 1048576;
  }
  else if(sim_type == "max"){
      n = 33554432;
  }
  else{
      n = 10;
  }
  
  matrix = _vmu->allocate_heap(n*n, 8, _pid);
  cout << "base address of matrix: " << matrix << endl;

  upper = _vmu->allocate_heap(n*n, 8, _pid);
  cout << "base address of upper: " << upper << endl;

  lower = _vmu->allocate_heap(n*n, 8, _pid);
  cout << "base address of lower: " << lower << endl;
}

P3::~P3()
{
  assert(matrix != 0 && lower != 0 && upper != 0);
  delete matrix;
  delete lower;
  delete upper;
}

bool P3::OneCycle()
{
  static int i = 0;
  // for upper
  static int k1 = i, j1 = 0, t1 = 0, t2 = 0;
  // for lower
  static int k2 = i, j2 = 0, t3 = 0, t4 = 0, t5 = 0;

  for (; i < n; ) {

    // Upper
    for (; k1 < n; k1++) {
      //int sum = 0;
      for (; j1 < i; j1++) {
        //sum += (lower[i][j] * upper[j][k]);
        for (; t1 < 2;) {
          if (t1 == 0 && _vmu->read_memory(lower, i*n + j1)) { t1++; return true; }
          else if (t1 == 0) { return true; }
          else if (t1 == 1 && _vmu->read_memory(upper, j1*n + k1)) { t1++; return true; }
          else if (t1 == 1) { return true; }
          else { assert (0 && "never be reached here"); }
        }
        t1 = 0;
      }

      //upper[i][k] = mat[i][k] - sum;
      for (;  t2 < 2; t2++) {
        if (t2 == 0 && _vmu->read_memory(matrix, i*n + k1)) { t2++; return true; }
        else if (t2 == 0) { return true; }
        else if (t2 == 1 && _vmu->read_memory(upper, i*n + k1)) { t2++; return true; }
        else if (t2 == 1) { return true; }
        else { assert (0 && "never be reached here"); }
      }
      t2 = 0;
      j1 = 0;
    }


    // Lower
    for (; k2 < n; k2++) {
      if (i == k2) {
        //lower[i][i] = 1;
        for (; t3 < 1;) {
          if (t3 == 0 && _vmu->write_memory(lower, i*n + i)) { t3++; return true; }
          else if (t3 == 0) { return true; }
          else { assert (0 && "never be reached here"); }
        }
        t3 = 0;
      } else {

        //int sum = 0;
        for (; j2 < i; j2++) {
          //sum += (lower[k][j] * upper[j][i]);
          for (; t4 < 2;) {
            if (t4 == 0 && _vmu->read_memory(lower, k2*n + j2)) { t4++; return true; }
            else if (t4 == 0) { return true; }
            else if (t4 == 1 && _vmu->read_memory(upper, j2*n + i)) { t4++; return true; }
            else if (t4 == 1) { return true; }
            else { assert (0 && "never be reached here"); }
          }
          t4 = 0;
        }

        for (; t5 < 3;) {
          //lower[k][i] = (mat[k][i] - sum) / upper[i][i];
          if (t5 == 0 && _vmu->read_memory(matrix, k2*n + i)) { t5++; return true; }
          else if (t5 == 0) { return true; }
          else if (t5 == 1 && _vmu->read_memory(upper, i*n + i)) { t5++; return true; }
          else if (t5 == 1) { return true; }
          else if (t5 == 2 && _vmu->write_memory(lower, k2*n + i)) { t5++; return true; }
          else if (t5 == 2) { return true; }
          else { assert (0 && "never be reached here"); }
        }
        t5 = 0;
        j2 = 0;
      }
    }

    i++;
    k1 = k2 = i;
  }

  if (!finished) {
    finished = true;
    cout<<"Process lud finished ..."<<endl;  
    num_proc_finished++;
  }
  
}

// Original LUD
//void lud(int mat[][MAX], int n)
//{
//  int lower[n][n], upper[n][n];
//  memset(lower, 0, sizeof(lower));
//  memset(upper, 0, sizeof(upper));
//
//  // Decomposing matrix into Upper and Lower
//  // triangular matrix
//  for (int i = 0; i < n; i++) {
//
//    // Upper Triangular
//    for (int k = i; k < n; k++) {
//
//      // Summation of L(i, j) * U(j, k)
//      int sum = 0;
//      for (int j = 0; j < i; j++)
//        sum += (lower[i][j] * upper[j][k]);
//
//      // Evaluating U(i, k)
//      upper[i][k] = mat[i][k] - sum;
//    }
//
//    // Lower Triangular
//    for (int k = i; k < n; k++) {
//      if (i == k) {
//        lower[i][i] = 1; // Diagonal as 1
//      } else {
//
//        // Summation of L(k, j) * U(j, i)
//        int sum = 0;
//        for (int j = 0; j < i; j++)
//          sum += (lower[k][j] * upper[j][i]);
//
//        // Evaluating L(k, i)
//        lower[k][i] = (mat[k][i] - sum) / upper[i][i];
//      }
//    }
//  }
//}
