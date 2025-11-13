#pragma once
#include "../common/common.h"
#include "./../assets/3D_Engine/3DEngine.h"


#define WINDOW_WIDTH 1920
#define WINDOW_HIGHT 1080

struct camera_t;
typedef struct camera_t Camera;

/**
 * @brief タイトルシーン
 */
typedef struct{
    List *polygonList;  //立体図形のリスト
}TitleScene3D;

/**
 * @brief タイトルシーンの状態管理 (UIアニメーション用)
 */
typedef struct {
	int animationTimer; // アニメーション進行タイマー
	float beatScale;    // タイトルテキストの拡大・縮小スケール
	float carX;         // 車のアニメーション用X座標
	int carAnimState;   // 車のアニメーション状態
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

/* game.c */
int gameLoop(void);

/* net.c */
int connectServer(char *serverName);
int sendData(NetworkContainer *container);
int recvData(NetworkContainer *container);

/* title_scene.c */
int titleScene(void);

/* main_scene.c */
int mainScene(void);

/* UI.c */
int UI_init(void);
void UI_renderTitleScreen(TitleScene *titleScene);
void UI_cleanup(void);

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
