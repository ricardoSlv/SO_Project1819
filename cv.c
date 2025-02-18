#include "ModuloDados.h"

char *PipeID;

//to do
//Testar se abriu
// da break com 1 input para s
int openPipeCv(char *pipeNr)
{

   mkfifo(pipeNr, 0600);

   int fdpc = open(pipeNr, O_RDONLY);
   return fdpc;
}

//Preenche uma struct de info com o input
int fillInfo(struct serverinfo *svinfo, char *input)
{
   //printf("input->%s<-",input);
   int sz = strlen(input);
   int r = 0;
   char *token = strtok(input, " \n");
   int artnr = atoi(token);
   //printf("token= %s, sz= %d\n",token,sz);
   if (strlen(token) == sz - 1)
   {
      if (artnr != 0)
      {
         svinfo->artNr = artnr;
         svinfo->action = 'i';
         r = 1;
      }
   }
   else
   {
      token = strtok(NULL, "\0");
      int uni = atoi(token);
      if (uni != 0 && artnr != 0)
      {
         svinfo->artNr = artnr;
         svinfo->action = 's';
         svinfo->units = uni;
         svinfo->artNr = artnr;
         r = 1;
      }
   }
   //printf("filinfo returning %d \n",r);
   return r;
}
void formatOutput(char *strS, char *strP, char *output)
{
   output[0] = 'i';
   strcpy(&output[1], strS);
   strcpy(&output[1 + StockSize - 1], strP);
}

//Tratamento de SIGPIPE, o servidor desliga durante um contacto com um cliente
void pipeBrokenRun()
{
   write(1, "Pipe Broken, server may have exited, shutting down\n", 52);
   remove(PipeID);
   exit(1);
}

//Saída limpa
void shutDownRun()
{
   write(1, "Shutting down\n", 52);
   remove(PipeID);
   exit(1);
}

int main()
{

   signal(SIGPIPE, pipeBrokenRun);
   signal(SIGINT, shutDownRun);
   signal(SIGTERM, shutDownRun);

   int fdw = -1;

   while (fdw == -1)
   {
      fdw = open("serverPipe", O_WRONLY);
      if (fdw == -1)
      {
         write(1, "Couldn't open server pipe, server may be down\n", 46);
         sleep(5);
      }
   }

   int tme = time(NULL);
   tme = tme - (tme / 10000000) * 10000000;

   PipeID = getPipeID(tme);
   puts(PipeID);
   int pipefd;
   struct serverinfo *svinf = malloc(infoSize);
   struct serveroutput *svout = malloc(outSize);
   svinf->clientID = tme;

   char *input = malloc(buffsize);
   int readnr;
   int n;

   write(0, "Insira o input (->s artigos difStock ou  ->i artigo)\n", 54);

   while (input[0] != 'q')
   {
      n = 1;
      readnr = readline(0, input);

      if (input[0] == 'q')
      {
         write(0, "Quit input\n", 12);
      }

      else if (fillInfo(svinf, input) == 0)
      {
         write(0, "Input inválido\n", 17);
      }

      else
      {

         printInfo(svinf);
         write(fdw, svinf, infoSize);

         pipefd = openPipeCv(PipeID);
         while (n > 0)
         {
            n = read(pipefd, svout, outSize);
            //printf("cvread=%d\n",n);
         }
         close(pipefd);
         printOut(svout);
         //puts(output);
      }
   }

   free(input);
   close(fdw);
   close(pipefd);
   remove(PipeID);

   return 0;
}