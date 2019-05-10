#include "ma.h"
#include "sv.h"
#include "cv.h"
#include "ModuloDados.h"

int main(int argc,  char* argv[]){
  if (argc==1||argc>2){int nada=write(0,"Segementation folte :(\n",24);nada++;}

  else if(argv[1][0]=='m'){
  maRun();
  }
  
  else if(argv[1][0]=='s'){
    svRun();
  }
  
  else if(argv[1][0]=='c'){
  cvRun();
  }

}
