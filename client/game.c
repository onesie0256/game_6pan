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

    switch (myGameManager.sceneNum)
    {
    case Scene_Title:
        endFlag = titleScene();
        break;
    
    default:
        break;
    }

    draw(NULL);

    return endFlag;
}

int init(void)
{
    return 0;
}