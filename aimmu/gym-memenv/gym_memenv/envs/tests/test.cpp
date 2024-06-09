#include <iostream>
#include "v2p.hpp"
#include "m_net.hpp"
#include "pfa.hpp"
#include <stdlib.h>
#include <math.h>
#include <string>
#include <cstring>
using namespace std;
void test_mnet()
{
  /*
      unsigned long long hop_count=0;
      int mem_size = atoi(argv[1]);
      int size_per_node = atoi(argv[2]);
      int x_dim = atoi(argv[3]);
      int y_dim = atoi(argv[4]);
      string topology = argv[5];
      string r_algo = argv[6];
      int port_positions[4]={0,3,12,15};
      cout<<"mem size: "<<mem_size<<endl;
      cout<<"size per node: "<<size_per_node<<endl;
      cout<<"x_dim: "<<x_dim<<endl;
      cout<<"y_dim: "<<y_dim<<endl;
      cout<<"topology: "<<topology<<endl;
      cout<<"r_algo: "<<r_algo<<endl;

      MemoryNetwork BN;
      BN.create_network(mem_size,size_per_node,x_dim,y_dim,topology,r_algo,port_positions);
      int g_port=0;
      int g_node=0;
      for(int i=0;i<atoi(argv[7]);i++){
      int node=BN.addr_to_nodeNum_conv(rand());
      int port=rand()%4;
      g_port=port; g_node=node;
      hop_count+=BN.hop_count_conv(port,node,r_algo);
      }
      hop_count+=BN.hop_count_conv(g_port,g_node,r_algo);
      cout<<"total hop count: "<<hop_count<<endl;
      */
}

void test_v2p()
{
  vir2phy  v2p;
  
  unsigned long long max_vir_addr = 18446744073709551615ull; //2^64-1
  unsigned long long num_phy_addr = 1024;
  PageFrameAllocator::GetInstance()->create_page_frames(1, 4);
  
  for (unsigned long long i = 0; i <= max_vir_addr; i++) {
   
    unsigned long long addr = v2p.GetPhyAddr(0, i);
    assert(addr == i % num_phy_addr);
  }
}


int main()
{
  test_v2p();

  return 0;
}
