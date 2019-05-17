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

typedef struct Palavra{
  char palavra[32];
  struct Palavra *prox;
} plv;

plv *ini = NULL;

void mostrar_forca(short erros, int jogador, char *str);

int main(int argc, char const *argv[])
{
  plv *aux;
  char str[32], tentativa[32], resposta[32];
  short erros = 0, i;
  struct sockaddr_in server;
  int s0, length, jogador = 0, n = 0;

  // Revisa erro de sintaxe
  if (argc < 2) {
    printf("Sintaxe: c.exe IP-DO-SERVIDOR\n");
    return 1;
  }
  // Configura socket
  s0 = socket(AF_INET, SOCK_STREAM, 0);
  if (s0 < 0) {
    fprintf(stderr, "Erro no socket\n");
    return 1;
  }
  bzero(&server, sizeof(server));
  server.sin_family = AF_INET ;
  server.sin_port = htons (PORT);
  server.sin_addr.s_addr = inet_addr(argv[1]);
  if (connect(s0, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
	   fprintf(stderr, "Erro no connect\n");
     return 1;;
	}

  recv(s0, &jogador, sizeof(jogador), 0);
  recv(s0, str, sizeof(str), 0);
  
  //Inicia jogo
  mostrar_forca(0, jogador, str);

  while(erros < 7) {
    recv(s0, &n, sizeof(int), 0); // recebe numero do jogador
    if (n == jogador) {
      printf(">> ");
      scanf("%s", tentativa);
      send(s0, tentativa, sizeof(tentativa), 0);
    } else {
      printf("...(Jogador-%d)...\n", n);
    }
    recv(s0, str, sizeof(str), 0);
    recv(s0, &erros, sizeof(erros), 0);
    if (erros == -1) {
      printf("Parabens, voce ganhou! A palavra era %s\n", str);
      break;
    }else if (erros == -2) {
      recv(s0, &n, sizeof(int), 0);
      printf("Voce Perdeu! Jogador %d venceu\n", n);
      break;
    }
    mostrar_forca(erros, jogador, str);
  }
  if (erros == 7) {
    printf("Todos perderam!\n");
  }
  sleep(3);
  close(s0);
  return 0;
}

void mostrar_forca(short erros, int jogador, char *str)
{
  system("clear");
  printf("* JOGADOR %2d          \n", jogador);
  printf("    _______            \n");
  printf("   |       |           \n");
  printf("   |     %s            \n", (erros > 0) ? "(0.0)":"     ");
  printf("   |      %c%c%c       \n", (erros > 2) ? '/':' ', (erros > 1) ? '|':' ', (erros > 3) ? '\\':' ');
  printf("   |     %c %c %c      \n", (erros > 2) ? '/':' ', (erros > 1) ? '|':' ', (erros > 3) ? '\\':' ');
  printf("   |      %c %c        \n", (erros > 4) ? '/':' ', (erros > 5) ? '\\':' ');
  printf("   |     %c   %c       \n", (erros > 4) ? '/':' ', (erros > 5) ? '\\':' ');
  printf(" __|__                 \n");
  printf("                     %s\n", str);
  //sleep(1);
  return;
}
