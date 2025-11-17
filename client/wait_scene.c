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
    waitSceneState.selectedWeaponIndex = 0; // 初期選択

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return 0; // ゲーム終了
            }
            // キーが押されたときのイベント
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_LEFT: // 左キー
                        waitSceneState.selectedWeaponIndex--;
                        if (waitSceneState.selectedWeaponIndex < 0) {
                            waitSceneState.selectedWeaponIndex = WEAPON_TYPE_MAX - 1; // 末尾にループ
                        }
                        break;
                    case SDLK_RIGHT: // 右キー
                        waitSceneState.selectedWeaponIndex++;
                        waitSceneState.selectedWeaponIndex %= WEAPON_TYPE_MAX; // 先頭にループ
                        break;
                }
            }
        }

        // 待機画面のUIを描画
        UI_renderWaitScreen(&waitSceneState);
        SDL_RenderPresent(myGameManager.renderer);

        SDL_Delay(16);
    }

    return 1; // 次のシーンへ
}
