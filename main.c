#include "ma.h"
#include "sv.h"
#include "cv.h"
int main(int argc,  char* argv[]){
  if (argc==1||argc>2){int nada=write(0,"Segementation folte :(\n",24);nada++;}

  else if(argv[1][0]=='m'){
  char* input=malloc(sizeof(char)*100);
  int readnr=read(0,input,100);
  maRun(readnr,input);
  free(input);
  }
  
  else if(argv[1][0]=='s'){
    svRun();
  }
  
  else if(argv[1][0]=='c'){
  cvRun();
  }

}
