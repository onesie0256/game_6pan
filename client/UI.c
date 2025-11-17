#include "client.h"

//武器情報の定義(client.hでextern宣言したものの実体)
const WeaponInfo weapon_info[WEAPON_TYPE_MAX] = {
    { Pistl,   "PISTOL",    "assets/images/pistol.png" },
    { Shotgun, "SHOTGUN",   "assets/images/shotgun.png" },
    { Sniper,  "SNIPER",    "assets/images/sniper.png" }
};

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
		TTF_Font *f = TTF_OpenFont("assets/fonts/Aircruiser3Dital.ttf", 100);
		if (f) {
			myGameManager.fonts[0] = f;
		}
	}
	//小さいフォントを読み込む(ロード画面用)
	if (myGameManager.fonts[1] == NULL) {
		TTF_Font *f = TTF_OpenFont("assets/fonts/Aircruiser.ttf", 24); // サイズを小さく
		if (f) {
			myGameManager.fonts[1] = f;
		}
	}

	//待機画面用のフォントを読み込む
	if (myGameManager.fonts[2] == NULL) {
		TTF_Font *f = TTF_OpenFont("assets/fonts/KiwiMaru-Regular.ttf", 32);
		if (f) {
			myGameManager.fonts[2] = f;
		}
	}

	return 0;
}

//中央揃えでテキストを描画する。フォントがNULLの場合はスキップ
static void UI_renderTextCentered(TTF_Font *font, const char *text, int y, SDL_Color color)
{
	if (myGameManager.renderer == NULL || font == NULL) return;

	//if (myGameManager.fonts[0]) { // fontがNULLでないことは上でチェック済み
		SDL_Surface *s = TTF_RenderUTF8_Blended(font, text, color);
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
	//}
}

//タイトル画面の描画: 背景、タイトルテキスト、「Enter を押してください」プロンプト
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


	//ビート効果を適用したタイトルボックスの描画
	float scale = titleScene->beatScale;
	int baseBoxW = myGameManager.windowW / 2;
	int baseBoxH = myGameManager.windowH / 5;
	int scaledBoxW = (int)(baseBoxW * scale);
	int scaledBoxH = (int)(baseBoxH * scale);

	SDL_Rect titleBox = { (myGameManager.windowW - scaledBoxW) / 2,
						  myGameManager.windowH / 3 + (baseBoxH - scaledBoxH) / 2,
						  scaledBoxW, scaledBoxH };
	SDL_SetRenderDrawColor(myGameManager.renderer, 60, 120, 200, 255);
	SDL_RenderFillRect(myGameManager.renderer, &titleBox);

	/* ビート効果付きタイトルテキスト描画
	 * scale を使用してテキストの大きさを変更
	 */
	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color black = { 0, 0, 0, 255 };
	if (myGameManager.fonts[0]) {
		/* ビート効果のスケールに応じたテキスト描画 */
		SDL_Surface *s = TTF_RenderUTF8_Blended(myGameManager.fonts[0], "3D GUN KART", white);
		if (s) {
			SDL_Texture *tex = SDL_CreateTextureFromSurface(myGameManager.renderer, s);
			if (tex) {
				int baseW = s->w, baseH = s->h;
				/* スケール適用 */
				int scaledW = (int)(baseW * scale);
				int scaledH = (int)(baseH * scale);
				int winW = myGameManager.windowW, winH = myGameManager.windowH;
				SDL_Rect dst = {
					(winW - scaledW) / 2,
					myGameManager.windowH / 3 + (baseBoxH - scaledH) / 2,
					scaledW,
					scaledH
				};
				SDL_RenderCopy(myGameManager.renderer, tex, NULL, &dst);
				SDL_DestroyTexture(tex);
			}
			SDL_FreeSurface(s);
		}
	}

	//Enterプロンプト点滅表示
	Uint32 ticks = SDL_GetTicks();
	if ((ticks / 500) % 2 == 0) {
		UI_renderTextCentered(myGameManager.fonts[0], "Press Enter to Start", myGameManager.windowH*3/4 + 50, black);
	}

	//注記: SDL_RenderPresentは呼び出し元で呼び出す
}

//待機画面の描画
void UI_renderWaitScreen(WaitScene *waitScene)
{
	if (myGameManager.renderer == NULL || waitScene == NULL) return;

	/* ウィンドウサイズを最新の状態に更新 */
	if (myGameManager.window) {
		int w, h;
		SDL_GetWindowSize(myGameManager.window, &w, &h);
		myGameManager.windowW = (Uint16)w;
		myGameManager.windowH = (Uint16)h;
	}

	//背景描画(ダークブルー)
	SDL_SetRenderDrawColor(myGameManager.renderer, 30, 30, 50, 255);
	SDL_RenderClear(myGameManager.renderer);

	//選択中の武器情報を取得
	int index = waitScene->selectedWeaponIndex;
	const WeaponInfo* selected_weapon = &weapon_info[index];

	//画面中央: 武器名を描画
	SDL_Color white = { 255, 255, 255, 255 };
	UI_renderTextCentered(myGameManager.fonts[0], selected_weapon->name, myGameManager.windowH / 2 - 50, white);

	//画面中央: 武器画像を描画
	//TODO: 3Dモデルに置き換えるまでの仮表示として画像を描画
	

	//画面上部: 操作説明を描画
	UI_renderTextCentered(myGameManager.fonts[2], "左右キーで武器を選択", 60, white);

	
	//画面右下: 「通信待機中．．．」を表示
	if (myGameManager.fonts[2]) { // 待機画面用フォント(インデックス2)を使用
		SDL_Surface *s = TTF_RenderUTF8_Blended(myGameManager.fonts[2], "通信待機中．．．", white);
		if (s) {
			SDL_Texture *tex = SDL_CreateTextureFromSurface(myGameManager.renderer, s);
			if (tex) {
				//右下から少し内側に配置
				SDL_Rect dst = { myGameManager.windowW - s->w - 60, myGameManager.windowH - s->h - 20, s->w, s->h };
				SDL_RenderCopy(myGameManager.renderer, tex, NULL, &dst);
				SDL_DestroyTexture(tex);
			}
			SDL_FreeSurface(s);
		}
	}
}

//ロード画面の描画
void UI_renderLoadingScreen(const char *message)
{
	if (myGameManager.renderer == NULL) return;
	SDL_SetRenderDrawColor(myGameManager.renderer, 20, 20, 20, 255);
	SDL_RenderClear(myGameManager.renderer);
	SDL_Color white = { 255, 255, 255, 255 };
	if (message) UI_renderTextCentered(myGameManager.fonts[0], message, myGameManager.windowH/2, white);
}

//メインゲーム画面の描画
void UI_renderMainScreen(void)
{
	if (myGameManager.renderer == NULL) return;
	SDL_SetRenderDrawColor(myGameManager.renderer, 10, 30, 10, 255);
	SDL_RenderClear(myGameManager.renderer);
	SDL_Color white = { 220, 220, 220, 255 };
	UI_renderTextCentered(myGameManager.fonts[0], "Main Game Screen", myGameManager.windowH/2 - 20, white);
}

//UIリソースのクリーンアップ(フォントのクローズ)
void UI_cleanup(void)
{
	for (int i = 0; i < FONT_MAX; ++i) {
		if (myGameManager.fonts[i]) {
			TTF_CloseFont(myGameManager.fonts[i]);
			myGameManager.fonts[i] = NULL;
		}
	}
	//他のモジュールがTTFに依存する可能性があるため、TTF_Quitは呼び出さない
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
