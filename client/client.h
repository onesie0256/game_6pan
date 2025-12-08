#pragma once
#include "../common/common.h"
#include "./../assets/3D_Engine/3DEngine.h"


#define WINDOW_WIDTH 1920
#define WINDOW_HIGHT 1080
#define JOYCON_BUTTON_EVENT SDL_USEREVENT + 1 // Joy-Conボタンイベントの定義
#define KEY_BUTTON_EVENT SDL_USEREVENT + 2

#define WEAPON_TYPE_MAX 3

struct camera_t;
typedef struct camera_t Camera;
struct vec3f_t;
typedef struct vec3f_t Vec3f;
struct polygon_t;
typedef struct polygon_t Polygon;
struct obj_t;
typedef struct obj_t Obj;
struct car_t;
typedef struct car_t Car;
struct course_t;
typedef struct course_t Course;
struct checkPoint_t;
typedef struct checkPoint_t CheckPoint;






/**
 * @brief タイトルシーン
 */
typedef struct{
    List *polygonList;  //立体図形のリスト
}TitleScene3D;

/**
 * @brief タイトルシーンの状態管理 
 */
typedef struct {
	int animationTimer; // アニメーション進行タイマー
	float beatScale;    // タイトルテキストの拡大・縮小スケール
    float carX;
    int carAnimState;
}TitleScene;

/**
 * @brief 待機シーンの状態管理
 */
typedef struct {
    int selectedWeaponIndex; // 選択中の武器のインデックス
} WaitScene;










/* game.c */
int gameLoop(void);
int mainLoopDelay(void *arg);

/* client_net.c */
void setup_client(char *serverName, uint16_t port); 
int control_requests();
void terminate_client();
void send_input_data(void);
void receive_input_data(void);
void send_Quit(void);

/* title_scene.c */
int titleScene(void);

/* wait_scene.c */
int waitScene(void);


/* UI.c */
int UI_init(void);
void UI_renderTitleScreen(TitleScene *titleScene);
void UI_renderWaitScreen(WaitScene *waitScene);
void UI_cleanup(void);

/* thread.c */
void createThread(void);
int joy_func(void *args);
int key_func(void *args);
SDL_bool isKeyDowned(int KEY_NAME);

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

/* 3DEutil.c */
int initWindow(char *title);
void closeWindow(void);
int draw(Camera *camera);
void updateCamera(Car *car , Camera *camera);

