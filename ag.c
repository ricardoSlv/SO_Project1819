#include "ModuloDados.h"

//Parte de concorrência
//argv[1]=nº de vendas a agregar
//argv[2]=mode b || t (binary,text)

int main(int argc, char *argv[])
{

  int fdagrtemp = open("tempFileOfAgr", O_RDWR | O_CREAT, 0644);

  Venda sale;
  Venda saletemp;
  char *writing = malloc(50);

  while ((read(0, &sale, 12) > 0))
  {
    lseek(fdagrtemp, (sale.artnr - 1) * 12, SEEK_SET);

    if (sale.units < 0)
    {
      if (read(fdagrtemp, &saletemp, 12) > 0)
      {

        if (saletemp.artnr > 0)
        {
          saletemp.units = saletemp.units + sale.units;
          saletemp.price = saletemp.price + sale.price;
        }
        else
        {
          saletemp.artnr = sale.artnr;
          saletemp.units = sale.units;
          saletemp.price = sale.price;
        }
      }
      else
      {
        saletemp.artnr = sale.artnr;
        saletemp.units = sale.units;
        saletemp.price = sale.price;
      }

      lseek(fdagrtemp, (sale.artnr - 1) * 12, SEEK_SET);
      write(fdagrtemp, &saletemp, 12);
    }
  }
  int sz = 0;
  lseek(fdagrtemp, 0, SEEK_SET);
  while (read(fdagrtemp, &saletemp, 12))
  {
    if (saletemp.artnr > 0)
    {
      sprintf(writing, "%d %d %.2f\n", saletemp.artnr, saletemp.units, saletemp.price);
      sz = strlen(writing);
      write(1, writing, sz);
    }
  }

  remove("tempFileOfAgr");
  return 0;
}