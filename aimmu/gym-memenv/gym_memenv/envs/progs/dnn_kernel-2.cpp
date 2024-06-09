#include <iostream>
#include "dnn_kernel-2.hpp"
#include "Registrar.hpp"

using namespace std;

static Registrar<dnn_k_2> registrar("dnn_ker_2");
static bool finished_2 = false;
bool incr_2 = false;

dnn_k_2::dnn_k_2(const Configuration& config, const string name) : Process(config, name)
{
  N = config.GetInt("N");
  H = config.GetInt("H");
  W = config.GetInt("W");
  C = config.GetInt("C");
  M = config.GetInt("M");
  R = config.GetInt("R");
  S = config.GetInt("S");
  U = config.GetInt("U");

  E = (H - R + U) / U;
  F = (W - S + U) / U;
  
  assert(E>0 && F>0);
  
  cout<<" requesting memory allocation for: "<<N*C*W*H*8<< ", "<<M*C*R*S*8<<", "<<N*M*F*E*8<<endl;

  IFMAP = _vmu->allocate_heap(N*C*W*H, 16, _pid);
  IFMAP_dummy = _vmu->allocate_heap(N*C*W*H, 16, _pid);
  FILTER = _vmu->allocate_heap(M*C*R*S, 8, _pid);
  FILTER_dummy = _vmu->allocate_heap(M*C*R*S, 16, _pid);
  OFMAP = _vmu->allocate_heap(abs(N*M*F*E), 16, _pid);
  OFMAP_dummy = _vmu->allocate_heap(abs(N*M*F*E), 16, _pid);
  

  z=0;
  u=0;
  x=0;
  y=0;
  k=0;
  i=0;
  j=0;

}

dnn_k_2::~dnn_k_2()
{
  assert(IFMAP!=0 && FILTER!=0 && OFMAP!=0);
  delete IFMAP;
  delete FILTER;
  delete OFMAP;
}

bool dnn_k_2::OneCycle(){
  //do{
  if(finished_2){
    z=0;
    u=0;
    x=0;
    y=0;
    k=0;
    i=0;
    j=0;
    
    IFMAP = _vmu->allocate_heap(N*C*W*H, 16, _pid);
    IFMAP_dummy = _vmu->allocate_heap(N*C*W*H, 16, _pid);
    FILTER = _vmu->allocate_heap(M*C*R*S, 8, _pid);
    FILTER_dummy = _vmu->allocate_heap(M*C*R*S, 16, _pid);
    OFMAP = _vmu->allocate_heap(abs(N*M*F*E), 16, _pid);
    OFMAP_dummy = _vmu->allocate_heap(abs(N*M*F*E), 16, _pid);
    
    finished_2 = false;
  }

  for(;z<N;){
    for(;u<M;){
      for(;x<F;){
        for(;y<E;){
          for(;k<C;){
            for(;i<R;){
              for(;j<S;){
               int ifmap_index = z*C*R*S + k*R*S + (U*x+i)*S + (U*y + j); 
               int filter_index = u*C*R*S + k*R*S + i*S + j;
               int ofmap_index = z*M*F*E + u*E*F + E*x + y;
               bool got_f1 = _vmu->get_phy_addr(OFMAP, ofmap_index);
               bool got_f2 = false;
               bool got_f3 = false;
               if(got_f1==true){
                  got_f2 = _vmu->get_phy_addr(IFMAP, ifmap_index);
                  if(got_f2==true){
                    got_f3 = _vmu->get_phy_addr(FILTER, filter_index);
                    if(got_f3==false){
                      return false;
                    }
                  }
                  else{
                    return false;
                  }                  
               }
               else{
                 return false;
               } 
               //OFMAP[ofmap_index] += IFMAP[ifmap_index] *  FILTER[filter_index];
               //cout<<"z="<<z<<" u="<<u<<" x="<<x<<" y="<<y<<" k="<<k<<" i="<<i<<" j="<<j<<endl;
                 if(_vmu->pim_operation(
                   OFMAP, ofmap_index,
                   IFMAP, ifmap_index,
                   FILTER, filter_index)){
                   j++;
                   incr_2 = true;
                   }
                 else{
                   incr_2 = false;
                 }
                 return true; 
              }
              if(incr_2==true){
                i++;
                j=0;
              }
            }
            if(incr_2==true){
              k++;
              i=0;
            }
          }
          if(incr_2==true){
            y++;
            k=0;
          }
        }
        if(incr_2==true){
          x++;
          y=0;
        }
      }
      if(incr_2==true){
        u++;
        x=0;
      }
    }
    if(incr_2==true){
      z++;
      u=0;
    }
  }
  if(!finished_2){
     finished_2 = true;
    cout<<"Process DNN Kernel Finished ..."<<endl;
    //num_proc_finished++;
    //_vmu->print_stats("dnn_ker");
  }
  //}while(1);
}
