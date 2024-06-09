#ifndef STAT_H
#define STAT_H

#include<iostream>
#include<string.h>
#include<vector>
#include<map>
#include<unordered_map>

using namespace std;

template <typename T>
class stats{
  public:
    FILE *stats_file;
    stats<T>();
    ~stats<T>();
    
    map<string, T>stat_tab;
    map<string, T>stat_tab_vec;//for vector type stats
    map<string, string>description;
    map<string, string>description_vec;//for vector type stats
    
    int _rollovers_stats;    
    int _rollovers_stats_vec;    
    
    void create_stats(string);
    void create_stats(string,string);//with description
    void create_stats_vec(int,string,string);//with description
    void incr(string, T);
    void incr_vec(string, T);//for vector type stats
    void update(string, T);
    void update_vec(string, T);//for vector type stats
    T value(string);
    void display_stats(string);
    void display_stats_vec(string);
    void clear();
};

template <typename T>
void stats<T>::clear(){
  stat_tab.clear();
  description.clear();
}

template <typename T>
stats<T>::~stats(){
  cout<<"[STATS] *** destructor empty ***"<<endl; 
}

template <typename T>
stats<T>::stats(){
  _rollovers_stats = 0;
  _rollovers_stats_vec = 0;
}

template <typename T>
void stats<T>::create_stats(string vname){
  auto it = stat_tab.find(vname);
  if(it!=stat_tab.end()){
    fprintf(stderr,"\nERROR:: *** duplicate creation of stats variable name \"%s\" *** \n\n",vname.c_str());
    exit(0);
  }
  else{
    stat_tab.insert(pair<string,T>(vname,0));
  }
}

template <typename T>
void stats<T>::create_stats(string vname, string desc){
  auto it = stat_tab.find(vname);
  if(it!=stat_tab.end()){
    fprintf(stderr,"\nERROR:: *** duplicate creation of stats variable name \"%s\" *** \n\n",vname.c_str());
    exit(0);
  }
  else{
    stat_tab.insert(pair<string,T>(vname,0));
    description.insert(pair<string,string>(vname,desc));
  }
}

template <typename T>
void stats<T>::create_stats_vec(int size, string vname, string desc){
  auto it = stat_tab_vec.find(vname);
  if(it!=stat_tab_vec.end()){
    fprintf(stderr,"\nERROR:: *** duplicate creation of stats variable name \"%s\" *** \n\n",vname.c_str());
    exit(0);
  }
  else{
    //cout<<"creating stats vector of size: "<<size<<endl;
    vector<long double> vec;
    vec.resize(size);
    stat_tab_vec.insert(pair<string,T>(vname,vec));
    description_vec.insert(pair<string,string>(vname,desc));
  }
}

template <typename T>
void stats<T>::incr(string vname, T incr_by){
  auto it = stat_tab.find(vname);
  if(it!=stat_tab.end()){
    //stat_tab[vname] = stat_tab[vname] + incr_by;
    it->second = it->second + incr_by;
  }
  else{
    fprintf(stderr,"\nERROR:: *** wrong stats variable name \"%s\" *** \n\n",vname.c_str());
    exit(0);
  }
}

template <typename T>
void stats<T>::incr_vec(string vname, T incr_by){
  auto it = stat_tab_vec.find(vname);
  if(it!=stat_tab.end()){
    //stat_tab[vname] = stat_tab[vname] + incr_by;
    it->second = it->second + incr_by;
  }
  else{
    fprintf(stderr,"\nERROR:: *** wrong stats variable name \"%s\" *** \n\n",vname.c_str());
    exit(0);
  }
}

template <typename T>
void stats<T>::update(string vname, T updated_value){
  auto it = stat_tab.find(vname);
  if(it!=stat_tab.end()){
    //stat_tab[vname] = updated_value;
    it->second = updated_value;
  }
  else{
    fprintf(stderr,"\nERROR:: *** wrong stats variable name \"%s\" *** \n\n",vname.c_str());
    exit(0);
  }
}

template <typename T>
void stats<T>::update_vec(string vname, T updated_value){
  auto it = stat_tab_vec.find(vname);
  if(it!=stat_tab_vec.end()){
    //stat_tab[vname] = updated_value;
    //cout<<"size: "<<it->second.size()<<endl;
    for(int i=0; i<updated_value.size(); i++){
      it->second[i] = updated_value[i];
    }
  }
  else{
    fprintf(stderr,"\nERROR:: *** wrong stats variable name \"%s\" *** \n\n",vname.c_str());
    exit(0);
  }
}

template <typename T>
T stats<T>::value(string vname){
  return stat_tab[vname];
}

template <typename T>
void stats<T>::display_stats(string filename){
  
  string path_to_stats = "stats/" + filename + "/stats-" + to_string(_rollovers_stats) + ".out";
  
  _rollovers_stats++;
  
  stats_file = fopen(path_to_stats.c_str(),"w");
  if(stats_file==nullptr){
    cout<<"\n ***[stats.hpp] The stats file can not be created --> exiting the simulation !!! ***\n "<<endl;
    exit(0);
  }
  fprintf(stats_file,"\n-------------------------------------------------------------\n");
  fprintf(stats_file,"Display Stats:");
  fprintf(stats_file,"\n-------------------------------------------------------------\n");
  for(auto& t:stat_tab){
    auto d = description.find(t.first);
    if(d!=description.end()){
      fprintf(stats_file, "%s : %Lf\t\t\t\t\t\t| %s\n",(t.first).c_str(),(long double)t.second,d->second.c_str());
    }
    else{
      fprintf(stats_file, "%s : %Lf\n",(t.first).c_str(),(long double)t.second);
    }
  }
  cout<<"finished writing in the stats file ..."<<endl;
  fclose(stats_file);
}

template <typename T>
void stats<T>::display_stats_vec(string filename){
  string path_to_stats = "stats/" + filename + "/stats_vec-"+ to_string(_rollovers_stats_vec) + ".out";

  _rollovers_stats_vec++;

  stats_file = fopen(path_to_stats.c_str(),"w");
  if(stats_file==nullptr){
    cout<<"\n ***[stats.hpp] The stats file can not be created --> exiting the simulation !!! ***\n "<<endl;
    exit(0);
  }
  fprintf(stats_file,"\n-------------------------------------------------------------\n");
  fprintf(stats_file,"Display Vector Stats:");
  fprintf(stats_file,"\n-------------------------------------------------------------\n");
  for(auto& t:stat_tab_vec){
    auto d = description_vec.find(t.first);
    if(d!=description_vec.end()){
      for(int i=0; i<t.second.size(); i++){  
        fprintf(stats_file, "%s[%d] : %0.10Lf\t\t\t\t\t\t| %s\n",(t.first).c_str(),i,(long double)t.second[i],d->second.c_str());
      }
    }
    else{
      for(int i=0; i<t.second.size(); i++){  
        fprintf(stats_file, "%s[%d] : %Lf\n",(t.first).c_str(),i,(long double)t.second[i]);
      }
    }
  }
  cout<<"finished writing in the vector stats file ..."<<endl;
  fclose(stats_file);
}
#endif
