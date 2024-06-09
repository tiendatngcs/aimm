#ifndef _DNN_KERNEL_H_
#define _DNN_KERNEL_H_

#include "config_utils.hpp"
#include "vmap.hpp"
#include "Process.hpp"

class dnn_k_2: public Process
{
  public:
    dnn_k_2(const Configuration& config, const string name);
    ~dnn_k_2();

    virtual bool OneCycle();

  private:
    vm *IFMAP;//input feature map
    vm *FILTER;//filter
    vm *OFMAP;//output feature map
    vm *BIAS;//bias

    vm *IFMAP_dummy;//input feature map
    vm *FILTER_dummy;//filter
    vm *OFMAP_dummy;//output feature map
    //IFMAP dims
    int N;//number of input feature maps
    int H;//height of the input feature map
    int W;//width of input feature map
    int C;//depth of input feature map
          //same depth will be used for filter as well
    
    //FILTER dims
    int M;//number of filters
    int R;//filter height
    int S;//filter width
    int U;//stride

    //OFMAP dims
    int E;//OFMAP height
    int F;//OFMAP width
    //depth will be M
    //number of OFMAP will be N

    //temp
    int T;
    int T_ReLU;

    //loop variables
    int z;
    int u;
    int x;
    int y;
    int k;
    int i;
    int j;
};

#endif
