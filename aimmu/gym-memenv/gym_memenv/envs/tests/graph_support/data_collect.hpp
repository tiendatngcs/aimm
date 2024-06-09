#ifndef DATA_COLLECT_HPP
#define DATA_COLLECT_HPP

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <assert.h>
#include <vector>

using namespace std;

class DataCollectHook{
  public:
    DataCollectHook(string category, string series_name, unsigned long recording_intv);
    
    ~DataCollectHook(){}
    
    void collect(long double data);

  private:
    string _c;//category name
    string _s;//series name
    unsigned long _r_intv;//recording interval
    unsigned long _intv;
    ofstream _file;//file pointer
    vector<long double>_data_list;//temporary data buffer
};

#endif
