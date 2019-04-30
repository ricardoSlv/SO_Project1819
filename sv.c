#include "sv.h"

//3 args, s-sales,r- reposição de stock, i-info, c- price change(inativo);
// to do ver se 7º carater do arg é space;

int openStocks(){
    return open("stocks.txt",O_RDWR|O_CREAT,0644);
}

int openSales(){
    return open("vendas.txt",O_RDWR|O_CREAT,0644);
}

int openPipeSv(char*pipeNr){
   
   mkfifo(pipeNr,0600);
    int fdpc=open(pipeNr,O_WRONLY);
    return fdpc;
}



int getPipeStr(char* input,char*strNr){
  int retVl=0;
  int nr= atoi(&input[1+artIndSize+1+StockSize+1]);
  if (nr!=0){
    sprintf(strNr,"%d",nr);
  }
  return retVl;
}

int getStockPointer(int fdstk,int artNr){
   int offset=(artNr-1)*StockSize;
   lseek(fdstk,offset,SEEK_SET);
}

void sendOutPut(char* pipeStr, char*output){
  int CvPipefd=openPipeSv(pipeStr);
  
  write(CvPipefd,output,16);    

}

void formatOutput(char*strS,char*strP,char*output){
  output[0]='i';
  strcpy(&output[1],strS);
  strcpy(&output[1+StockSize-1],strP);
}

void saleWrite(int fda,int fdsales,int unitch,int artNr,char*args){
    
    float prc=getPrice(fda,artNr);
    prc=prc*unitch;
    char*strpr=malloc(priceSize+1);
    
    lseek(fdsales,0,SEEK_END);
    args[artIndSize ]=' ';
    write(fdsales,args,artIndSize+1);
    
    sprintf(strpr,"%d",unitch);
    formatStr(strpr,StockSize-1,unitch);
    strpr[StockSize-1]=' ';
    write(fdsales,strpr,StockSize);
    
    sprintf(strpr,"%.2f",prc);
    formatPrice(strpr,priceSize);
    strpr[priceSize]='\n';
    //puts(strpr);
    write(fdsales,strpr,priceSize+1);
}

int runSale(char*args,int fdstk,int fdpc,int fda,int fdsales){
    int readStatus;
    char*token=malloc(8);
    
      token=strtok(&args[1]," ");
      //puts(args);
      int artNr=atoi(token);
      
      if(artNr!=0){
          token=strtok(NULL," ");
          puts(token);
        //puts(args);
        
        int i;
        for(i=0;token[i]=='0';i++);
        
        int unitChange=atoi(&token[i]);
        printf("%d\n",unitChange);
        
        if(unitChange!=0){
          
          getStockPointer(fdstk,artNr);
          char* units=malloc(StockSize);  
          int readStatus=read(fdstk,units,StockSize);
          //puts(units);
          
          if(readStatus==0){
               if(unitChange<0){
                 write(fdpc,"ef             ",16);
                 return 0;
                 }
             //printf("d\n");
             write(fdpc,"s             ",16);
             formatStr(units,StockSize,unitChange);
             //puts(units);
             getStockPointer(fdstk,artNr);
             write(fdstk,units,StockSize);
           }
          
          else{
            int uniCurr=atoi(units);
            if((unitChange+uniCurr)<0){
                write(fdpc,"ef             ",16);
                return 0;
            }
            
            if(unitChange<0){
              int salenr = abs(unitChange);
              saleWrite(fda,fdsales,salenr,artNr,&args[1]);
              
            }
            
            write(fdpc,"s              ",16);
            uniCurr=uniCurr+unitChange;
            formatStr(units,StockSize-1,uniCurr);
            units[6]='\n';
            getStockPointer(fdstk,artNr);
            write(fdstk,units,StockSize);
           }
           free(units);
        }
      }
}

int runInfo(int fpa, int fpstk,int fdpc,char* args){
   strtok(&args[1]," ");
   int artNr=atoi(&args[1]);
   getStockPointer(fpstk,artNr);
   seekPriceByte(fpa,artNr);
   char* strS = malloc(StockSize);
   strS[StockSize-1]='\0';
   char* strP = malloc(priceSize+1);
   strP[priceSize]='\0';

   int rd=read(fpstk,strS,StockSize-1);
   int nv=atoi(strS);
   if (rd==0||nv==0){write(fdpc,"ef             ",16);return 0;}
   
   rd=read(fpa,strP,priceSize);
   nv=atof(strP);
   if (rd==0||nv==0){write(fdpc,"en             ",16);return 0;}
   
   char*output=malloc(17);
   formatOutput(strS,strP,output);
   puts(output);
   int r=write(fdpc,output,16);
   printf("%d\n",r);
   
   free(strS);
   free(strP);
}

int svRun(){
    mkfifo("serverPipe",0600);
    int fdstk=openStocks();
    int fdsales=openSales();
    int fda=openArtigos();
    
    int fdpp=open("serverPipe",O_RDONLY);
    char*args=malloc(25);
    int readStatus;
    char* pipeStr=malloc(8);

    while(readStatus!=0){
    printf("readstatus=%d ",readStatus);
    readStatus=read(fdpp,args,24);
    printf("%d\n",readStatus);
    int fdpc;

    if(readStatus!=0){
     printf("%s\n",args); 
     
     getPipeStr(args,pipeStr);
     puts(pipeStr);
     fdpc = openPipeSv(pipeStr); 

     if(args[0]=='s'){
      runSale(args,fdstk,fdpc,fda,fdsales);
      }
     else if(args[0]=='i'){
      runInfo(fda,fdstk,fdpc,args);
      }
     close(fdpc);
    }
    }
  

    return 0;

}