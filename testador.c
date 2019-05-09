#include "cv.h"

int main(int argc, char*argv[]){
  int fdm=open("Testema.txt",0644);
  int fdc=open("Testecv.txt",0644);
  
  if(argv[1][0]=='m'){
  dup2(fdm,0);
  execlp("./exec","./exec","m",(char *) NULL);
  printf("Did not execute");
  }
  else if(argv[1][0]=='c'){
  execlp("./exec","./exec","c",(char *) NULL);
  printf("Did not execute");
  }
  return 0;
}