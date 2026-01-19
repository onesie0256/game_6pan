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
    waitSceneState.isSendGunId = SDL_FALSE;

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
            running = 0;  //メイン画面へ
        } 

        UI_updateWaitSurface(&waitSceneState);

        draw(NULL);
            

        if (isKeyDowned(K_ENTER) == SDL_TRUE){
            myGameManager.sceneID = Scene_Main;
            running = 0;
        }

        if (isKeyDowned(K_ENTER) == SDL_TRUE){
            myGameManager.sceneID = Scene_Main;
            running = 0;
        }

        #ifdef USE_JOY

		if (myGameManager.joyBotton[JOY_X] == SDL_TRUE ) {
			myGameManager.sceneID = Scene_Main;
            running = 0;
			break;
		}   

		#endif 

        SDL_Delay(16);
    }

    myGameManager.gunId = waitSceneState.selectedWeaponIndex;
    myGameManager.scene = NULL;
    send_gunId();
    waitSceneState.isSendGunId = SDL_TRUE;
    UI_updateWaitSurface(&waitSceneState);
    draw(NULL);
    
    waitUntilAck();
    myGameManager.sceneID = Scene_Main;
    return 1; //メインシーンへ
}