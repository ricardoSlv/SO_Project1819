#include <sys/wait.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>    
#include <math.h>
#include <time.h>
#include <signal.h>

#define artSize 12 
#define StockSize 4
#define PipeIndSize 7
#define outSize  12
#define infoSize 16
#define cacheSz 20
#define buffsize 110
#define nthreads 4
#define salesize 12

struct produto{
  int codigo;
  float price;
  int namezone;
};

typedef struct  serverinfo
{
  char action;
  int artNr;
  int units;
  int clientID;
}SvInfo;

typedef struct serveroutput{
   char runstat;
   int stock;
   float price;
}SvOut;

typedef struct sale{
  int artnr;
  int units;
  float price;
}Venda;

int readline(int, char*);

char* getPipeID(int);

void printInfo(SvInfo*);

void printOut(SvOut*);

void printSale(struct sale);

int openArtigos();

float getPrice(int,int);