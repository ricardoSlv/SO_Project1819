#include "sv.h"
#include <signal.h>

//3 args, s-sales,r- reposição de stock, i-info, c- price change(inativo);
// to do existe e fore de stock e não existe ;


typedef struct  serverinfo
{
  char action;
  int artNr;
  int units;
  int clientID;

};

typedef struct serveroutput{
   
   char runstat;
   int stock;
   float price;

};

typedef struct sale{
  int artnr;
  int units;
  float price;
}Venda;

typedef struct artprc{
  int artnr;
  float prc;
}ArtPrc;

typedef struct cachePrc{
  int ind;
  ArtPrc prices[cacheSz];
}Cacheprc;


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

char* getPipeID(int tme){

    char*pipeID=malloc(8);
    sprintf(pipeID,"%d",tme);
  
  return pipeID;
}

void printInfo(SvInfo* svinfo){

  printf("Mode:%c, Art:%d, Units:%d, Client ID %d\n",svinfo->action,svinfo->artNr,svinfo->units,svinfo->clientID);
}

void printOut(SvOut* svout){

  printf("Runstat:%c, Stock:%d, Price:%f\n",svout->runstat,svout->stock,svout->price);
}

int getStockPointer(int fdstk,int artNr){
   int offset=(artNr-1)*(sizeof(int));
   lseek(fdstk,offset,SEEK_SET);
}

void sendOutPut(char* pipeStr, char*output){
  int CvPipefd=openPipeSv(pipeStr);
  
  write(CvPipefd,output,outSize);    

}

void runAg(){
   
   Venda sale;
   char*stringsale=malloc(40);
   int sz;
   int fds = openSales();
   int fdsa=open("vendasAg.txt",O_RDWR|O_CREAT,0644);

   while(read(fds,&sale,12)!=0){
       puts("li 1");
       
        sprintf(stringsale,"%d %d %f\n",sale.artnr,sale.units,sale.price);
        sz=strlen(stringsale);
        write(fdsa,stringsale,sz); 
   }
   sleep(15);
     remove("vendasAg.txt"); 

   
   if(fork()==0){
     dup2(fds,0);
     execlp("./ag","./exec","c",(char *) NULL);
   }
}

float Cached(int artNr,Cacheprc* prodlist){
    float r=0;
    puts("searching cache");
    for(int i=0;i<20;i++){
       if(prodlist->prices[i].artnr==artNr){
         r=prodlist->prices[i].prc;
         printf("cached r=%f\n",r);
         break;
       }
    }
   return r; 
}

void cacheAdd(ArtPrc prd,Cacheprc* prodlist){
  
  prodlist->prices[prodlist->ind]=prd;
  prodlist->ind=(prodlist->ind+1)%cacheSz;
}

float seekPrice(int fpa,int artNr,Cacheprc* prodlist){
    float prc;
    float cached=Cached(artNr,prodlist);
    if(cached){
      prc=cached;
    }
    else
    {
      cached=getPrice(fpa,artNr);
       if(cached>0){
         ArtPrc prd;
         prd.artnr=artNr;
         prd.prc=cached;
         printf("im adding %d, %f\n",artNr,cached);
         cacheAdd(prd,prodlist);
       }
    }
    return cached;
    
}

void formatOutput(char*strS,char*strP,char*output){
  output[0]='i';
  strcpy(&output[1],strS);
  strcpy(&output[1+StockSize-1],strP);
}

void saleWrite(int fda,int fdsales,SvInfo args){
    
    struct sale venda;
    float prc=getPrice(fda,args.artNr);
    if(args.units>0){prc=0;}
    else{prc=prc*args.units;}
    venda.artnr=args.artNr;
    venda.units=args.units;
    venda.price=prc;

    lseek(fdsales,0,SEEK_END);
    write(fdsales,&venda,sizeof(venda));
}

int runSale(SvInfo args,int fdstk,int fdpc,int fda,int fdsales,Cacheprc* prodlist){
    int readStatus;
    SvOut out;
    //puts("running sale");

    if(seekPrice(fda,args.artNr,prodlist)<=0){
      out.runstat='e';
      write(fdpc,&out,outSize);
      return 0;
    }

    else if(args.artNr&&args.units){

          getStockPointer(fdstk,args.artNr);
          int uniCurr;
          int readStatus=read(fdstk,&uniCurr,StockSize);
          
          if(readStatus==0){
            if(args.units<0){
                 out.runstat='e';
            }
                 
             else{
             out.runstat='s';
             getStockPointer(fdstk,args.artNr);
             write(fdstk,&args.units,4);
             }
          }
          
          
          else{
            if((args.units+uniCurr)<0){
                out.runstat='e';
            }
            else{

             saleWrite(fda,fdsales,args);
             out.runstat='s';
            
             uniCurr=uniCurr+args.units;
             getStockPointer(fdstk,args.artNr);
             write(fdstk,&uniCurr,4);
             out.stock=uniCurr;
            }
          }

      write(fdpc,&out,outSize);
      printOut(&out);
    }
    
    else{
     out.runstat='e';
     write(fdpc,&out,outSize);
     printOut(&out);
    }
      
}

int runInfo(int fpa, int fpstk,int fdpc,SvInfo args,Cacheprc* prodlist){
   //strtok(&args[1]," ");
   //int artNr=atoi(&args[1]);
   SvOut out;
   getStockPointer(fpstk,args.artNr);
   //seekPriceByte(fpa,artNr);
   int stk;
   float prc;

   //char* strP = malloc(priceSize+1);
   //strP[priceSize]='\0';

   int rd=read(fpstk,&stk,StockSize);
   //printf("read= %d, stk= %d\n",rd,stk);
  
   if (rd==0){stk==0;}
   
    //puts("getting price");
    prc=seekPrice(fpa,args.artNr,prodlist);
    
    
    if (prc<=0){out.runstat='e';}
    
    else{
     out.price=prc;
     out.runstat='i';
     out.stock=stk;
    }

    int w= write(fdpc,&out,outSize);
    printOut(&out);
}

void ShutdownRun(){
  remove("serverPipe");
  puts("exiting");
  exit(1);
}

int svRun(){
   
    signal(SIGINT,ShutdownRun);
    signal(SIGTERM,ShutdownRun);

    mkfifo("serverPipe",0600);
    int fdstk=openStocks();
    int fdsales=openSales();
    int fda=openArtigos();
    
    int fdpp=open("serverPipe",O_RDONLY);
    int sz=sizeof(struct serverinfo);
    SvInfo args;
    int readStatus;
    char* pipeStr;
    int fdpc;
    Cacheprc* prodlist=malloc(sizeof(struct cachePrc));
    prodlist->ind=0;

  while(1){
    readStatus=read(fdpp,&args,sz);
    
    if (readStatus==0){
      write(1,"Server sleeping\n",17);
      sleep(5);
      }
    
    else{
     printInfo(&args); 
    
     pipeStr=getPipeID(args.clientID);
     fdpc = openPipeSv(pipeStr); 
     
     if(args.action=='s'){
      runSale(args,fdstk,fdpc,fda,fdsales,prodlist);
      }
     else if(args.action=='i'){
      runInfo(fda,fdstk,fdpc,args,prodlist);
      }
      
     close(fdpc);
    }
  }
    
    return 0;

}