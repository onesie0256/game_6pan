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

    return endFlag;
}
