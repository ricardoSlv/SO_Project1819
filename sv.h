#include "ma.h"

#define StockSize 4
#define PipeIndSize 7
#define outSize  12
#define infoSize 16

typedef struct serverinfo SvInfo;
typedef struct serveroutput SvOut;

int svRun();

int runSale(SvInfo ,int ,int, int ,int );

int openPipeSv(char* );

int readline(int, char*);

char* getPipeID(int);

void printInfo(SvInfo*);

void printOut(SvOut*);







