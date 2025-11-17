#pragma once
#include "../common/common.h"
#include "./../assets/3D_Engine/3DEngine.h"


#define WINDOW_WIDTH 1920
#define WINDOW_HIGHT 1080
#define JOYCON_BUTTON_EVENT SDL_USEREVENT + 1 // Joy-Conボタンイベントの定義
#define KEY_BUTTON_EVENT SDL_USEREVENT + 2

struct camera_t;
typedef struct camera_t Camera;

/**
 * @brief タイトルシーン
 */
typedef struct{
    List *polygonList;  //立体図形のリスト
}TitleScene;

/**
 * @brief メインゲームのシーン
 */
typedef struct {
    List *polygonList;  //立体図形のリスト
    List *cars;         //車のリスト
    List *UIList;       //UIのリスト
    Camera *camera;     //カメラ
}MainScene;

/**
 * @brief キーの状態
 */
typedef enum {
    K_RIGHT, //→
    K_LEFT,  //←
    K_UP,    //↑
    K_DOWN,  //↓
    K_SPACE, //スペースキー
    K_SHIFT, //シフトキー
    K_ENTER, //エンターキー
    K_ESCAPE,//escキー
    K_MAX    //使用するキーの数
}KeyNum;

//ジョイコン使用
//#ifdef USE_JOY

#include <joyconlib.h>

/**
 * @brief ジョイコンのボタン
 */
typedef enum {

    JOY_Home,
    JOY_Plus, //+ボタン
    JOY_Minus, //-ボタン
    JOY_ZR,   //ZRボタン
    JOY_ZL,   //ZRボタン
    JOY_A,
    JOY_B,
    JOY_X,
    JOY_Y,

    JOY_Max   //ボタンの数

}JoyConInputNum;

//#endif

/* game.c */
int gameLoop(void);
int mainLoopDelay(void *arg);

/* net.c */
int connectServer(char *serverName);
int sendData(NetworkContainer *container);
int recvData(NetworkContainer *container);

/* title_scene.c */
int titleScene(void);

/* main_scene.c */
int mainScene(void);

/* thread.c */
void createThread(void);
int joy_func(void *args);
int key_func(void *args);
typedef enum{
    UIG_Title = 1,
    UIG_Playing = 2,
    UIG_BallMoving = 4,
    UIG_BallStop = 8,
    UIG_CourseNum = 16,
    UIG_PlayerNum = 32,
    UIG_Score = 64
}UIGroup;

typedef struct {
    SDL_Surface *surface;
    
    SDL_Rect dst;

    SDL_bool isDisplay;
    SceneKinds group;

}UI;

