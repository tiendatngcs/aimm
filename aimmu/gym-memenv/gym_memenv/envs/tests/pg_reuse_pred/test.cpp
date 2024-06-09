#include "tr_helper.hpp"

int main(int argc, char**argv){
  assert(argc>1 && "have to mention the file path");
  
  string folder_path = argv[1];
  string bench = argv[2];
  int trace_limit = atoi(argv[3]);
  string target_folder = argv[4];
  
  bool status = mkdir(target_folder.c_str(), 0777);
  string full_path = target_folder + "/" + bench + ".pg";

  trace_entry te;
  trace_helper th(folder_path, bench, trace_limit);
  unsigned long dest = 0, src1 = 0, src2 = 0;
  map<unsigned long, unsigned long>load_pgmap;//passing this map 
  //while(1){
    //te = th.get_memop();
    //te.print_all();
  //}
  th.record_pg_uasge();
  th.print_page_access_summary(full_path);
  th.load_map(full_path, load_pgmap);
  
  //cout<<"size of load_pg_map: "<<load_pgmap.size()<<endl;

  //for(auto it = load_pgmap.begin(); it!=load_pgmap.end(); it++){
  //    cout<<it->first<<","<<it->second<<""<<endl;
  //}

  th.affinity_analysis();
}
