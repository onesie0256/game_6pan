#pragma once
#include "../common/common.h"
#include "./../assets/3D_Engine/3DEngine.h"

/**
 * @brief タイトルシーン
 */
typedef struct{
    List *polygonList;  //立体図形のリスト
}titleScene;

/**
 * @brief メインゲームのシーン
 */
typedef struct {
    List *polygonList;  //立体図形のリスト
    List *cars;         //車のリスト
    List *UIList;       //UIのリスト
    Camera *camera;     //カメラ
}mainScene;


/* game.c */
int gameLoop(void);
int init(void);

/* net.c */
int connectServer(char *serverName);
int sendData(NetworkContainer *container);
int recvData(NetworkContainer *container);

/* title_scene.c */
int titleScene(void);



