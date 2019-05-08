#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>   /*chamadas wait*() e macros relacionadas*/
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>    /*O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <math.h>

typedef struct sale{
  int artnr;
  int units;
  float price;
}Venda;

void printSale(Venda sale){
  printf("Art %d Units %d Price %.2f\n",sale.artnr,sale.units,sale.price);
}

int main(int argc, char* argv[]){
   
   time_t t;
   time(&t);
   printf("\n current time is : %s",ctime(&t));
   
   int fdagrtemp=open("tempFileOfAgr",O_RDWR|O_CREAT,0644);
   int fdagrfinal=open(ctime(&t),O_WRONLY|O_CREAT,0644);
   int fdsales=open("vendasAg.txt",O_RDONLY|O_CREAT,0644);
   
   printf("%s\n",argv[1]);
   Venda sale;
   Venda saletemp;
   char*writing=malloc(50);

   while((read(fdsales,&sale,12)>0)){
     puts("salefile");printSale(sale);
      lseek(fdagrtemp,(sale.artnr-1)*12,SEEK_SET);
     
      if(read(fdagrtemp,&saletemp,12)>0){
        
       if(saletemp.artnr>0&&sale.units<0){
        puts("agfileanteshavia");printSale(saletemp);
        saletemp.units=saletemp.units+sale.units;
        saletemp.price=saletemp.price+sale.price;
        puts("agfiledepshavia");printSale(saletemp);
       }
       else if (sale.units<0){
        puts("agfileanteshavia0");printSale(saletemp);
        saletemp.artnr=sale.artnr;
        saletemp.units=sale.units;
        saletemp.price=sale.price;
        puts("agfiledepshavia0");printSale(saletemp);
       }
      }
      else if (sale.units<0){
        puts("agfileantes0");printSale(saletemp);
        saletemp.artnr=sale.artnr;
        saletemp.units=sale.units;
        saletemp.price=sale.price;
        puts("agfiledeps0");printSale(saletemp);
      }
      
      lseek(fdagrtemp,(sale.artnr-1)*12,SEEK_SET);
      puts("Im writing");printSale(saletemp);
      write(fdagrtemp,&saletemp,12);
   }
   int sz=0;
   lseek(fdagrtemp,0,SEEK_SET);
   while(read(fdagrtemp,&saletemp,12)){
    if(saletemp.artnr>0){
     sprintf(writing,"name%d units%d price%.2f\n",saletemp.artnr,saletemp.units,saletemp.price);
     sz=strlen(writing);
     printf("writing=%s, size=%d",writing,sz);
     write(fdagrfinal,writing,sz);
    }
   }
  remove("tempFileOfAgr");
  //remove("")

  return 0;

}