#include "tr_helper.hpp"

int main(int argc, char**argv){
  assert(argc>1 && "have to mention the file path");
  string folder_path = argv[1];
  string bench = argv[2];
  int trace_limit = atoi(argv[3]);
  trace_entry te;
  trace_helper th(folder_path, bench, trace_limit);
  for(int i=0; i<4; i++){
    te = th.get_memop();
    te.print_all();
  }
}
