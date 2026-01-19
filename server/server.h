#pragma once
#include "../common/common.h"
#include <netinet/in.h>
#define DEF_PORT 50100
#define MAX_LEN_ADDR 32
#define BROADCAST -1
#define MAX_NUM_CLIENTS 10

typedef unsigned short u_short;

typedef struct {
  uint8_t id; 
  int sock;
  struct sockaddr_in addr;
  socklen_t addr_len;
  SDL_bool keyNow[KEY_MAX];   //キーの状態
  SDL_bool keyPrev[KEY_MAX];  //キーの状態(1フレーム前)
  #ifdef USE_JOY
  SDL_bool joyBotton[JOY_KEY_MAX];  //ジョイコンのボタンの状態
  SDL_bool joyBottonPrev[JOY_KEY_MAX];
  #endif
} SERVER_CLIENT;

/* server_net.c */
void setup_server(int, u_short);
int control_requests();
void terminate_server();
void sendCarInfo(void);
void sendACK(void);
void sendAllClientData(void);
void sendCount(int count);
void sendQuit(void);

/* server_physics.c */
void setupPhysics(void);
Uint32 update_physics(Uint32 interval , void *name);
