#include "ma.h"

#define StockSize 4
#define PipeIndSize 7
#define outSize  12
#define infoSize 16
#define cacheSz 20

typedef struct serverinfo SvInfo;
typedef struct serveroutput SvOut;
typedef struct cachePrc Cacheprc;

int svRun();

int runSale(SvInfo ,int ,int, int ,int ,Cacheprc*);

int openPipeSv(char* );

int readline(int, char*);

char* getPipeID(int);

void printInfo(SvInfo*);

void printOut(SvOut*);







