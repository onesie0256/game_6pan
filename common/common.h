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

#define KEY_MAX 10
#define JOY_KEY_MAX 10
#define FONT_MAX 5
#define FPS_f 90.0f

#define MAX_LEN_NAME 100
#define BACKLOG 10      // 同時接続待ち上限
#define MAX_Clients 10


struct list_t;
typedef struct list_t List;

typedef struct client_t{
    uint8_t id;                 //クライアントのid
    uint8_t gunId;              //銃の種類
    SDL_bool keyNow[KEY_MAX];   //キーの状態
    SDL_bool keyPrev[KEY_MAX];  //キーの状態(1フレーム前)
    SDL_bool joyBotton[JOY_KEY_MAX];  //ジョイコンのボタンの状態
    SDL_bool joyBottonPrev[JOY_KEY_MAX];
    SDL_bool gunReady;
}Client;




/**
 * @name GameManager
 * @brief ゲーム全体の情報を格納する
 */
typedef struct
{
    SDL_bool quitRequest;           //SDL_QUITが通知されたらTRUE
    uint8_t myID;                   //ID
    uint8_t playerNum;              //プレイヤー人数
    uint8_t gunId;                  //自分の銃の種類
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
    SDL_bool keyNow[KEY_MAX]; //キーの状態
    SDL_bool keyPrev[KEY_MAX];//キーの状態(1フレーム前)
    #ifdef USE_JOY
    SDL_bool joyBotton[JOY_KEY_MAX];  //ジョイコンのボタンの状態
    SDL_bool joyBottonPrev[JOY_KEY_MAX];
    float StickX;            //ジョイスティックのx方向の傾き
    float StickY;            //ジョイスティックのy方向の傾き
    #endif

    Client clients[MAX_Clients];    //クライアントのリスト

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



typedef struct{
    uint8_t id;                 //id
    char order;                 //命令
    uint8_t option;             //お好きにどうぞ
    uint16_t keyInputs;         //キーの情報
    uint16_t joyKeyInputs;      //ジョイスティックボタン
    float stickX;               //スティック傾きX
    float stickY;               //スティック傾きY
}NetworkContainer;


 

extern GameManager myGameManager;