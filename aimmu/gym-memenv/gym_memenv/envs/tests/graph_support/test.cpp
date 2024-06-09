#include "data_collect.hpp"

using namespace std;

int main(int argc, char **argv){
  string folder_name = argv[1];
  string file_name = argv[2];
  DataCollectHook h(folder_name, file_name, 1000);
  for(int i=0; i<10000; i++){
    h.collect(rand());
  }
  return 0;
}
