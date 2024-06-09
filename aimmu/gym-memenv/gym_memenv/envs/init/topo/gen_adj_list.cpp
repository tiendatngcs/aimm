#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char**argv){
  int dim = atoi(argv[1]);
  for(int i=0;i<dim;i++){
    for(int j=0;j<dim;j++){
      if(i==0){//row 0 | nothing is on top
        if(j==0){//left top corner
          cout<<j+1<<" "<<i*dim + j+dim<<endl;
        }
        else if(j<(dim-1)){
          cout<<i*dim + j-1<<" "<<i*dim + j+1<<" "<<i*dim + j+dim<<endl;
        }
        else{//top right corner
          cout<<i*dim + j-1<<" "<<i*dim + j+dim<<endl;
        }
      }
      else if(i<(dim-1)){
        if(j==0){//left top corner
          cout<<i*dim + j-dim<<" "<<i*dim + j+1<<" "<<i*dim + j+dim<<endl;
        }
        else if(j!=(dim-1)){
          cout<<i*dim + j-dim<<" "<<i*dim + j-1<<" "<<i*dim + j+1<<" "<<i*dim + j+dim<<endl;
        }
        else{//top right corner
          cout<<i*dim + j-dim<<" "<<i*dim + j-1<<" "<<i*dim + j+dim<<endl;
        }
      }
      else{
        if(j==0){//left top corner
          cout<<i*dim + j-dim<<" "<<i*dim + j+1<<endl;
        }
        else if(j<(dim-1)){
          cout<<i*dim + j-dim<<" "<<i*dim + j-1<<" "<<i*dim + j+1<<endl;
        }
        else{//top right corner
          cout<<i*dim + j-dim<<" "<<i*dim + j-1<<endl;
        }
      }
    }
  }
}
