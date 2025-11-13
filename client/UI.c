#include "client.h"

//UI初期化:TTFの初期化とフォントの読み込み
int UI_init(void)
{
	/* TTFがまだ初期化されていなければ初期化 */
	if (TTF_WasInit() == 0) {
		if (TTF_Init() == -1) {
			fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
		}
	}

	if (myGameManager.fonts[0] == NULL) {
		/* フォント読み込み */
		TTF_Font *f = TTF_OpenFont("assets/fonts/Aircruiser3Dital.ttf", 36);
		if (f) {
			myGameManager.fonts[0] = f;
		}
	}

	return 0;
}

/* 中央揃えでテキストを描画する。フォントがNULLの場合はスキップ */
static void UI_renderTextCentered(const char *text, int y, SDL_Color color)
{
	if (myGameManager.renderer == NULL) return;

	if (myGameManager.fonts[0]) {
		SDL_Surface *s = TTF_RenderUTF8_Blended(myGameManager.fonts[0], text, color);
		if (s) {
			SDL_Texture *tex = SDL_CreateTextureFromSurface(myGameManager.renderer, s);
			if (tex) {
				int w = s->w, h = s->h;
				int winW = myGameManager.windowW, winH = myGameManager.windowH;
				/* 水平中央、指定Y座標に配置 */
				SDL_Rect dst = { (winW - w) / 2, y, w, h };
				SDL_RenderCopy(myGameManager.renderer, tex, NULL, &dst);
				SDL_DestroyTexture(tex);
			}
			SDL_FreeSurface(s);
		}
	}
}

/* タイトル画面の描画: 背景、タイトルテキスト、「Enter を押してください」プロンプト */
void UI_renderTitleScreen(TitleScene *titleScene)
{
	if (myGameManager.renderer == NULL || titleScene == NULL) return;

	/* ウィンドウサイズを最新の状態に更新 */
	if (myGameManager.window) {
		int w, h;
		SDL_GetWindowSize(myGameManager.window, &w, &h);
		myGameManager.windowW = (Uint16)w;
		myGameManager.windowH = (Uint16)h;
	}

	//背景描画
	SDL_SetRenderDrawColor(myGameManager.renderer, 255, 255, 255, 255);
	SDL_RenderClear(myGameManager.renderer);


	//タイトルボックス描画 
	SDL_Rect titleBox = { myGameManager.windowW/8, myGameManager.windowH/3,
						  myGameManager.windowW*3/4, myGameManager.windowH/4 };
	SDL_SetRenderDrawColor(myGameManager.renderer, 60, 120, 200, 255);
	SDL_RenderFillRect(myGameManager.renderer, &titleBox);

	/* ビート効果付きタイトルテキスト描画
	 * beatScale を使用してテキストの大きさを変更
	 */
	SDL_Color white = { 255, 255, 255, 255 };
	if (myGameManager.fonts[0]) {
		/* ビート効果のスケールに応じたテキスト描画 */
		SDL_Surface *s = TTF_RenderUTF8_Blended(myGameManager.fonts[0], "3D ガン★カート", white);
		if (s) {
			SDL_Texture *tex = SDL_CreateTextureFromSurface(myGameManager.renderer, s);
			if (tex) {
				int baseW = s->w, baseH = s->h;
				/* スケール適用 */
				int scaledW = (int)(baseW * titleScene->beatScale);
				int scaledH = (int)(baseH * titleScene->beatScale);
				int winW = myGameManager.windowW, winH = myGameManager.windowH;
				SDL_Rect dst = {
					(winW - scaledW) / 2,
					myGameManager.windowH / 3 + 10,
					scaledW,
					scaledH
				};
				SDL_RenderCopy(myGameManager.renderer, tex, NULL, &dst);
				SDL_DestroyTexture(tex);
			}
			SDL_FreeSurface(s);
		}
	}

	/* Enter プロンプト点滅表示 */
	Uint32 ticks = SDL_GetTicks();
	if ((ticks / 500) % 2 == 0) {
		UI_renderTextCentered("Press Enter to Start", myGameManager.windowH*3/4 + 50, white);
	}

	/* 注記: SDL_RenderPresent は呼び出し元で呼び出す */
}

//ロード画面の描画 
void UI_renderLoadingScreen(const char *message)
{
	if (myGameManager.renderer == NULL) return;
	SDL_SetRenderDrawColor(myGameManager.renderer, 20, 20, 20, 255);
	SDL_RenderClear(myGameManager.renderer);
	SDL_Color white = { 255, 255, 255, 255 };
	if (message) UI_renderTextCentered(message, myGameManager.windowH/2, white);
}

//メインゲーム画面の描画
void UI_renderMainScreen(void)
{
	if (myGameManager.renderer == NULL) return;
	SDL_SetRenderDrawColor(myGameManager.renderer, 10, 30, 10, 255);
	SDL_RenderClear(myGameManager.renderer);
	SDL_Color white = { 220, 220, 220, 255 };
	UI_renderTextCentered("Main Game Screen", myGameManager.windowH/2 - 20, white);
}

/* UI リソースのクリーンアップ（フォントのクローズ） */
void UI_cleanup(void)
{
	for (int i = 0; i < FONT_MAX; ++i) {
		if (myGameManager.fonts[i]) {
			TTF_CloseFont(myGameManager.fonts[i]);
			myGameManager.fonts[i] = NULL;
		}
	}
	/* 他のモジュールが TTF に依存する可能性があるため、TTF_Quit は呼び出さない */
}
#include <SDL2/SDL_image.h>

void createUIwithText(SceneKinds group , char *text , SDL_Rect dst , int fontNum , SDL_Color color)
{
    UI *rtn = (UI *)malloc(sizeof(UI));
    rtn->dst = dst;
    rtn->isDisplay = SDL_FALSE;
    rtn->group = group;
    rtn->surface = TTF_RenderUTF8_Blended(myGameManager.fonts[fontNum] , text , color);
    addListNode(myGameManager.UIList , rtn , NULL);
}

void createUIwithIMG(SceneKinds group , char *filename , SDL_Rect dst)
{
    UI *rtn = (UI *)malloc(sizeof(UI));
    rtn->dst = dst;
    rtn->isDisplay = SDL_FALSE;
    rtn->group = group;
    rtn->surface = IMG_Load(filename);
    if (!rtn->surface){
        printf("IMG_Load error: %s\n", IMG_GetError());
    }
    else
    addListNode(myGameManager.UIList , rtn , NULL);
    
}

void deleteUI(SceneKinds group)
{
    ListNode *temp = myGameManager.UIList->head;
    while (temp != NULL){
        ListNode *next = temp->next;
        UI *data = (UI*)temp->data;
        if (data->group & group){
            if(data->surface) SDL_FreeSurface(data->surface);
            free(data);
            deleteListNode(myGameManager.UIList , temp);
        }
        temp = next;
    }
}
