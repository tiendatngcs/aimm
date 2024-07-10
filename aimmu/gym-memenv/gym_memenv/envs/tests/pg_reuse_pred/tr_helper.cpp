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
      cout << ">>>>>>>>>>>> End of trace File num:" << _file_num << "trace_limit:" << _trace_limit << endl;
      _file_num = 0;//reset file number 
      exit(0);
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

void trace_helper::_check_and_record(unsigned long addr){
  unsigned long pg_addr = addr>>11;//TODO: assuming page size 2KB
  auto it = _pg_usage.find(pg_addr);
  if(it!=_pg_usage.end()){
    it->second++;
  } 
  else{
    //cout<<"recording for: "<<pg_addr<<" size: "<<_pg_usage.size()<<endl;
    _pg_usage[pg_addr] = 1;
  }
}

void trace_helper::_record_connection_access(unsigned long addr1, unsigned long addr2){
  unsigned long _k1 = addr1 >> 11;
  unsigned long _k2 = addr2 >> 11;

  auto it_1 = _affinity.find(_k1);
  if(it_1 != _affinity.end()){
    auto it_2 = (it_1->second).find(_k2);
    if(it_2 != (it_1->second).end()){
      it_1->second[_k2] += 1;
    }
    else{
      it_1->second[_k2] = 1;
    }
  }
  else{
    map<unsigned long, unsigned long>temp;
    _affinity[_k1] = temp;
    _affinity[_k1][_k2] = 1;
  }
}


void trace_helper::affinity_analysis(){
  
  unsigned long connections = 0;
  unsigned long accesses = 0;
  int row = -1, col = -1; 
  
  vector<unsigned long> quad;
  quad.resize(4);

  vector<vector<unsigned long> > vec;
  vec.resize(20);
  for(int i = 0; i<20; i++){
    vec[i].resize(20);
  }  


  for(auto it =_affinity.begin(); it != _affinity.end(); ++it){
    connections = (it->second).size();//number of unique pages connected
    accesses = _pg_usage[it->first];
      
    if(connections<2){
      row = 0;
    }
    else if(connections<4){
      row = 1;
    }
    else if(connections<6){
      row = 2;
    }
    else if(connections<10){
      row = 3;
    }
    else if(connections<20){
      row = 4;
    }
    else if(connections<50){
      row = 5;
    }
    else if(connections<75){
      row = 6;
    }
    else if(connections<100){
      row = 7;
    }
    else if(connections<200){
      row = 8;
    }
    else if(connections<350){
      row = 9;
    }
    else if(connections<500){
      row = 10;
    }
    else if(connections<800){
      row = 11;
    }
    else if(connections<1000){
      row = 12;
    }
    else if(connections<2500){
      row = 13;
    }
    else if(connections<5000){
      row = 14;
    }
    else if(connections<7500){
      row = 15;
    }
    else if(connections<10000){
      row = 16;
    }
    else if(connections<15000){
      row = 17;
    }
    else if(connections<20000){
      row = 18;
    }
    else{
      row = 19;
    }
    
    
    if(accesses<2){
      col = 0;
    }
    else if(accesses<10){
      col = 1;
    }
    else if(accesses<50){
      col = 2;
    }
    else if(accesses<75){
      col = 3;
    }
    else if(accesses<100){
      col = 4;
    }
    else if(accesses<150){
      col = 5;
    }
    else if(accesses<200){
      col = 6;
    }
    else if(accesses<500){
      col = 7;
    }
    else if(accesses<700){
      col = 8;
    }
    else if(accesses<1000){
      col = 9;
    }
    else if(accesses<2500){
      col = 10;
    }
    else if(accesses<5000){
      col = 11;
    }
    else if(accesses<7500){
      col = 12;
    }
    else if(accesses<10000){
      col = 13;
    }
    else if(accesses<20000){
      col = 14;
    }
    else if(accesses<30000){
      col = 15;
    }
    else if(accesses<50000){
      col = 16;
    }
    else if(accesses<70000){
      col = 17;
    }
    else if(accesses<100000){
      col = 18;
    }
    else{
      col = 19;
    }
    
    vec[row][col] += 1;

    if(row < 10 && col < 10){//low-low
      quad[0]++;
    }
    else if(row < 10 && col > 10){//low-high
      quad[1]++;
    }
    else if(row > 10 && col < 10){//high-low
      quad[2]++;
    }
    else{
      quad[3]++;
    }
  }

  unsigned long conn_wt = 0;
  unsigned long acc_wt = 0;
  
  cout<<"Weighted Matrix:"<<endl;

  cout<<"[";
  
  int i =0, j=0;

  for(i=0; i<20; i++){

    switch(i){
      case 0:
        conn_wt = 1;
        break;
      case 1:
       conn_wt = 4;
       break;
      case 2:
        conn_wt = 6;
        break;
      case 3:
        conn_wt = 10;
        break;
      case 4:
        conn_wt = 20;
        break;
      case 5:
        conn_wt = 50;
        break;
      case 6: 
        conn_wt = 75;
        break;
      case 7:
        conn_wt = 100;
        break;
      case 8: 
        conn_wt = 200;
        break;
      case 9:
        conn_wt = 350;
        break;
      case 10:
        conn_wt = 500;
        break;
      case 11:
        conn_wt = 800;
        break;
      case 12:
        conn_wt = 1000;
        break;
      case 13: 
        conn_wt = 2500;
        break;
      case 14:
        conn_wt = 5000;
        break;
      case 15:
        conn_wt = 7500;
        break;
      case 16:
        conn_wt = 10000;
        break;
      case 17:
        conn_wt = 15000;
        break;
      case 18:
        conn_wt = 20000;
        break;
      case 19:
        conn_wt = 20000;
    }
    
    cout<<"[";
    for(j=0; j<20; j++){

      switch(j){
        case 0:
          acc_wt = 1;
          break;
        case 1:
         acc_wt = 10;
         break;
        case 2:
          acc_wt = 50;
          break;
        case 3:
          acc_wt = 75;
          break;
        case 4:
          acc_wt = 100;
          break;
        case 5:
          acc_wt = 150;
          break;
        case 6: 
          acc_wt = 200;
          break;
        case 7:
          acc_wt = 500;
          break;
        case 8: 
          acc_wt = 700;
          break;
        case 9:
          acc_wt = 1000;
          break;
        case 10:
          acc_wt = 2500;
          break;
        case 11:
          acc_wt = 5000;
          break;
        case 12:
          acc_wt = 7500;
          break;
        case 13: 
          acc_wt = 10000;
          break;
        case 14:
          acc_wt = 20000;
          break;
        case 15:
          acc_wt = 30000;
          break;
        case 16:
          acc_wt = 50000;
          break;
        case 17:
          acc_wt = 70000;
          break;
        case 18:
          acc_wt = 100000;
          break;
        case 19:
          conn_wt = 100000;
      }
      
     cout<<vec[i][j]*conn_wt*acc_wt<<","; 
    }
    cout<<"]";
    if(i!=19 && j!=19){
      cout<<","<<endl;
    }
  }
  cout<<"]"<<endl;
  
  cout<<"\n\nWhithout weights:"<<endl;
  cout<<"[";
  for(i=0; i<20; i++){
    cout<<"[";
    for(j=0; j<20; j++){
      cout<<vec[i][j]<<",";
    } 
    cout<<"]";
    if(i!=19 && j!=19){
      cout<<","<<endl;
    }
  }
  cout<<"]"<<endl;

  cout<<"\n\nQuad: [";
  for(int i=0; i<4; i++){
    cout<<""<<quad[i]<<",";
  }
  cout<<"]"<<endl;
}

void trace_helper::record_pg_uasge(){
  while(1){
    bool t_empty = false;
    if(_trace.empty()){
      _file_num++;
      t_empty = true;
      if(_file_num > _trace_limit){
        _file_num = 0;//reset file number 
        //exit(0);
        break;
      }
    }
    if(t_empty==true){
      string file_path = _folder_path + "/" + _bench + "/" + _bench + "-pim-" + to_string(_file_num) + ".op";
      cout<<"file path: "<<file_path<<endl;
      read_trace(file_path);
      _vector_index = -1;
    }
    _vector_index++;
    unsigned long dest = _trace[_vector_index].return_dest();
    unsigned long src1 = _trace[_vector_index].return_src1();
    unsigned long src2 = _trace[_vector_index].return_src2();
   
    _check_and_record(dest); 
    _check_and_record(src1); 
    _check_and_record(src2); 


   _record_connection_access(dest, src1); 
   _record_connection_access(dest, src2); 
   _record_connection_access(src1, src2); 
   _record_connection_access(src1, dest); 
   _record_connection_access(src2, dest); 


    _trace.erase(_trace.begin()+_vector_index);
  }
}

void trace_helper::print_page_access_summary(string file_path){
   _target_file.open(file_path, ios_base::out);
   cout<<"Printing page access ..."<<endl;
   if(!_target_file.is_open()){
    cout<<"file "<< file_path <<" is not open ..."<<endl;
    assert(0);
   }
   else{
    for(auto it = _pg_usage.begin(); it!=_pg_usage.end(); it++){
      //cout<<it->first<<","<<it->second<<""<<endl;
      _target_file<<it->first;
      _target_file<<",";
      _target_file<<it->second<<endl;;
    } 
   }
   _target_file.close();
}

void trace_helper::load_map(string file_path, map<unsigned long, unsigned long> &lpm){//passing map by reference
  ifstream ifstr(file_path, ifstream::in);
  string::size_type sz = 0;
  cout<<"reading MAP from--> filepath = "<<file_path<<endl;
  while(!ifstr.eof()){
    string line = "problem is here";
    string delim = ",";
    getline(ifstr, line);
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
    if(!splits[0].empty() && !splits[1].empty()){
      //cout<<"pgaddr: "<<splits[0]<<" count: "<<splits[1]<<endl;
      unsigned long pg_addr = stoull(splits[0], &sz, 0);
      unsigned long count = stoull(splits[1], &sz, 0);
      lpm[pg_addr] = count;//no check required as all are unique!!!
    }
  }
}
