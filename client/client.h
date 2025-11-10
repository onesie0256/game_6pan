#pragma once
#include "../common/common.h"
#include "./../assets/3D_Engine/3DEngine.h"

/* game.c */
int gameLoop(void);
int init(void);

/* net.c */
int connectServer(char *serverName);
int sendData(NetworkContainer *container);
int recvData(NetworkContainer *container);

/* title_scene.c */
int titleScene(void);



