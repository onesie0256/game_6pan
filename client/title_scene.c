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
	TitleScene titleScene;  //タイトル画面状態の初期化

	UI_init(); 

	//タイトル画面状態の初期化 
	titleScene.animationTimer = 0;
	titleScene.beatScale = 1.0f;

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

		//タイトルUI描画 
		UI_renderTitleScreen(&titleScene);


		draw(NULL);

		SDL_Delay(16); 
	}

	return 1;
}