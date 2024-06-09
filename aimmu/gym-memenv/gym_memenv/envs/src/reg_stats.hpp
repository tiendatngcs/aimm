#ifndef _REG_STATS_H_
#define _REG_STATS_H_

#include "stats.hpp"
#include "extern_vars.hpp"

class reg_stats{
  public:
    reg_stats(){}
    ~reg_stats(){}
    void create_stats();
    void update_stats();
    void display_stats(string file){
      g_stats.display_stats(file);
      //if(!multi_program){
      //g_stats_vec.display_stats_vec(file);
     // }
    }
    void clear_stats();
};

#endif
