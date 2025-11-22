#include <SDL2/SDL_image.h>
#include "client.h"

//武器情報の定義(client.hでextern宣言したものの実体)
const WeaponInfo weapon_info[WEAPON_TYPE_MAX] = {
    { Pistl,   "PISTOL",    "assets/images/pistol.png" },
    { Shotgun, "SHOTGUN",   "assets/images/shotgun.png" },
    { Sniper,  "SNIPER",    "assets/images/sniper.png" }
};

/**
 * @name UI_init
 * @brief UIの初期化（SDL、TTF）
 * 
 */
int UI_init(void)
{
	/*
	//SDLを初期化 
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return -1;
	}
	*/

	//TTFを初期化 
	if (TTF_WasInit() == 0) {
		if (TTF_Init() == -1) {
			fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
		}
	}

	if (myGameManager.fonts[0] == NULL) {
		/* フォント読み込み */
		TTF_Font *f = TTF_OpenFont("assets/fonts/Aircruiser3Dital.ttf", 100);
		if (f) {
			if (f)
			myGameManager.fonts[0] = f;
		}
		else if (f = TTF_OpenFont("./../assets/fonts/Aircruiser3Dital.ttf", 100)){
			myGameManager.fonts[0] = f;
		}
	}
	//小さいフォントを読み込む(待機画面用)
	if (myGameManager.fonts[1] == NULL) {
		TTF_Font *f = TTF_OpenFont("assets/fonts/Aircruiser.ttf", 24); // サイズを小さく
		if (f) {
			myGameManager.fonts[1] = f;
		}
		else if (f = TTF_OpenFont("./../assets/fonts/aircruiser.ttf", 24)){
			myGameManager.fonts[1] = f;
		}
	}

	//待機画面用のフォントを読み込む
	if (myGameManager.fonts[2] == NULL) {
		TTF_Font *f = TTF_OpenFont("assets/fonts/KiwiMaru-Regular.ttf", 32);
		if (f) {
			myGameManager.fonts[2] = f;
		}
		else if (f = TTF_OpenFont("./../assets/fonts/KiwiMaru-Regular.ttf", 32)){
			myGameManager.fonts[2] = f;
		}
	}

	return 0;
}

/**
 * @brief 中央揃えでテキストを描画する(UI.c内のみで使用)
 * @param font 使用するフォント
 * @param text 描画するテキスト
 * @param y 描画するY座標
 * @param color テキストの色
 */
static void UI_renderTextCentered(TTF_Font *font, const char *text, int y, SDL_Color color)
{
	if (myGameManager.UI == NULL) return;

	SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font, text, color);
	if (textSurface) {
		SDL_Rect dst;
		dst.x = (myGameManager.UI->w - textSurface->w) / 2;
		dst.y = y;
		SDL_BlitSurface(textSurface, NULL, myGameManager.UI, &dst);
		SDL_FreeSurface(textSurface);
	}
}

/**
 * @brief タイトル画面を描画する
 * @param titleScene タイトルシーンの状態。アニメーションなどに使用します。
 */
void UI_renderTitleScreen(TitleScene *titleScene)
{
	if (myGameManager.UI == NULL || titleScene == NULL) return;

	// 背景描画（白色）
	SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGB(myGameManager.UI->format, 0, 255, 255));

	//ビート効果を適用したタイトルボックスの描画
	float scale = titleScene->beatScale;
	int baseBoxW = myGameManager.windowW / 2;
	int baseBoxH = myGameManager.windowH / 5;
	int scaledBoxW = (int)(baseBoxW * scale);
	int scaledBoxH = (int)(baseBoxH * scale);

	SDL_Rect titleBox = { (myGameManager.windowW - scaledBoxW) / 2,
						  myGameManager.windowH / 3 + (baseBoxH - scaledBoxH) / 2,
						  scaledBoxW, scaledBoxH };

	SDL_FillRect(myGameManager.UI, &titleBox, SDL_MapRGB(myGameManager.UI->format, 60, 120, 200));

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Color black = { 0, 0, 0, 255 };
	if (myGameManager.fonts[0]) {
		/* ビート効果のスケールに応じたテキスト描画 */
		SDL_Surface *textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[0], "3D GUN KART", white);
		if (textSurface) {
			int baseW = textSurface->w, baseH = textSurface->h;
			/* スケール適用 */
			int scaledW = (int)(baseW * scale);
			int scaledH = (int)(baseH * scale);

			SDL_Rect dst = {
				(myGameManager.windowW - scaledW) / 2,
				myGameManager.windowH / 3 + (baseBoxH - scaledH) / 2,
				scaledW,
				scaledH
			};

			SDL_BlitScaled(textSurface, NULL, myGameManager.UI, &dst);
			SDL_FreeSurface(textSurface);
		}
	}

	//Enterプロンプト点滅表示
	Uint32 ticks = SDL_GetTicks();
	if ((ticks / 500) % 2 == 0) {
		UI_renderTextCentered(myGameManager.fonts[0], "Press Enter to Start", myGameManager.windowH*3/4 + 50, black);
	}
}

/**
 * @brief 待機画面を描画する
 * @param waitScene 待機シーンの状態。選択中の武器表示などに使用
 */
void UI_renderWaitScreen(WaitScene *waitScene)
{
	if (myGameManager.UI == NULL || waitScene == NULL) return;

	//背景描画(水色)
	SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGB(myGameManager.UI->format, 0, 255, 255));

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
		SDL_Surface *textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[2], "通信待機中．．．", white);
		if (textSurface) {
			//右下から少し内側に配置
			SDL_Rect dst = { myGameManager.windowW - textSurface->w - 60, myGameManager.windowH - textSurface->h - 20, textSurface->w, textSurface->h };
			SDL_BlitSurface(textSurface, NULL, myGameManager.UI, &dst);
			SDL_FreeSurface(textSurface);
		}
	}
}

/**
 * @brief ロード画面を描画する
 * @param message 表示するメッセージ
 */
void UI_renderLoadingScreen(const char *message)
{
	if (myGameManager.UI == NULL) return;
	SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGB(myGameManager.UI->format, 20, 20, 20));
	SDL_Color white = { 255, 255, 255, 255 };
	if (message) UI_renderTextCentered(myGameManager.fonts[0], message, myGameManager.windowH/2, white);
}

/**
 * @brief メインゲーム画面のUIを描画する
 * @note 現在はプレースホルダーとしてテキストを表示するのみです。
 */
void UI_renderMainScreen(void)
{
	if (myGameManager.UI == NULL) return;
	SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGB(myGameManager.UI->format, 10, 30, 10));
	SDL_Color white = { 220, 220, 220, 255 };
	UI_renderTextCentered(myGameManager.fonts[0], "Main Game Screen", myGameManager.windowH/2 - 20, white);
}

/**
 * @name UI_cleanup
 * @brief UI関連のリソースを解放する
 * @note 読み込んだフォントを全て解放します。
 */
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

/**
 * @brief テキストからUI要素を作成し、UIリストに追加する
 * @param group UIが所属するシーン/グループ
 * @param text 表示するテキスト
 * @param dst 表示する矩形領域
 * @param fontNum 使用するフォントのインデックス
 * @param color テキストの色
 */
void createUIwithText(SceneKinds group , char *text , SDL_Rect dst , int fontNum , SDL_Color color)
{
    UI *rtn = (UI *)malloc(sizeof(UI));
    rtn->dst = dst;
    rtn->isDisplay = SDL_FALSE;
    rtn->group = group;
    rtn->surface = TTF_RenderUTF8_Blended(myGameManager.fonts[fontNum] , text , color);
    addListNode(myGameManager.UIList , rtn , NULL);
}

/**
 * @brief 画像ファイルからUI要素を作成し、UIリストに追加する
 * @param group UIが所属するシーン/グループ
 * @param filename 画像ファイルへのパス
 * @param dst 表示する矩形領域
 */
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

/**
 * @brief 指定されたグループのUI要素を削除する
 * @param group 削除するUIのグループ
 */
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