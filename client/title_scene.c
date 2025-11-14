#include "client.h"
#include <math.h>

int titleScene(void)
{
	//タイトル画面状態の初期化
	TitleScene titleScene;

	UI_init(); //UI初期化

	//タイトル画面状態の初期化 
	titleScene.animationTimer = 0;
	titleScene.beatScale = 1.0f;
	titleScene.carX = 0.0f;
	titleScene.carAnimState = 1;

	//イベント処理:Enter(Xにする)で次シーンへ  
	SDL_Event e;
	int running = 1;
	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				return 0; // ゲーム終了 
			}
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
					myGameManager.sceneID = Scene_Wait;
					running = 0;
					break;
				}
			}
		}

		//アニメーション更新
		titleScene.animationTimer += 16; 

		/* ビート効果（拡大縮小）: sin波で0.9～1.1をループ */
		float beatPhase = (titleScene.animationTimer % 1000) / 1000.0f; /* 0.0～1.0 */
		titleScene.beatScale = 1.0f + 0.15f * sinf(beatPhase * 2.0f * 3.14159f);

		/* 車アニメーション5秒周期で左から右へループ
		 * 素材が揃ったら、調整
		 */
		float carPhase = (titleScene.animationTimer % 5000) / 5000.0f; /* 0.0～1.0, 5秒周期 */
		titleScene.carX = carPhase * myGameManager.windowW;

		//タイトルUI描画 
		UI_renderTitleScreen(&titleScene);
		SDL_RenderPresent(myGameManager.renderer);

		SDL_Delay(16); 
	}

	/* 注記: フォントは今後の画面でも使用するため、ここではクリーンアップしない */
	return 1;
}
