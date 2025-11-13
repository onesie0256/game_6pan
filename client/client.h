#pragma once
#include "../common/common.h"
#include "./../assets/3D_Engine/3DEngine.h"

#define WINDOW_WIDTH 1980
#define WINDOW_HEIGHT 1080

/* game.c */
int gameLoop(void);
int init(void);

/* net.c */
int connectServer(char *serverName);
int sendData(NetworkContainer *container);
int recvData(NetworkContainer *container);

/* title_scene.c */
int titleScene(void);

/* UI.c */
int UI_init(void);
void UI_renderTitleScreen(TitleScene *titleScene);
void UI_renderLoadingScreen(const char *message);
void UI_renderMainScreen(void);
void UI_cleanup(void);



