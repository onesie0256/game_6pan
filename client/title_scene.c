/**
 * @file title_scene.c
 * @brief タイトル画面のシーン処理
 */
#include "client.h"
#include <math.h>

/**
 * @name titleScene
 * @brief タイトル画面のメインループ
 * @return ゲームが終了するなら0, そうでなければ1を返す
 */
int titleScene(void)
{
	static SDL_bool setupFlag = SDL_TRUE;
	static TitleScene titleState = {0};

	if (setupFlag) {

	UI_init(); //UI初期化

	//タイトル画面状態の初期化 
	titleState.animationTimer = 0;
	titleState.beatScale = 1.0f;
	titleState.carX = 0.0f;
	titleState.carAnimState = 1;

	setupFlag = SDL_FALSE;
	}

	//イベント処理:Enter(Xにする)で次シーンへ  

	
		if (myGameManager.quitRequest == SDL_TRUE) {
			setupFlag = SDL_TRUE;
			return 0; // ゲーム終了 
		}
		else{
			if (myGameManager.keyNow[K_ENTER] == SDL_TRUE ) {
				myGameManager.sceneID = Scene_Wait;
			}
		
		#ifdef USE_JOY

		if (myGameManager.joyBotton[JOY_Home] == SDL_TRUE ) {
			myGameManager.sceneID = Scene_Wait;
			break;
		}

		#endif 
		}

		
		

	//アニメーション更新
	titleState.animationTimer += 16; 

	/* ビート効果（拡大縮小）: sin波で0.9～1.1をループ */
	float beatPhase = (titleState.animationTimer % 1000) / 1000.0f; /* 0.0～1.0 */
	titleState.beatScale = 1.0f + 0.15f * sinf(beatPhase * 2.0f * 3.14159f);

	/* 車アニメーション5秒周期で左から右へループ
	 * 素材が揃ったら、調整
	 */
	float carPhase = (titleState.animationTimer % 5000) / 5000.0f; /* 0.0～1.0, 5秒周期 */
	titleState.carX = carPhase * myGameManager.windowW;

	/*
	//タイトルUI描画 
	UI_renderTitleScreen(&titleState);
		SDL_RenderPresent(myGameManager.renderer);

		SDL_Delay(16); 
	}
	*/
	UI_renderTitleScreen(&titleState);
	draw(NULL);

	/* 注記: フォントは今後の画面でも使用するため、ここではクリーンアップしない */
	return 1;
}

