#include "cv.h"

int main(){
  int fd=open("Testes.txt",0644);
  dup2(fd,0);
  execlp("./exec","./exec","c",(char *) NULL);
  printf("Did not execute");
  return 0;
}