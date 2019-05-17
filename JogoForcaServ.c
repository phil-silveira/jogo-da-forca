// Bibliotecas padroes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
// Bibliotecas socket
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>

#define PORT 6660
#define MAX_JOGADORES 6

typedef struct Palavra{
  char palavra[32];
  struct Palavra *prox;
} plv;
plv *ini = NULL;

int carregar_palavras();
int escolher_palavra(char *str, int n);

int main(int argc, char const *argv[])
{
  plv *aux;
  char str[32], tentativa[32], resposta[32];
  short erros = 0, n, x, i;
  int s[MAX_JOGADORES], s0, length, j, numj = 0;
  struct sockaddr_in server;
	length = sizeof(server);

  // Escolhe numero de jogadores
  while(numj < 1 || numj > MAX_JOGADORES) {
    printf("Digite o numero de jogadores(1-%d): ",MAX_JOGADORES);
    scanf("%d", &numj);
  }

  // Configurando socket
  s0 = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons (PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  if(bind(s0, (struct sockaddr *)&server, length) == -1) {
	   fprintf(stderr, "Erro no bind\n");
     return 0;
	}
    if (listen(s0, numj) == -1) {
  		fprintf(stderr, "Erro no listen\n");
  		return 0;
  	}
    for(i = 0; i < numj; i++) {
      s[i] = accept(s0, (struct sockaddr *)&server, &length);
      printf("conexao %d estabelecida\n", i+1);//debug
    }

  //  Inicializa lista de palavras
  n = carregar_palavras();
  aux = (plv *)escolher_palavra(str, n);

  for(j = 1; j <= numj; j++) {
    send(s[j-1], &j, sizeof(int), 0);// Envia numero do jogador
    send(s[j-1], str, sizeof(str), 0);// Envia numero de letras
  }

  //Inicia jogo
  printf("A palava sera -> %s\n", aux->palavra);//debug

  for(j = 1; erros < 7; 1) {
    printf("(J-%d) ", j);//debug
    for(i = 0; i < numj; i++) {
      send(s[i], &j, sizeof(int), 0);// Envia numero do jogador
      sleep(1);
    }
    recv(s[j-1], tentativa, sizeof(tentativa), 0);
    printf("Tentativa = %s\n", tentativa);//debug
    if (strlen(tentativa) > 1) { // Se for uma palavra
      if (!strcmp(tentativa, aux->palavra)) {
        printf("Jogador %d acertou a palavra\n", j);//debug
        erros = -1;
        send(s[j-1], aux->palavra, sizeof(aux->palavra), 0);// Envia numero de letras
        send(s[j-1], &erros, sizeof(erros), 0);
        erros = -2;
        for(i = 0; i < numj; i++) {
          send(s[i], aux->palavra, sizeof(aux->palavra), 0);// Envia numero de letras
          send(s[i], &erros, sizeof(erros), 0);
        }
        return 0;
      } else {
        erros++;
      }
    }
    else { // Se for uma letra
      x = 1;
      for(i = 0; i < strlen(aux->palavra); i++) {
        if (tentativa[0] == aux->palavra[i]) {
          str[i] = tentativa[0];
          x = 0;
        }
      }
      for(i = 0; i < strlen(aux->palavra); i++) {
        if (str[i] == '-') {
          break;
        }
      }
      if (i == strlen(aux->palavra) && str[strlen(aux->palavra) - 1] != '-') {
        printf("Jogador %d acertou a palavra\n", j);//debug
        erros = -1;
        send(s[j-1], aux->palavra, sizeof(aux->palavra), 0);// Envia numero de letras
        send(s[j-1], &erros, sizeof(erros), 0);
        erros = -2;
        for(i = 0; i < numj; i++) {
          send(s[i], aux->palavra, sizeof(aux->palavra), 0);// Envia numero de letras
          send(s[i], &erros, sizeof(erros), 0);
          send(s[i], &j, sizeof(j), 0);
        }
        break;
      }
      erros += x;
    }
    for(i = 0; i < numj; i++) {
      send(s[i], str, sizeof(str), 0);// Envia numero de letras
  	  send(s[i], &erros, sizeof(erros), 0);
    }
    printf("\t\t%s\n", str);
    j = (j < numj)? ++j : 1;
    printf("j = %d\n", j);
  }
  for(i = 0; i < numj; i++) {
    close(s[i]);
  }
  return 0;
}

int escolher_palavra(char *str, int n)
{
  int i, x;
  plv *aux = ini;

  //  Escolhe a palavra
  srand((unsigned)time(NULL));
  x = (rand()% n);
  for(i = 0; i < x; i++) {
    aux = aux->prox;
  }
  for(i = 0; i < strlen(aux->palavra); i++) {
    str[i] = '-';
  }
  str[i] = '\0';
  return (int)aux;
}

int carregar_palavras()
{
  FILE *fd;
  plv *aux = NULL,*novo = NULL;
  char str[32];
  short nump = 0;

  fd = fopen("Palavras.txt","r");
  if (fd == NULL) {
    printf("Arquivo \"Palavras.txt\" nao foi encontrado\n");
    return -1;
  }
   fscanf(fd, "%s", str);
  while(!feof(fd)) {
    novo = (plv *)malloc(sizeof(plv));
    strcpy(novo->palavra, str);
    novo->prox = NULL;
    if (ini == NULL) {
      ini = novo;
    } else {
      aux->prox = novo;
    }
    aux = novo;
    fscanf(fd, "%s", str);
    nump++;
  }
  fclose(fd);
  return nump;
}
