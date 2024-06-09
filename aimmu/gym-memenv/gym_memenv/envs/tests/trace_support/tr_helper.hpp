#ifndef _TR_HELPER_HPP
#define _TR_HELPER_HPP

#include<iostream>
#include<stdlib.h>
#include<string>
#include<cstring>
#include<assert.h>
#include<fstream>
#include<vector>

using namespace std;

class trace_entry{
  public:
    trace_entry(){
      _id = -1;
      _clk = 0;
      _pkt_id = 0;
      _dest = 0;
      _src1 = 0;
      _src2 = 0;
    }
    trace_entry(vector<string>&st){
      _id = stol(st[0]);
      _clk = stol(st[1]);
      _pkt_id = stol(st[2]);
      _dest = stol(st[3]);
      _src1 = stol(st[4]);
      _src2 = stol(st[5]);
    }
    void print_all(){
      cout<<"id: "<<_id<<" clk: "<<_clk<<" pkt: "<<_pkt_id<<" dest: "
        <<_dest<<" src: "<<_src1<<" src2: "<<_src2<<endl;  
    }

  private:
    int _id;
    unsigned long _clk;
    unsigned long _pkt_id;
    unsigned long _dest;
    unsigned long _src1;
    unsigned long _src2;
};

class trace_helper{
  public:
    trace_helper(){
      _file_num = 0;
      _vector_index = -1;
      _trace_limit = 0;
      _folder_path = "";
      _bench = "";
    }

    trace_helper(string fp, string b, int lim):_folder_path(fp),_bench(b),_trace_limit(lim), 
      _file_num(0), _vector_index(-1){}
    
    void get_path(string path, string bench, int trace_limit);
    void read_trace(string filename);
    trace_entry get_memop();
    vector<string> split_line(string line, string delim);

  private:
    int _file_num;//keeps track of the file number
    int _vector_index;
    int _trace_limit;
    string _folder_path;
    string _bench;
    vector<trace_entry>_trace;
};

#endif
