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

void trace_helper::get_path(string path, string bench, int trace_limit, bool no_roll_over){
  _folder_path = path;
  _bench = bench;
  _trace_limit = trace_limit;
  _no_roll_over = no_roll_over;
  cout<<dec<<"folder path = "<<_folder_path<<" bench: "<<_bench<<" trace limit: "<<_trace_limit<<endl;
}

void trace_helper::read_trace(string file_path){
  ifstream ifstr(file_path, ifstream::in);
  _trace.clear();
  while(!ifstr.eof()){
    string line;
    getline(ifstr, line);
    vector<string> st = split_line(line, ",");
    if(st.size()>5){//to avoid problem in trace
      trace_entry te(st);
      _trace.push_back(te);
    }
    //cout<<" "<<line<<endl;
    //for(int i=0;i<st.size();i++){
    //  cout<<"i: "<<i<<" "<<st[i]<<endl;
    //}
    //cout<<"\n\n\n"<<endl;
  }
  //cout<<"finished read_trace from--> folder path = "<<_folder_path<<" bench: "
  //  <<_bench<<" file number: "<<_file_num<<"/"<<_trace_limit<<endl;
  //cout<<" bench: "<<_bench<<" ["<<_file_num<<"]"<<endl;
}

trace_entry trace_helper::get_memop(){
  bool t_empty = false;
  bool rolled_over = false;
  bool end_of_trace = false;
  // cout << "get_memop" << endl;
  if(_trace.empty() || _vector_index == _trace.size()){
    // cout << "Trace is empty" << endl;
    _file_num++;
    t_empty = true;
    if(_file_num>=_trace_limit){
      end_of_trace = true;
      if(_no_roll_over){
        cout<<"no roll over --- *** Simulation End *** "<<endl;
        //exit(0);//will exit in the sim loop
      }
      else{
        _file_num = 0;//reset file number 
        rolled_over = true;
      }
      cout<<"+++(trace ended) rollong over traces for: "<<_bench<<endl;
    }
  }
  if(t_empty==true){
    string file_path = _folder_path + "/" + _bench + "/" + _bench + "-pim-" + to_string(_file_num) + ".op";
    cout<<"[TRACE HELPER] file path: "<<file_path<<endl;
    read_trace(file_path);
    _vector_index = -1;
    // cout<<"[TRACE HELPER] finished reading trace "<<file_path<<endl;
  }
  _vector_index++;
  trace_entry l_te = _trace[_vector_index];
  // l_te.print_all();
  if(rolled_over==true){
    cout << "Rolling over" << endl;
    l_te._reset = true;//TODO if rolled over, need to reset the virtual memory
                        //TODO also need to free the frames in the physical memory
  }

  if (end_of_trace){
    cout << "End of trace" << endl;
    l_te._end_of_trace = true;
  }
  // cout << "Erasing index " << _vector_index << endl;
  // cout << "Trace size " << _trace.size() << endl;
  // _trace.erase(_trace.begin()+_vector_index);
  // cout << "Erasing finished " << endl;
  return l_te;
}
