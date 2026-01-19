/**
 * @name game.c
 * @brief ゲームループの中身
 */

#include "client.h"

/**
 * @name gameLoop
 * @brief ゲームループの中身
 * @return ゲームが終了するなら0,そうでなければ1を返す
 */
int gameLoop(void)
{
    int endFlag = 1;

    SDL_Thread *delayThre = SDL_CreateThread(mainLoopDelay , "mainLoopDelay" , NULL);


    switch (myGameManager.sceneID)
    {
    case Scene_Title:
        endFlag = titleScene();
        break;
    case Scene_Wait:
        endFlag = waitScene();
        break;
    case Scene_Main:
        endFlag = mainScene();
    default:
        break;
    }


    SDL_WaitThread(delayThre , NULL);

    return endFlag;
}

int mainLoopDelay(void *arg)
{
    SDL_Delay(1000 / FPS_f);
    return 0;
}