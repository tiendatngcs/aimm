#include "tr_helper.hpp"

vector<string> trace_helper::split_line(string line, string delim){
  vector<string> splits;
  string org_line = line;
  int pos = 0;
  string token;
  while((pos = line.find(delim)) != string::npos){
    token = line.substr(0, pos);
    splits.push_back(token);
    line.erase(0, pos + delim.length());
  } 
  splits.push_back(line);
  return splits;
}

void trace_helper::get_path(string path, string bench, int trace_limit){
  _folder_path = path;
  _bench = bench;
  _trace_limit = trace_limit;
  cout<<"folder path = "<<_folder_path<<" bench: "<<_bench<<" trace limit: "<<_trace_limit<<endl;
}

void trace_helper::read_trace(string file_path){
  ifstream ifstr(file_path, ifstream::in);
  _trace.clear();
  cout<<"read_trace from--> folder path = "<<_folder_path<<" bench: "<<_bench<<" trace limit: "<<_trace_limit<<endl;
  while(!ifstr.eof()){
    string line;
    getline(ifstr, line);
    vector<string> st = split_line(line, ",");
    if(st.size()>=5){//to avoid problem in trace
      trace_entry te(st);
      _trace.push_back(te);
    }
    //cout<<" "<<line<<endl;
    //for(int i=0;i<st.size();i++){
    //  cout<<"i: "<<i<<" "<<st[i]<<endl;
    //}
    //cout<<"\n\n\n"<<endl;
  }
}

trace_entry trace_helper::get_memop(){
  bool t_empty = false;
  if(_trace.empty()){
    _file_num++;
    t_empty = true;
    if(_file_num > _trace_limit){
      _file_num = 0;//reset file number 
    }
  }
  if(t_empty==true){
    string file_path = _folder_path + "/" + _bench + "/" + _bench + "-pim-" + to_string(_file_num) + ".op";
    cout<<"file path: "<<file_path<<endl;
    read_trace(file_path);
    _vector_index = -1;
  }
  _vector_index++;
  trace_entry l_te = _trace[_vector_index];
  _trace.erase(_trace.begin()+_vector_index);
  return l_te;
}
