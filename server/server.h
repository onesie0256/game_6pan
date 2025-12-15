#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>

#include "../common/common.h"
#define DEF_PORT 50100
#define MAX_LEN_ADDR 32
#define BROADCAST -1
#define MAX_NUM_CLIENTS 4

typedef unsigned short u_short;

typedef struct {
  uint8_t id; 
  int sock;
  struct sockaddr_in addr;
  SDL_bool keyNow[KEY_MAX];   //キーの状態
  SDL_bool keyPrev[KEY_MAX];  //キーの状態(1フレーム前)
  #ifdef USE_JOY
  SDL_bool joyBotton[JOY_KEY_MAX];  //ジョイコンのボタンの状態
  SDL_bool joyBottonPrev[JOY_KEY_MAX];
  #endif
} SERVER_CLIENT;

void setup_server(int, u_short);
int control_requests();
void terminate_server();