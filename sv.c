#include "ModuloDados.h"
//3 args, s-sales,r- reposição de stock, i-info, c- price change(inativo);

typedef struct artprc
{
  int artnr;
  float prc;
} ArtPrc;

typedef struct cachePrc
{
  int ind;
  ArtPrc prices[cacheSz];
} Cacheprc;

int openStocks()
{
  return open("stocks", O_RDWR | O_CREAT, 0644);
}

int openSales()
{
  return open("vendas", O_RDWR | O_CREAT, 0644);
}

int openPipeSv(char *pipeNr)
{

  mkfifo(pipeNr, 0600);
  int fdpc = open(pipeNr, O_WRONLY);
  return fdpc;
}

int getStockPointer(int fdstk, int artNr)
{
  int offset = (artNr - 1) * (sizeof(int));
  lseek(fdstk, offset, SEEK_SET);
}

//File infos->ultimo byte agregado
//Abre sale file e copia desde lastBag até EOF para vendasAg
//Abre o ficheiro para output com nome=data
//Stdin=vendasAg Stdout=Fileoutput
//Fork->Exec->Wait
//Remove vendasAg
void runAg()
{

  Venda sale;
  //char*stringsale=malloc(40);
  int sz;
  int lastBag;
  int fds = openSales();
  int fdsa = open("vendasAg.txt", O_RDWR | O_CREAT, 0644);
  int fdinf = open("Infos.txt", O_RDWR | O_CREAT, 0644);
  time_t t;
  time(&t);
  int fdagrfinal = open(ctime(&t), O_WRONLY | O_CREAT, 0644);
  //int fdagrmed=open("intermedAgr",O_RDWR|O_CREAT,0644);
  int rdstt = read(fdinf, &lastBag, 4);
  if (rdstt == 0)
  {
    lastBag = 0;
  }
  sz = lseek(fds, 0, SEEK_END);
  lseek(fds, lastBag, SEEK_SET);
  lseek(fdinf, 0, SEEK_SET);
  write(fdinf, &sz, 4);
  //int agBytes = (sz-lastBag)/12;
  //int agSize = agBytes/nthreads;
  //printf("lastbag->%d, size->%d, agsize->%d\n",lastBag,sz,agSize);

  while (read(fds, &sale, 12) != 0)
  {
    //printSale(sale);
    //puts("li 1");
    write(fdsa, &sale, 12);
  }
  lseek(fdsa, 0, SEEK_SET);

  if (fork() == 0)
  {
    dup2(fdsa, 0);
    dup2(fdagrfinal, 1);
    execlp("./ag", "./ag", (char *)NULL);
  }

  /* Tentativa de agregação concorrente
   char*agsize=malloc(12);
   for(int i=1;i<=4;i++){
    printf("lancei o %d\n",i);
    if(fork()==0){
      lseek(fdagrmed,(i-1)*salesize,SEEK_SET);
      dup2(fdsa,0);
      //dup2(fdagrmed,1);
      if(i==4){agSize=agBytes-3*agSize;}
      sprintf(agsize,"%d",agSize);
      execlp("./ag","./ag",agsize,"b",(char *) NULL);
    }
    if(i!=4){lseek(fdsa,agSize,SEEK_CUR);}
   }
   
   int stt;
   for(int i=1;i<=4;i++){wait(&stt);printf("caçei o %d\n",i);}

   if(fork()==0){
      lseek(fdagrmed,0,SEEK_SET);
      int sze=lseek(fdagrmed,0,SEEK_SET);
      lseek(fdagrmed,0,SEEK_SET);
      dup2(fdagrmed,0);
      dup2(fdagrfinal,1);
      sprintf(agsize,"%d",sze);
      execlp("./ag","./ag",agsize,"t",(char *) NULL);
    }
    wait(&stt);
    printf("caçei o ultimo stt=%d\n",stt);
  */
  int stt;
  wait(&stt);
  close(fdsa);
  remove("vendasAg.txt");
  //remove("intermedAgr");
}

//Procura um preço na cache
float Cached(int artNr, Cacheprc *prodlist)
{
  float r = 0;
  //puts("searching cache");
  for (int i = 0; i < 20; i++)
  {
    if (prodlist->prices[i].artnr == artNr)
    {
      r = prodlist->prices[i].prc;
      //printf("cached r=%f\n",r);
      break;
    }
  }
  return r;
}

void cacheAdd(ArtPrc prd, Cacheprc *prodlist)
{

  prodlist->prices[prodlist->ind] = prd;
  prodlist->ind = (prodlist->ind + 1) % cacheSz;
}

//Procura um preço na cache, se não encontrar procura no ficheiro de artigos
//Se encontrar adiciona o preço á cache, se não devolve -1
float seekPrice(int fpa, int artNr, Cacheprc *prodlist)
{
  float prc;
  float cached = Cached(artNr, prodlist);
  if (cached)
  {
    prc = cached;
  }
  else
  {
    cached = getPrice(fpa, artNr);
    if (cached > 0)
    {
      ArtPrc prd;
      prd.artnr = artNr;
      prd.prc = cached;
      cacheAdd(prd, prodlist);
    }
  }
  return cached;
}

//Altera um preço na cache
void updatePrice(int fpa, Cacheprc *prodlist, SvInfo args)
{
  float prc = getPrice(fpa, args.artNr);
  for (int i = 0; i < cacheSz; i++)
  {
    if (prodlist->prices[i].artnr == args.artNr)
    {
      prodlist->prices[i].prc = prc;
      break;
    }
  }
}

//Escreve um venda no ficheiro de vendasd
void saleWrite(int fda, int fdsales, SvInfo args)
{

  struct sale venda;
  float prc = getPrice(fda, args.artNr);
  if (args.units > 0)
  {
    prc = 0;
  }
  else
  {
    prc = prc * args.units * -1;
  }
  venda.artnr = args.artNr;
  venda.units = args.units;
  venda.price = prc;

  lseek(fdsales, 0, SEEK_END);
  write(fdsales, &venda, sizeof(venda));
}

int runSale(SvInfo args, int fdstk, int fdpc, int fda, int fdsales, Cacheprc *prodlist)
{
  int readStatus;
  SvOut out;
  //puts("running sale");
  float prc = seekPrice(fda, args.artNr, prodlist);
  if (prc <= 0)
  {
    out.runstat = 'e';
    write(fdpc, &out, outSize);
    return 0;
  }

  else if (args.artNr && args.units)
  {

    getStockPointer(fdstk, args.artNr);
    int uniCurr;
    int readStatus = read(fdstk, &uniCurr, StockSize);

    if (readStatus == 0)
    {
      if (args.units < 0)
      {
        out.runstat = 'e';
      }

      else
      {
        out.runstat = 's';
        out.stock = args.units;
        out.price = prc;
        getStockPointer(fdstk, args.artNr);
        write(fdstk, &args.units, 4);
      }
    }

    else
    {
      if ((args.units + uniCurr) < 0)
      {
        out.runstat = 'e';
      }
      else
      {

        saleWrite(fda, fdsales, args);
        out.runstat = 's';
        out.price = prc;

        uniCurr = uniCurr + args.units;
        getStockPointer(fdstk, args.artNr);
        write(fdstk, &uniCurr, 4);
        out.stock = uniCurr;
      }
    }

    write(fdpc, &out, outSize);
    printOut(&out);
  }

  else
  {
    out.runstat = 'e';
    write(fdpc, &out, outSize);
    printOut(&out);
  }
  return 0;
}

//Procura e devolve a informação sobre um artigo
int runInfo(int fpa, int fpstk, int fdpc, SvInfo args, Cacheprc *prodlist)
{
  SvOut out;
  getStockPointer(fpstk, args.artNr);
  int stk;
  float prc;

  int rd = read(fpstk, &stk, StockSize);

  if (rd == 0)
  {
    stk = 0;
  }

  prc = seekPrice(fpa, args.artNr, prodlist);

  if (prc <= 0)
  {
    out.runstat = 'e';
  }

  else
  {
    out.price = prc;
    out.runstat = 'i';
    out.stock = stk;
  }

  int w = write(fdpc, &out, outSize);
  printOut(&out);
  return 0;
}

//Tratamento de sinal
void ShutdownRun()
{
  remove("serverPipe");
  puts("exiting");
  exit(1);
}

int main()
{

  signal(SIGINT, ShutdownRun);
  signal(SIGTERM, ShutdownRun);

  mkfifo("serverPipe", 0600);
  int fdstk = openStocks();
  int fdsales = openSales();
  int fda = openArtigos();

  int fdpp = open("serverPipe", O_RDONLY);
  int sz = sizeof(struct serverinfo);
  SvInfo args;
  int readStatus;
  char *pipeStr;
  int fdpc;
  Cacheprc *prodlist = malloc(sizeof(struct cachePrc));
  prodlist->ind = 0;

  while (1)
  {
    readStatus = read(fdpp, &args, sz);

    if (readStatus == 0)
    {
      write(1, "Server sleeping\n", 17);
      sleep(5);
    }
    else if (args.action == 'p')
    {
      write(1, "Changing price\n", 16);
      updatePrice(fda, prodlist, args);
    }
    else if (args.action == 'a')
    {
      write(1, "Running Ag\n", 12);
      runAg();
    }

    else
    {
      printInfo(&args);

      pipeStr = getPipeID(args.clientID);
      fdpc = openPipeSv(pipeStr);

      if (args.action == 's')
      {
        runSale(args, fdstk, fdpc, fda, fdsales, prodlist);
      }
      else if (args.action == 'i')
      {
        runInfo(fda, fdstk, fdpc, args, prodlist);
      }

      close(fdpc);
    }
  }

  return 0;
}