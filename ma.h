#include <unistd.h>     /*chamadas ao sistema: defs e decls essenciais*/
#include <sys/wait.h>   /*chamadas wait*() e macros relacionadas*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>    /*O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <math.h>

#define artIndSize 6
#define priceSize 7
#define nameIndSize 8
#define artSize 24 

int openArtigos();

void formatStr(char* ,int,int);

void formatPrice(char* str,int size);

float getPrice(int,int);

void seekPriceByte(int,int);

int maRun(int , char*);
