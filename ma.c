#include "ModuloDados.h"

int openStrings()
{
  return open("strings", O_RDWR | O_CREAT, 0644);
}

int openStrings2()
{
  return open("strings2.txt", O_RDWR | O_CREAT, 0644);
}

//Produto existe nos artigos?
int checkValidProd(int fda, int ind)
{
  lseek(fda, 0, SEEK_SET);
  int size = lseek(fda, 0, SEEK_END);

  return ((ind - 1) * artSize) < size ? 1 : 0;
}

//Adiciona uma string ao strings, devolte o byte onde começou a escrita
static int addString(int fp, char *str)
{

  long tamanho = lseek(fp, 0, SEEK_END);
  write(fp, str, strlen(str));
  write(fp, "\n", 1);

  return tamanho;
}

//Adiciona um artigo
int addArtigo(int fpa, int fps, char *strinput)
{
  char *token;
  long tamanho = lseek(fpa, 0, SEEK_END);
  int numArts = (tamanho / artSize) + 1;
  struct produto prd;

  token = strtok(strinput, " ");
  int nr = addString(fps, token);

  token = strtok(NULL, "\n");
  float preco = atof(token);

  if ((numArts && preco && nr >= 0) != 0)
  {
    prd.codigo = numArts;
    prd.price = preco;
    prd.namezone = nr;

    int sz = sizeof(struct produto);
    write(fpa, &prd, sz);
    lseek(fpa, -sz, SEEK_CUR);
    read(fpa, &prd, sz);
    printf("%d,%f,%d\n", prd.codigo, prd.price, prd.namezone);
    write(0, "Sucess\n", 8);
    return 1;
  }

  else
  {
    write(0, "Input inválido\n", 17);
    return 0;
  }
}

//Despoletado pela função de mudança de nome
void compactador(int fda, int fdstrings)
{
  write(1, "Compacting\n", 12);
  struct produto prod;
  char *name = malloc(80);
  int pointer = 0;
  int readnr = 0;
  int fdstrings2 = openStrings2();
  lseek(fdstrings, 0, SEEK_SET);
  lseek(fda, 0, SEEK_SET);

  while (read(fda, &prod, 12) != 0)
  {
    lseek(fdstrings, prod.namezone, SEEK_SET);
    readnr = readline(fdstrings, name);
    //printf("read %d, prodnamezone %d,name %s",readnr,prod.namezone,name);
    write(fdstrings2, name, readnr);
    prod.namezone = pointer;
    lseek(fda, -12, SEEK_CUR);
    write(fda, &prod, 12);
    pointer = pointer + readnr;
  }
  lseek(fdstrings, 0, SEEK_SET);
  truncate("strings", 1);
  lseek(fdstrings2, 0, SEEK_SET);
  int rdnr = read(fdstrings2, name, 80);
  while (rdnr)
  {
    write(fdstrings, name, rdnr);
    rdnr = read(fdstrings2, name, 80);
  }
  remove("strings2.txt");
}

//Mudança de nome, atualiza o nº de bytes inuteis no file de infos
//Pode despoletar o compactador
void changeName(int fpa, int fps, char *input)
{
  char *token;

  token = strtok(input, " ");
  int ind = atoi(token);
  //Não passar do fim
  if (checkValidProd(fpa, ind))
  {
    lseek(fpa, (ind - 1) * (artSize), SEEK_SET);

    struct produto prd;
    read(fpa, &prd, artSize);
    printf("Antes:%d,%f,%d\n", prd.codigo, prd.price, prd.namezone);

    char *temp = malloc(80);
    lseek(fps, prd.namezone, SEEK_SET);
    int newUseless = readline(fps, temp);
    token = strtok(NULL, "\n");
    int nr = addString(fps, token);
    lseek(fpa, -artSize, SEEK_CUR);
    prd.namezone = nr;

    write(fpa, &prd, artSize);
    lseek(fpa, -artSize, SEEK_CUR);
    read(fpa, &prd, artSize);
    printf("Depois:%d,%f,%d\n", prd.codigo, prd.price, prd.namezone);

    int fdinf = open("Infos.txt", O_RDWR | O_CREAT, 0644);
    int uselessBy;
    lseek(fdinf, 4, SEEK_SET);
    int rdstt = read(fdinf, &uselessBy, 4);
    if (rdstt == 0)
    {
      uselessBy = 0;
    }
    uselessBy = uselessBy + newUseless;
    lseek(fps, 0, SEEK_SET);
    int sz = lseek(fps, 0, SEEK_END);
    lseek(fdinf, 4, SEEK_SET);

    if ((sz / uselessBy) < 5)
    {
      uselessBy = 0;
      write(1, "20 percent wasted space reached, \n", 35);
      write(fdinf, &uselessBy, 4);
      compactador(fpa, fps);
    }
    else
    {
      write(fdinf, &uselessBy, 4);
    }
  }
  else
  {
    write(0, "Produto não válido\n", 21);
  }
}

//Mudança de preço nos file artigos
void changePrice(int fpa, char *input)
{
  char *token;

  token = strtok(input, " ");
  int ind = atoi(token);

  if (checkValidProd(fpa, ind))
  {
    lseek(fpa, (ind - 1) * (artSize), SEEK_SET);

    struct produto prd;
    read(fpa, &prd, artSize);
    printf("Antes:%d,%f,%d\n", prd.codigo, prd.price, prd.namezone);

    token = strtok(NULL, "\n");
    float prc = atof(token);

    if (prc > 0)
    {
      //printf("preco=%s %f\n",token,prc);
      lseek(fpa, -artSize, SEEK_CUR);
      prd.price = prc;

      write(fpa, &prd, artSize);
      lseek(fpa, -artSize, SEEK_CUR);
      read(fpa, &prd, artSize);
      printf("Depois:%d,%f,%d\n", prd.codigo, prd.price, prd.namezone);
      int fdpp = open("serverPipe", O_WRONLY);
      if (fdpp > 0)
      {
        struct serverinfo args;
        args.action = 'p';
        args.artNr = prd.codigo;
        write(fdpp, &args, 16);
        close(fdpp);
      }
    }
    else
    {
      write(0, "Preço não válido\n", 21);
    }
  }
  else
  {
    write(0, "Produto não válido\n", 21);
  }
}

//Tenta abrir o sever pipe, não corre o risco de bloqueio se o servidor executar
//correctamente, isto é, remover o serverpipe á saida
void sendAg()
{
  int fdpp = open("serverPipe", O_WRONLY);
  if (fdpp > 0)
  {
    struct serverinfo args;
    args.action = 'a';
    write(fdpp, &args, 16);
    close(fdpp);
    write(1, "Signal Sent\n", 13);
  }
  else
  {
    write(1, "Couldn't send signal, server may be down\n", 42);
  }
}

int main()
{

  int fpArts = openArtigos();
  int fpStrings = openStrings();
  char *input = malloc(sizeof(char) * 100);
  int readnr;
  write(0, "Comando: i+nome+preço, n+art+newname, p+art+newprice, a(agregador)\n", 69);

  while (input[0] != 'q')
  {

    readnr = readline(0, input);
    if (input[0] == 'i')
    {
      addArtigo(fpArts, fpStrings, &input[2]);
    }
    else if (input[0] == 'n')
    {
      changeName(fpArts, fpStrings, &input[2]);
    }
    else if (input[0] == 'p')
    {
      changePrice(fpArts, &input[2]);
    }
    else if (input[0] == 'q')
    {
      write(0, "Quit input\n", 12);
    }
    else if (input[0] == 'a')
    {
      sendAg();
    }
    else
    {
      write(0, "Input inválido\n", 17);
    }
  }

  free(input);
  return 0;
}
