#include "data_collect.hpp"

using namespace std;

DataCollectHook::DataCollectHook(string folder, string file_name, unsigned long rec_intv){
  _c = folder;
  _s = file_name;
  _r_intv = rec_intv;
  _intv = 0;
  cout<<"recoding data for: "<<_c<<endl;
  cout<<"the series name: "<<_s<<endl;
}

void DataCollectHook::collect(long double data){
  if(_intv < _r_intv){
    _data_list.push_back(data);
    _intv++; 
  }
  else{
    bool status = mkdir(_c.c_str(), 0777);
    string full_path = _c + "/" + _s;
    //cout<<"writing to file ... : "<<full_path<<endl;
    _file.open(full_path, ios_base::app); 
    if(!_file.is_open()){
      cout<<"file "<< full_path <<"is not open ..."<<endl;
      assert(0);
    }
    else{
      for(int i=0; i<_data_list.size(); i++){
        _file<<_data_list[i];
        _file<<",";
      }
      _data_list.clear();
    }
  _intv=0;
  _file.close();  
  }
}
