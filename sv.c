#include "sv.h"

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

int readline(int fd,char*inpt){
  char c;
  int i;
  for(i=0;c!='\n';i++){
     read(fd,&c,1);
     inpt[i]=c;
  }
  inpt[i]='\0';
  return i;
}


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
    printf("info %ld output %ld\n",sizeof(struct serverinfo),sizeof(struct serveroutput));
  
  return pipeID;
}

void printInfo(SvInfo* svinfo){

  printf("Mode:%c, Art:%d, Units:%d, Client ID %d\n",svinfo->action,svinfo->artNr,svinfo->units,svinfo->clientID);
}

void printOut(SvOut* svout){

  printf("Runstat:%c, Stock:%d, Price:%f\n",svout->runstat,svout->stock,svout->price);
}



/*int getPipeStr(SvInfo args,char*strNr){
  int retVl=0;
  int nr= atoi(args.clientID);
  if (nr!=0){
    sprintf(strNr,"%d",nr);
  }
  return retVl;
}*/

int getStockPointer(int fdstk,int artNr){
   int offset=(artNr-1)*(sizeof(int));
   lseek(fdstk,offset,SEEK_SET);
}

void sendOutPut(char* pipeStr, char*output){
  int CvPipefd=openPipeSv(pipeStr);
  
  write(CvPipefd,output,outSize);    

}

void formatOutput(char*strS,char*strP,char*output){
  output[0]='i';
  strcpy(&output[1],strS);
  strcpy(&output[1+StockSize-1],strP);
}

void saleWrite(int fda,int fdsales,SvInfo args){
    
    float prc=getPrice(fda,args.artNr);
    prc=prc*args.units;
    char*strpr=malloc(priceSize+1);
    
    lseek(fdsales,0,SEEK_END);
    write(fdsales,&args.artNr,4);
    write(fdsales,&args.units,4);
    write(fdsales,&prc,4);
    //args[artIndSize ]=' ';
    //write(fdsales,args,artIndSize+1);
    
    //sprintf(strpr,"%d",unitch);
    //formatStr(strpr,StockSize-1,unitch);
    //strpr[StockSize-1]=' ';
    //write(fdsales,strpr,StockSize);
    
    //sprintf(strpr,"%.2f",prc);
    //formatPrice(strpr,priceSize);
    //strpr[priceSize]='\n';
    //puts(strpr);
    //write(fdsales,strpr,priceSize+1);

}

int runSale(SvInfo args,int fdstk,int fdpc,int fda,int fdsales){
    int readStatus;
    SvOut out;
    puts("running sale");
    //char*token=malloc(8);
    
      //token=strtok(&args[1]," ");
      //puts(args);
      //int artNr=atoi(token);
      
      if(args.artNr&&args.units){
          //token=strtok(NULL," ");
          //puts(token);
        //puts(args);
        
        //int i;
        //for(i=0;token[i]=='0';i++);
        
        //int unitChange=atoi(&token[i]);
        //printf("%d\n",unitChange);
          
          getStockPointer(fdstk,args.artNr);
          int uniCurr;
          int readStatus=read(fdstk,&uniCurr,StockSize);
          //puts(units);
          printf("readstatus=%d\n",readStatus);
          
          if(readStatus==0){
            puts("readstatus==0");
               if(args.units<0){
                 //write(fdpc,"ef             ",16);
                 out.runstat='e';
                 //return 0;
                 }
                 
             else{
               //printf("d\n");
             //write(fdpc,"s             ",16);
             out.runstat='s';
             //formatStr(units,StockSize,args.units);
             //puts(units);
             getStockPointer(fdstk,args.artNr);
             write(fdstk,&args.units,4);
             }
           }
          
          
          else{
            puts("changing sale");
            printf("unicurr, %d\n",uniCurr);
            if((args.units+uniCurr)<0){
                //write(fdpc,"ef             ",16);
                out.runstat='e';
            }
            else{
              if(args.units<0){
              
               saleWrite(fda,fdsales,args);
              
             }
            
             //write(fdpc,"s              ",16);
             out.runstat='s';
            
             uniCurr=uniCurr+args.units;
             //formatStr(units,StockSize-1,uniCurr);
             //units[6]='\n';
             getStockPointer(fdstk,args.artNr);
             write(fdstk,&uniCurr,4);
             out.stock=uniCurr;
             //write(fdstk,units,StockSize);
            }
           }
           write(fdpc,&out,outSize);
           printOut(&out);
        }
        else{
          out.runstat='e';
          write(fdpc,&out,outSize);
        }
      }

int runInfo(int fpa, int fpstk,int fdpc,SvInfo args){
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
   printf("read= %d, stk= %d\n",rd,stk);
   if (rd==0||stk==0){out.runstat='e';}
   
   else{
    puts("getting price");
    prc=getPrice(fpa,args.artNr);
    
    
    if (prc==0){
      out.runstat='e';
      }
    else{
     //char*output=malloc(17);
     //formatOutput(strS,strP,output);
     //puts(output);
     //int r=write(fdpc,output,16);
     //printf("%d\n",r);
     out.price=prc;
     out.runstat='i';
     out.stock=stk;
     
     //free(strP);
     }
    }
    int w= write(fdpc,&out,outSize);
    printf("could write %d\n",w);
    printOut(&out);
}

int svRun(){
    mkfifo("serverPipe",0600);
    int fdstk=openStocks();
    int fdsales=openSales();
    int fda=openArtigos();
    
    int fdpp=open("serverPipe",O_RDONLY);
    //char*args=malloc(25);
    int sz=sizeof(struct serverinfo);
    SvInfo args;
    int readStatus;
    char* pipeStr;
    int fdpc;

    while(1){
    printf("readstatus=%d ",readStatus);
    readStatus=read(fdpp,&args,sz);
    if (readStatus==0){sleep(5);}
    printf("%d\n",readStatus);
    

    if(readStatus!=0){
     //printf("%s\n",args);
     printInfo(&args); 
      
     //getPipeStr(args,pipeStr);
     pipeStr=getPipeID(args.clientID);
     puts(pipeStr);
     fdpc = openPipeSv(pipeStr); 

     if(args.action=='s'){
      runSale(args,fdstk,fdpc,fda,fdsales);
      }
     else if(args.action=='i'){
      runInfo(fda,fdstk,fdpc,args);
      }
     close(fdpc);
    }
    }
  

    //remove("serverPipe");
    return 0;

}