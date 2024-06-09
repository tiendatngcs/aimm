#include<stdlib.h>
#include "traffic_pattern.hpp"

using namespace std;

int main(int argc, char** argv){
  SyntheticTraffic *SynTr = new SyntheticTraffic(); 
  unsigned long cycle = atoi(argv[1]);
  for(unsigned long c=0; c < cycle; c++){
    SynTr->OneCycle(); 
  }
}
