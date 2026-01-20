/**
 * @file title_scene.c
 * @brief タイトル画面のシーン処理
 */
#include "client.h"
#include <math.h>

static int flag = 0;

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

	setupFlag = SDL_FALSE;
	}

	//イベント処理:Enter(Joy-conではX)で次シーンへ  
	if (myGameManager.quitRequest == SDL_TRUE) {
		setupFlag = SDL_TRUE;
		SDL_Delay(100);
		return 0; // ゲーム終了 
	}
	else {if (flag == 0 && (isKeyDowned(K_ENTER) || myGameManager.joyBotton[JOY_X])) {
			flag = 1;

			Audio_PlaySE(SE_START);

			SDL_SetHint(SDL_HINT_IME_INTERNAL_EDITING , "1");
			//SDL_SetHint("SDL_IME_SHOW_UI" , "1");
			SDL_Rect rect = (SDL_Rect){0,0,myGameManager.windowH,myGameManager.windowW};
			SDL_SetTextInputRect(&rect);
			memset(myGameManager.myName , 0 , MYNAME_MAX);
			SDL_StartTextInput();
			titleState.flag = 1;
		}
	}

	titleState.animationTimer++; //タイマーを更新

	if (flag == 0){
		//ビート効果（拡大縮小）: sin波でループ
		float beatPhase = (titleState.animationTimer % 100) / 100.0f; 
		titleState.beatScale = 1.0f + 0.15f * sinf(beatPhase * 2.0f * 3.14159f);
	}
	else if (flag == 1){
		if (isKeyDowned(K_ENTER)){
			SDL_StopTextInput();
			myGameManager.sceneID = Scene_Wait;
			Audio_PlaySE(SE_START);
			return 1;
		}
	}

	UI_updateTitleSurface(&titleState);
	draw(NULL);
	
	return 1;
}

