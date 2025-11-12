/**
 * @file hoge
 */
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define KEY_MAX 10
#define JOY_KEY_MAX 10
#define FONT_MAX 5
#define FPS_f 90.0f

typedef struct{
    uint8_t SceneID;   //シーンのID
}Scene;
/**
 * @name GameManager
 * @brief ゲーム全体の情報を格納する
 */
typedef struct
{
    uint8_t myID;                   //ID
    uint8_t playerNum;              //プレイヤー人数
    //to do 自分の選んだ銃の情報
    char *serverName;               //サーバーの名前
    SDL_Window *window;
    SDL_Renderer *renderer;         //レンダラ
    SDL_GLContext context;          //コンテキスト
    Uint16 windowW;                 //ウィンドウの横幅
    Uint16 windowH;                 //ウィンドウの縦幅
    TTF_Font *fonts[FONT_MAX];      //フォント
    Scene *scene;                   //シーン
    SDL_Surface *UI;                //UI用のサーフェイス
    List *UIList;
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
    //to do
}Car;

/**
 * @brief 銃の構造体
 */
typedef struct
{
    //to do
}Gun;

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