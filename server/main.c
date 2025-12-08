#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "server.h"

#define SERVER_PORT 50000

GameManager myGameManager;

extern int connectClient(void);
extern void CloseClient(int i);

int main(int argc, char *argv[]) {
  int num_cl = 1;
  u_short port = DEF_PORT;

  switch (argc) {
  case 1:
    break;
  case 2:
    num_cl = atoi(argv[1]);
    break;
  case 3:
    num_cl = atoi(argv[1]);
    port = atoi(argv[2]);
    break;
  default:
    fprintf(stderr, "Usage: %s [number of clients] [port number]\n", argv[0]);
    return 1;
  }

  if (num_cl < 0 || num_cl > MAX_NUM_CLIENTS) {
    fprintf(stderr, "Max number of clients is %d\n", MAX_NUM_CLIENTS);
    return 1;
  }

  fprintf(stderr, "Number of clients = %d\n", num_cl);
  fprintf(stderr, "Port number = %d\n", port);

  setup_server(num_cl, port);

  int cond = 1;
  while (cond) {
      cond = control_requests();
  }
  SDL_Delay(3000);
  terminate_server();

  return 0;
}