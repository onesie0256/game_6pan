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

/**
 * @brief 銃の番号
 */
typedef enum{
    Pistl,      //ピストル
    Shotgun,    //ショットガン
    Sniper,     //スナイパー
}GunKinds;

/**
 * @brief 武器の表示情報を格納する構造体
 */
typedef struct {
    GunKinds kind;
    const char* name;
    const char* image_path;
} WeaponInfo;


// 武器情報配列を宣言
extern const WeaponInfo weapon_info[WEAPON_TYPE_MAX];


/**
 * @brief メインゲームのシーン
 */
typedef struct {
    List *polygonList;  //立体図形のリスト
    List *cars;         //車のリスト
    List *UIList;       //UIのリスト
    Camera *camera;     //カメラ
    Car *myCar;         //自分の操作する車
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
#ifdef USE_JOY

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

#endif


/**
 * @brief 車の情報
 */
typedef struct car_t
{
    uint8_t id;                 //操作するプレイヤーのid
    Polygon *collisionBox;      //当たり判定の直方体
    Obj *model;                 //3Dモデル
    Vec3f center;               //中心座標
    Vec3f velocity;             //速度
    Vec3f direction;            //方向ベクトル
    Vec3f preCoordOfVertexs[8]; //1フレーム前の各頂点の座標
}Car;

/**
 * @brief 銃の構造体
 */
typedef struct
{
    //to do
}Gun;

/* game.c */
int gameLoop(void);
int mainLoopDelay(void *arg);

/* net.c */
int connectServer(char *serverName);
int sendData(NetworkContainer *container);
int recvData(NetworkContainer *container);

/* title_scene.c */
int titleScene(void);

/* wait_scene.c */
int waitScene(void);

/* main_scene.c */
int mainScene(void);


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

/* car.c */
Car *createCar(List *list , uint8_t id , Vec3f coord);
void displayCars(List *list);
void moveCar(List *carList , List *PolygonList);
//void destroyCar(Car *car);