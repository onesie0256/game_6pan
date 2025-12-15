/**
 * @file wait_scene.c
 * @brief 待機画面のシーン処理
 */
#include "client.h"

/**
 * @name waitScene
 * @brief 待機画面のメインループ
 * @return ゲームが終了するなら0, そうでなければ1を返す
 */
int waitScene(void)
{
    // 待機画面の状態を初期化
    WaitScene waitSceneState;
    waitSceneState.selectedWeaponIndex = 0; 
    myGunSent = SDL_FALSE;
    gunReadyCount = 0;


    int running = 1;
    while (running) {
            if (myGameManager.quitRequest == SDL_TRUE) {
                SDL_Delay(100);
                return 0; // ゲーム終了
            }
                    if (isKeyDowned(K_LEFT) == SDL_TRUE){
                         //左キー
                        waitSceneState.selectedWeaponIndex--;
                        if (waitSceneState.selectedWeaponIndex < 0) {
                            waitSceneState.selectedWeaponIndex = WEAPON_TYPE_MAX - 1; // 末尾にループ
                        }
                    }
                    if (isKeyDowned(K_RIGHT) == SDL_TRUE) {
                        //右キー
                        waitSceneState.selectedWeaponIndex++;
                        waitSceneState.selectedWeaponIndex %= WEAPON_TYPE_MAX; // 先頭にループ
                    }
                    if (isKeyDowned(K_ENTER) == SDL_TRUE) {
                        // 選択した武器を反映
                        myGameManager.gunId = weapon_info[waitSceneState.selectedWeaponIndex].kind;
                        send_gun_data(); // 銃データをサーバに送信
                        myGunSent = SDL_TRUE;     // 準備完了
                    } 
                    if (myGameManager.sceneID == Scene_Main) {
                    // 全員分のGを受信済み
                    return 1;
                    }

        UI_updateWaitSurface(&waitSceneState);

        draw(NULL);
        SDL_Delay(16);
        
    }

    return 1; //メインシーンへ
}