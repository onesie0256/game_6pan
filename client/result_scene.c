/**
 * @file result_scene.c
 * @brief リザルト画面のシーン処理
 */
#include "client.h"

/**
 * @name resultScene
 * @brief リザルト画面のメインループ
 * @return ゲームが終了するなら0, そうでなければ1を返す
 */
int resultScene(void)
{
	static SDL_bool setupFlag = SDL_TRUE;
	static ResultScene resultState = {0};

	if (setupFlag) {
		// リザルト画面状態の初期化
		if (myGameManager.scene != NULL) {
			MainScene *mainScene = (MainScene *)myGameManager.scene;
			if (mainScene->myCar != NULL) {
				resultState.rank = mainScene->myCar->place;
				printf("DEBUG: resultState.rank = %d\n", resultState.rank);
			} else {
				resultState.rank = 4;  
			}
		} else {
			resultState.rank = 4;  
		}
		setupFlag = SDL_FALSE;
	}

	if (myGameManager.quitRequest == SDL_TRUE) {
		setupFlag = SDL_TRUE;
		SDL_Delay(100);
		// シーンの破棄
		if (myGameManager.scene != NULL) {
			destroyScene();
		}
		return 0; // ゲーム終了
	}

	// 画面全体をクリア（ゲーム中のUIを削除）
	if (myGameManager.UI) {
		SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGB(myGameManager.UI->format, 0, 0, 0));
	}

	UI_updateResultSurface(&resultState);
	draw(NULL);
	
	return 1;
}