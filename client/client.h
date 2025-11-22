#pragma once
#include "../common/common.h"
#include "./../assets/3D_Engine/3DEngine.h"


#define WINDOW_WIDTH 1920
#define WINDOW_HIGHT 1080

struct camera_t;
typedef struct camera_t Camera;
struct vec3f_t;
typedef struct vec3f_t Vec3f;
struct polygon_t;
typedef struct polygon_t Polygon;
struct obj_t;
typedef struct obj_t Obj;


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
 * @brief 銃の番号
 */
typedef enum{
    Pistl,      //ピストル
    Shotgun,    //ショットガン
    Sniper,     //スナイパー
}GunKinds;

/**
 * @brief 車の情報
 */
typedef struct
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

/* net.c */
int connectServer(char *serverName);
int sendData(NetworkContainer *container);
int recvData(NetworkContainer *container);

/* title_scene.c */
int titleScene(void);

/* main_scene.c */
int mainScene(void);

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


/* car.c */
Car *createCar(List *list , uint8_t id , Vec3f coord);
void displayCars(List *list);
void moveCar(List *carList , List *PolygonList);
//void destroyCar(Car *car);