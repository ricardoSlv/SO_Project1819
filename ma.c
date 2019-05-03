#include "ma.h"

//To do:
//Validar args

struct produto{
  int codigo;
  float price;
  int namezone;
};


int openArtigos(){
     return open("artigos.txt",O_RDWR|O_CREAT,0644);
}

int openStrings(){
    return open("strings.txt",O_RDWR|O_CREAT,0644);
}

int checkValidProd(int fda, int ind){
  lseek(fda,0,SEEK_SET);
  int size=lseek(fda,0,SEEK_END);
  
  return ((ind-1)*artSize)<size ? 1:0;
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
  struct produto prd;
  

  //Codigo de artigo
  //char* strn=malloc(sizeof(char)*(artIndSize+1));
  //formatStr(strn,artIndSize,numArts);
  
  //write(fpa,strn,artIndSize);
  //free(strn);
  //write(fpa," ",1);

  //Nome
  token = strtok(strinput," ");
  int nr=addString(fps,token);
  
  //char*strname=malloc(sizeof(char)*(nameIndSize+1));
  //formatStr(strname,nameIndSize,nr);

  //Preço
  //validarpreço
  token = strtok(NULL,"\n");
  float preco=atof(token);
  
  //char*strpr=malloc(priceSize+1);
  //strcpy(strpr,token);
  //formatPrice(strpr,priceSize);
  //write(fpa,strpr,priceSize);
  //write(fpa," ",1);
  //free(strpr);
  
  //Escrever indice string
  //write(fpa,strname,strlen(strname));
  //write(fpa,"\n",1);
  //free(strname);
  if(numArts&&preco&&nr!=0){
  prd.codigo=numArts;
  prd.price=preco;
  prd.namezone=nr;

  int sz=sizeof(struct produto);
  write(fpa,&prd,sz);
  lseek(fpa,-sz,SEEK_CUR);
  read(fpa,&prd,sz);
  printf("%d,%f,%d,%d\n",prd.codigo,prd.price,prd.namezone,sz);
  write(0,"sucess\n",8);
  return 1;
  }
  else{
    write(0,"Input inválido\n",17);
    return 0;
  }
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
   
   //seekPriceByte(fpa,ind);
   //char*strNameInd=malloc(sizeof(char)*nameIndSize);
   //read(fpa,strNameInd,nameIndSize);
   //float prc=atof(strNameInd);
   //free(strNameInd);
   lseek(fpa,(ind-1)*artSize,SEEK_SET);
   struct produto prd;
   read(fpa,&prd,artSize); 
   
   printf("%f\n",prd.price);
  return prd.price;
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
    //int nb=seekNameByte(fpa,ind);
    //Não passar do fim
    if(checkValidProd(fpa,ind)){
    lseek(fpa,(ind-1)*(artSize),SEEK_SET);
    
    struct produto prd;
    read(fpa,&prd,artSize);
    printf("Antes:%d,%f,%d\n",prd.codigo,prd.price,prd.namezone);
    

    token=strtok(NULL,"\n");
    int nr=addString(fps,token);
    lseek(fpa,-artSize,SEEK_CUR);
    prd.namezone=nr;
    
    write(fpa,&prd,artSize);
    lseek(fpa,-artSize,SEEK_CUR);
    read(fpa,&prd,artSize);
    printf("Depois:%d,%d,%f\n",prd.codigo,prd.namezone,prd.price);
    }
    else{
      write(0,"Produto não válido\n",21);
    }
    //char*c=malloc(nameIndSize+1);
    //formatStr(c,nameIndSize,nr);
    //write(fpa,c,strlen(c));
    //free(c);

}

void changePrice(int fpa, char*input){
   char*token;
    
    token=strtok(input," ");
    int ind=atoi(token);
    //int nb=seekNameByte(fpa,ind);
    //Não passar do fim
    if(checkValidProd(fpa,ind)){
    lseek(fpa,(ind-1)*(artSize),SEEK_SET);
    
    struct produto prd;
    read(fpa,&prd,artSize);
    printf("Antes:%d,%f,%d\n",prd.codigo,prd.price,prd.namezone);
    

    token=strtok(NULL,"\n");
    float prc=atof(token);
    printf("preco=%s %f\n",token,prc);
    lseek(fpa,-artSize,SEEK_CUR);
    prd.price=prc;
    
    write(fpa,&prd,artSize);
    lseek(fpa,-artSize,SEEK_CUR);
    read(fpa,&prd,artSize);
    printf("Depois:%d,%f,%d\n",prd.codigo,prd.price,prd.namezone);
    }
    else{
      write(0,"Produto não válido\n",21);
    }
}


int maRun(){

 int fpArts=openArtigos();
 int fpStrings=openStrings();
 char* input=malloc(sizeof(char)*100);
 int readnr;
 write(0,"Comando: i+nome+preço, n+art+newname, p+art+newprice\n",55);

 while(input[0]!='q'){
  
  readnr=read(0,input,100);
 if(input[0]=='i'){ 
     addArtigo(fpArts,fpStrings,&input[2]);
 }
 else if(input[0]=='n'){
 changeName(fpArts,fpStrings,&input[2]);
 }
 else if(input[0]=='p'){
 changePrice(fpArts,&input[2]);
 }
 else if (input[0]=='q'){
   write(0,"Quit input\n",12);
 }
 }

 free(input);
 return 0;
 }