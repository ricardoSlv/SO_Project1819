#include "cv.h"


//to do
//Testar se abriu
// da break com 1 input para s
int openPipeCv(char*pipeNr){
   
   mkfifo(pipeNr,0600);
    
    int fdpc=open(pipeNr,O_RDONLY);
    return fdpc;
}

char* getPipeID(int tme){

    char*pipeID=malloc(8);
    sprintf(pipeID,"%d",tme);
  
  return pipeID;
}

int validateInputS(char*input){  
   int r=1;
   char*values=strdup(input);
   strtok(values," \n");
   char*token=strtok(NULL," \n");
   if(atoi(values)==0||atof(token)==0){r = 0;}
   free (values);
   return r;
}

int validateInputI(char*input){
 int r=1;
   char*values=strdup(input);
   strtok(values,"\n");
   r=atoi(values);
   free(values);
   return r;
}

char* formatPipe(char mode, char* PipeID){
   char* input=malloc(24);
   strcpy(&input[1+artIndSize+1+StockSize+1],PipeID);
   input[0]=mode;
   for(int i=1;i<1+artIndSize+1+StockSize;i++){input[i]=' ';}
   return input;
}

char* formatReqS(char*input,int tme){
   
   char*values=strdup(input);
   //puts(values);
   strtok(values," ");
   char*token=strtok(NULL,"\n");
   //puts(values);puts(token);
   int artNr=atoi(values);
   int chNr=atoi(token);
   //printf("%d, %d\n",artNr,chNr);

   values=malloc(artNr+1);
   token=malloc(StockSize);

   formatStr(values,artIndSize,artNr);
   formatStr(token,StockSize,chNr);
   int v=strlen(values);
   values[v]=' ';
   v=strlen(token);
   token[v]='\n';
   //printf("%s",values);
   //puts(token);
   char*formInput=malloc(24);
   formInput[0]='s';
   strcpy(&formInput[1],values);
   formInput[artIndSize+1]=' ';
   strcpy(&formInput[artIndSize+2],token);
   formInput[artIndSize+2+StockSize]=' ';
   sprintf(&formInput[artIndSize+2+StockSize+1],"%d",tme);
   //puts(formInput);
   free (values);

   return formInput;
}

char* formatReqI(char*input,int tme){
   char*values=strdup(input);
   //puts(values);
   strtok(values," \n");
   //puts(values);
   int artNr=atoi(values);

   //printf("%d\n",artNr);

   values=malloc(artNr+1);

   formatStr(values,artIndSize,artNr);
   int v=strlen(values);
   values[v]=' ';

 char*formInput=malloc(24);
   formInput[0]='i';
   strcpy(&formInput[1],values);
   formInput[artIndSize+1]=' ';
   for(int i=artIndSize+2;i<artIndSize+2+StockSize+1;i++){formInput[i]=' ';}
   sprintf(&formInput[artIndSize+2+StockSize+1],"%d",tme);
   //formInput[24]='\n';
   return formInput;
}

int cvRun(){
  //int i=strlen(input);
  //printf("%d\n",i);
  
   
   int fdw=open("serverPipe",O_WRONLY);
   
   int tme=time(NULL);
   tme=tme-(tme/10000000)*10000000;

   //printf("%d\n",tme);
   char*PipeID=getPipeID(tme);
   char*pipeInput=formatPipe('p',PipeID);
   printf("%s\n",pipeInput);
   int openpp=0;
   int pipefd;
   char*formInput;
   char*output=malloc(16);
   char* input=malloc(sizeof(char)*100);
   int readnr;
   write(0,"Insira o input (->s artigos difStock ou  ->i artigo)\n",54);

   while(input[0]!='q'){   
   readnr=read(0,input,100);
   puts(input);
   input[16]='\0';
   input[17]=' ';
   input[18]=' ';
   
    if(input[0]=='s'){ 
     if(validateInputS(&input[2])==0){
         write(0,"Input inválido",16);
         
     }
     else{
     formInput=formatReqS(&input[2],tme);
     puts(formInput);
     write(fdw,formInput,24);
     pipefd=openPipeCv(PipeID);
     read(pipefd,output,16);
     close(pipefd);
     puts(output);}
    }

    else if(input[0]=='i'){
        if(validateInputI(&input[1])==0){
         write(0,"Input inválido\n",17);
     }
     else{
        formInput=formatReqI(&input[1],tme);
        write(fdw,formInput,24);
        pipefd=openPipeCv(PipeID);
        read(pipefd,output,16);
        
        close(pipefd);
        printf("%s\n",formInput);
        puts(output);
        

        }
     free(formInput);
     
    }
    else if(input[0]=='q'){
       write(0,"Quit input\n",12);
    }
    else{
       write(0,"Input inválido\n",17);
   
    }
    
    
    //puts("quasefim");
    
    
    //int pipdd=openPipeSv(PipeID);
    /*write(fdw,formInput,24);
    read(pipefd,output,16);
    close(pipefd);
    puts(output);*/
   }

    //free(formInput);
    free(input);
    close(fdw);
    close(pipefd);
    remove(PipeID);

    return 0;

    

}