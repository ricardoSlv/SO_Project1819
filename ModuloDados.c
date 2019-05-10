#include "ModuloDados.h"

void printInfo(SvInfo* svinfo){

  printf("Mode:%c, Art:%d, Units:%d, Client ID %d\n",svinfo->action,svinfo->artNr,svinfo->units,svinfo->clientID);
}

void printOut(SvOut* svout){

  printf("Runstat:%c, Stock:%d, Price:%f\n",svout->runstat,svout->stock,svout->price);
}

void printSale(struct sale vnd){
  printf("Art %d Units %d Price %.2f\n",vnd.artnr,vnd.units,vnd.price);
}

float getPrice(int fpa,int ind){
   
   lseek(fpa,(ind-1)*artSize,SEEK_SET);
   struct produto prd;
   int rdn=read(fpa,&prd,artSize); 
   if (rdn==0){prd.price=-1;}
   
   //printf("%f\n",prd.price);
  return prd.price;
}

int readline(int fd,char*inpt){
  char c='O';
  int i;
  for(i=0;c!='\n'&&read(fd,&c,1);i++){
     inpt[i]=c;
  }
  inpt[i]='\0';
  return i;
}

int openArtigos(){
     return open("artigos.txt",O_RDWR|O_CREAT,0644);
}

char* getPipeID(int tme){

    char*pipeID=malloc(8);
    sprintf(pipeID,"%d",tme);
  
  return pipeID;
}