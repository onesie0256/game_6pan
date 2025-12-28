/**
 * @file hoge
 */
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#ifdef USE_JOY
#include <joyconlib.h>
#endif
#include "./../assets/3D_Engine/3DEngine.h"

#define KEY_MAX 10
#define JOY_KEY_MAX 10
#define FONT_MAX 5
#define FPS_f 90.0f
#define FPS_inv 1.0f/FPS_f

#define MAX_LEN_NAME 100
#define BACKLOG 10      // 同時接続待ち上限
#define MAX_Clients 10

#define MYNAME_MAX 30

#define WEAPON_TYPE_MAX 3

/* network orders */
#define COMMAND_QUIT        'Q' //終了要求コマンド
#define COMMAND_SYN         'S' //接続要求コマンド(クライアント->サーバー)
#define COMMAND_INPUT_DATA  'I' //入力情報コマンド(クライアント->サーバー)
#define COMMAND_CARINFO     'C' //車情報コマンド(サーバー->クライアント)
#define COMMAND_GUN         'G' //銃の種類の送信コマンド(クライアント->サーバー)
#define COMMAND_CLIENT_DATA 'c' //クライアントの情報のデータ
#define COMMAND_ACK         'A' //ACKコマンド
#define COMMAND_COUNT       'D' //スタート時のカウントダウン



struct list_t;
typedef struct list_t List;
struct checkPoint_t;
typedef struct checkPoint_t CheckPoint;
struct gun_t;
typedef struct gun_t Gun;






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

typedef struct client_t{
    uint8_t id;                 //クライアントのid
    uint8_t gunId;              //銃の種類
    SDL_bool keyNow[KEY_MAX];   //キーの状態
    SDL_bool keyPrev[KEY_MAX];  //キーの状態(1フレーム前)
    SDL_bool joyBotton[JOY_KEY_MAX];  //ジョイコンのボタンの状態
    SDL_bool joyBottonPrev[JOY_KEY_MAX];
}Client;

/**
 * @name GameManager
 * @brief ゲーム全体の情報を格納する
 */
typedef struct
{
    SDL_bool quitRequest;           //SDL_QUITが通知されたらTRUE
    int ackRequest;            //ACKが通知されたらTRUE
    uint8_t myID;                   //ID
    uint8_t playerNum;              //プレイヤー人数
    uint8_t gunId;                  //自分の銃の種類
    char myName[MYNAME_MAX];        //自分の名前
    char serverName[MAX_LEN_NAME];               //サーバーの名前
    uint16_t serverPort;             //サーバーのポート番号
    SDL_Window *window;
    SDL_Renderer *renderer;         //レンダラ
    SDL_GLContext context;          //コンテキスト
    Uint16 windowW;                 //ウィンドウの横幅
    Uint16 windowH;                 //ウィンドウの縦幅
    TTF_Font *fonts[FONT_MAX];      //フォント
    void *scene;                    //シーン
    uint8_t sceneID;                //シーン番号
    SDL_Surface *UI;                //UI用のサーフェイス
    List *UIList;
    SDL_Event event; // SDLイベント構造体    
    SDL_Thread * joy_thread;
    #ifdef USE_JOY
    joyconlib_t jc;
    joycon_btn prev_btn; // 前回のボタン状態
    #endif 
    SDL_Thread * key_thread;
    SDL_Thread * net_thread;
    SDL_TimerID timer;
    SDL_bool keyNow[KEY_MAX]; //キーの状態
    SDL_bool keyPrev[KEY_MAX];//キーの状態(1フレーム前)
    #ifdef USE_JOY
    SDL_bool joyBotton[JOY_KEY_MAX];  //ジョイコンのボタンの状態
    SDL_bool joyBottonPrev[JOY_KEY_MAX];
    float StickX;            //ジョイスティックのx方向の傾き
    float StickY;            //ジョイスティックのy方向の傾き
    #endif

    Client clients[MAX_Clients];    //クライアントのリスト

    ObjEX models[WEAPON_TYPE_MAX+MAX_Clients];  //銃と車のモデルを格納
}GameManager;

/**
 * @brief シーンの番号
 */
typedef enum{
    Scene_Title,      //タイトル画面
    Scene_Wait,       //待機画面
    Scene_Main,       //ゲーム画面
    Scene_Max
}SceneKinds;

typedef struct {
    uint16_t keyInputs;         //キーの情報
    uint16_t joyKeyInputs;      //ジョイスティックボタン
    float stickX;               //スティック傾きX
    float stickY;               //スティック傾きY
}InputData;

typedef struct {
    float carX;
    float carY;
    float carZ;

    Quaternion q;
    //下位1ビット目に発射ボタンを押したかどうか,下位2ビット目に地面の上にいるかの情報を仕込む
    uint8_t param;
    uint8_t HP;
    
}CarInfo;

typedef struct {
    uint8_t gunId;
    //char name[] //todo
}ClientData;

typedef union {
    InputData inputData;
    CarInfo carInfo;
    ClientData clientData;
}NetworkContainer_u;



typedef struct{
    uint8_t id;                     //id
    char order;                     //命令
    NetworkContainer_u container;   //内容
}NetworkContainer;

/**
 * @brief 車の情報
 */
typedef struct car_t
{
    uint8_t id;                 //操作するプレイヤーのid
    Polygon *collisionBox;      //当たり判定の直方体
    ObjInfo *model;             //3Dモデルの情報
    Vec3f center;               //中心座標
    Vec3f preCenter;            //1フレーム前の中心座標
    Vec3f velocity;             //速度
    Vec3f direction;            //方向ベクトル
    Vec3f preCoordOfVertexs[8]; //1フレーム前の各頂点の座標
    float hp;                   //HP
    float speed;                //スピード(UI専用)
    int place;                  //順位
    Gun *gun;                   //所持する銃
    int rapNum;                 //周回数
    int checkPointNum;          //次のチェックポイント

    Quaternion q;
    Quaternion q_pre;
    SDL_bool shotFlag;          //発射フラグ
    SDL_bool isOnGround;        //地面にいるかどうか

    Polygon *nextPlane;         //次のチェックポイントの平面
    CheckPoint *nextCheckPoint; //次のチェックポイントの座標

    Vec3f centerZero;
    Vec3f vertZero[8];

    SDL_bool isGoaled;          //ゴールしたかどうか
}Car;

/**
 * @brief 銃の番号
 */
typedef enum{
    Pistol,      //ピストル
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

typedef struct gun_t{
    GunKinds kind;                  //銃の種類
    int carId;                      //所属する車
    int bulletNum;                  //残り弾薬数
    const int maxBulletNum;         //最大弾薬数
    const int reloadFrame;          //リロードにかかるフレーム
    int reloadFrameNow;             //現在のリロードのフレーム
    const float damage;             //攻撃力
    const int fireCoolFrame;        //銃発射のクールタイム(≒連射レート)
    int fireCoolFrameNow;           //現在の発射のクールタイム
    float ammoSpeed;                //弾薬の速度
    int maxAmmoLivingFrame;         //弾薬の残存フレーム
    float ammoRadius;               //弾薬の半径
    ObjInfo *model;                 //3Dモデルの情報
}Gun;

typedef struct {
    GunKinds kind;                  //弾薬の種類
    Vec3f velocity;                 //速度
    int maxLivingFrame;             //最大残存フレーム
    int livingFrameNow;             //今の残存フレーム
    Polygon *model;                 //弾薬のモデル
    Vec3f color;                    //弾薬の色
    float damage;                   //攻撃力
    Vec3f center;                   //中心座標
    Vec3f preCoord;                 //1フレーム前の座標
    char id[6];                     //id
    int carId;                      //所属する車
    float radius;                   //半径
}Ammo;

// 武器情報配列を宣言
extern const WeaponInfo weapon_info[WEAPON_TYPE_MAX];



typedef struct checkPoint_t{
    Vec3f coord;
    Vec3f normal;
}CheckPoint;

typedef struct course_t{
    Polygon *courseModel;           //コースのモデル
    List *courseCollision;          //コースのあたり判定のリスト
    List *checkPointPlaneList;      //チェックポイントの平面のリスト
    List *checkPointPointList;      //チェックポイントのリスト
    int checkPointNum;              //チェックポイントの数
}Course;

/**
 * @brief メインゲームのシーン
 */
typedef struct {
    List *polygonList;  //立体図形のリスト
    List *cars;         //車のリスト
    List *UIList;       //UIのリスト
    List *ammoList;     //弾薬のリスト
    Camera *camera;     //カメラ
    Car *myCar;         //自分の操作する車
    Course *course;     //コースの構造体
    Polygon *checkPointPlaneZero;   //最初のチェックポイントの平面
    CheckPoint *checkPointZero;     //最初のチェックポイントの座標
    int goaledPlayerNum;            //ゴールしたプレイヤーの数
    int sendInputDataPlayerNum;     //入力データを送信するプレイヤーの数
    int sendCarInfoPlayerNum;       //車情報を送信するプレイヤーの数
    int count;                      //カウントダウン
    GLuint bulletTextureID;
}MainScene;

/* car.c */
Car *createCar(List *list , uint8_t id , Vec3f coord , GunKinds kind , Polygon *nextPlaneZero , CheckPoint *nextCheckPointZero);
void displayCars(List *list);
void moveCar(List *carList , List *PolygonList , int count);
//void destroyCar(Car *car);
void damageCar(Car *car , float damage);
Car *getCarFromId(List *carList , uint8_t id);
void collisionCars(List *carList);
void teleportCarEX(Car *car);
void rotateCarEX(Car *car);
void updateCarCenter(Car *car);

/* gun.c */
void register_ammoList(List *list);
Gun* createGun(GunKinds kind , int carId);
void fireGun(Car *car , Gun *gun);
Ammo* createAmmo(Gun *gun , Vec3f coord , Vec3f direct);
void updateAmmos(void);
void destroyAmmo(Ammo *ammo);
void displayAmmos(void);
void updateGuns(List *carList);
void collisionAmmoCars(List *carList);
void collisionAmmoCars_c(List *carList);

/* course_manager.h */
Course *createCourse(Polygon **checkPointPlaneZero , CheckPoint **checkPointZero);
void checkCarCheckPoint(List *carList , Course *course);
void carPlaceAlgorithmSetup(void);
void updatePlace(void);

/* main_scene.c */
int mainScene(void);


extern GameManager myGameManager;