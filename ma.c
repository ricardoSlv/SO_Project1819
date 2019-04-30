#include "ma.h"

//To do:
//Validar args


int openArtigos(){
     return open("artigos.txt",O_RDWR|O_CREAT,0644);
}

int openStrings(){
    return open("strings.txt",O_RDWR|O_CREAT,0644);
}

void formatStr(char* str,int size,int nr){
 
  sprintf(str,"%d",nr);
  int len=strlen(str);
  strcpy(&str[size-len],str);
  
  for(int i=0;i<(size-len);i++){
    str[i]='0';
  }
}

void formatPrice(char* str,int size){

  int len=strlen(str);
  strcpy(&str[size-len],str);
  
  for(int i=0;i<(size-len);i++){
    str[i]='0';
  }
}


int addString(int fp,char* str){
  
  long tamanho=lseek(fp,0, SEEK_END);
  
  write(fp,str,strlen(str));
  write(fp,"\n",1);

  return tamanho;
}


int addArtigo(int fpa,int fps,char*strinput){
  char*token;

  long tamanho=lseek(fpa,0, SEEK_END);
  int numArts=(tamanho/artSize)+1;
  

  //Codigo de artigo
  char* strn=malloc(sizeof(char)*(artIndSize+1));
  formatStr(strn,artIndSize,numArts);
  write(fpa,strn,artIndSize);
  free(strn);
  write(fpa," ",1);

  //Nome
  token = strtok(strinput," ");
  int nr=addString(fps,token);
  char*strname=malloc(sizeof(char)*(nameIndSize+1));
  formatStr(strname,nameIndSize,nr);

  //Preço
  //validarpreço
  token = strtok(NULL,"\n");
  char*strpr=malloc(priceSize+1);
  strcpy(strpr,token);
  formatPrice(strpr,priceSize);
  write(fpa,strpr,priceSize);
  write(fpa," ",1);
  free(strpr);
  
  //Escrever indice nome
  write(fpa,strname,strlen(strname));
  write(fpa,"\n",1);
  free(strname);
}

//Atoi==0?
int seekNameByte(int fpa,int ind){
  lseek(fpa,(artSize*(ind-1)+artIndSize+priceSize+2),SEEK_SET);
  char*strNameInd=malloc(sizeof(char)*nameIndSize);
  //read(fpa,strNameInd,nameIndSize);
  return atoi(strNameInd);
}

void seekPriceByte(int fpa,int ind){
  lseek(fpa,(artSize*(ind-1)+artIndSize+1),SEEK_SET);
  char*strNameInd=malloc(sizeof(char)*nameIndSize);
}


float getPrice(int fpa,int ind){
   seekPriceByte(fpa,ind);
   char*strNameInd=malloc(sizeof(char)*nameIndSize);
   read(fpa,strNameInd,nameIndSize);
   float prc=atof(strNameInd);
   free(strNameInd);
   printf("%f",prc);
  return prc;
}

//free?
char* seekNameOnFile(int fps,int nbyte){
  lseek(fps,nbyte,SEEK_SET);
  char*name=malloc(100);
  read(fps,name,100);
  char*token=strtok(name,"\n");
}

void changeName(int fpa,int fps,char* input){
    char*token;
    
    token=strtok(input," ");
    int ind=atoi(token);
    int nb=seekNameByte(fpa,ind);

    token=strtok(NULL,"\n");
    int nr=addString(fps,token);
    char*c=malloc(nameIndSize+1);
    formatStr(c,nameIndSize,nr);
    write(fpa,c,strlen(c));
    free(c);

}

void changePrice(int fpa, char*input){
  char*token;
    
    token=strtok(input," ");
    int ind=atoi(token);
    seekPriceByte(fpa,ind);

    token=strtok(NULL,"\n");
    formatPrice(token,priceSize);
    write(fpa,token,priceSize);
}


int maRun(int readnr,char*input){

 int fpArts=openArtigos();
 int fpStrings=openStrings();

 if(input[0]=='i'){
   //if(input[1]!=' '){write(0,"Input failed\n",14);}
   //else{ 
     addArtigo(fpArts,fpStrings,&input[2]);
 }
 else if(input[0]=='n'){
 changeName(fpArts,fpStrings,&input[2]);
 }
 else if(input[0]=='p'){
 changePrice(fpArts,&input[2]);
 }

 return 0;
 }