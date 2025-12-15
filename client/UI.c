#include <SDL2/SDL_image.h>
#include "client.h"

//武器情報の定義(client.hでextern宣言したものの実体)
const WeaponInfo weapon_info[WEAPON_TYPE_MAX] = {
    { Pistol,   "PISTOL",    "assets/images/pistol.png" },
    { Shotgun, "SHOTGUN",   "assets/images/shotgun.png" },
    { Sniper,  "SNIPER",    "assets/images/sniper.png" }
};

/**
 * @name UI_init
 * @brief UIの初期化（TTFのみ）
 * 
 */
int UI_init(void)
{
	//TTFを初期化 
	if (TTF_WasInit() == 0) {
		if (TTF_Init() == -1) {
			fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
		}
	}

	if (myGameManager.fonts[0] == NULL) {
		// フォント読み込み 
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
static void UI_updateTextCenteredOnSurface(TTF_Font *font, const char *text, int y, SDL_Color color)
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
void UI_updateTitleSurface(TitleScene *titleScene)
{
	if (myGameManager.UI == NULL || titleScene == NULL) return;

	//背景描画（白色）
	SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGB(myGameManager.UI->format, 255, 255, 255));

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
		//ビート効果のスケールに応じたテキスト描画 
		SDL_Surface *textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[0], "3D GUN KART", white);
		if (textSurface) {
			int baseW = textSurface->w, baseH = textSurface->h;
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
	if ((SDL_GetTicks() / 700) % 2 == 0) { // 0.7秒ごとに点滅

	#ifdef USE_JOY
		UI_updateTextCenteredOnSurface(myGameManager.fonts[2], "キーボードで ENTER / Joy-Conで X を押してスタート", myGameManager.windowH * 3 / 4 + 50, black);
	#else
		UI_updateTextCenteredOnSurface(myGameManager.fonts[2], "キーボードで ENTER を押してスタート", myGameManager.windowH * 3 / 4 + 50, black);
	#endif
	}
}

/**
 * @brief 待機画面を描画する
 * @param waitScene 待機シーンの状態。選択中の武器表示などに使用
 */
void UI_updateWaitSurface(WaitScene *waitScene)
{
	if (myGameManager.UI == NULL || waitScene == NULL) return;

	//背景描画(水色)
	SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGB(myGameManager.UI->format, 224, 255, 255));

	//選択中の武器情報を取得
	int index = waitScene->selectedWeaponIndex;
	const WeaponInfo* selected_weapon = &weapon_info[index];

	//画面中央: 武器名を描画
	SDL_Color black = {0, 0, 0, 0};
	UI_updateTextCenteredOnSurface(myGameManager.fonts[0], selected_weapon->name, myGameManager.windowH / 2 - 50, black);

	//画面中央: 武器画像を描画
	

	//画面上部: 操作説明を描画
	UI_updateTextCenteredOnSurface(myGameManager.fonts[2], "左右キーで武器を選択", 60, black);

	
	//画面右下: 「通信待機中．．．」を表示
	if (myGameManager.fonts[2]) { 
		SDL_Surface *textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[2], "通信待機中．．．", black);
		if (textSurface) {
			//右下から少し内側に配置
			SDL_Rect dst = { myGameManager.windowW - textSurface->w - 130, myGameManager.windowH - textSurface->h - 20, textSurface->w, textSurface->h };
			SDL_BlitSurface(textSurface, NULL, myGameManager.UI, &dst);
			SDL_FreeSurface(textSurface);
		}
	}
}

/**
 * @brief メインゲーム画面のUIを描画する 
 */
void UI_updateMainSurface(MainScene *scene)
{
	if (myGameManager.UI == NULL || scene == NULL || scene->myCar == NULL) return;

	//UIサーフェイスを透明でクリア
	SDL_FillRect(myGameManager.UI, NULL, SDL_MapRGBA(myGameManager.UI->format, 0, 0, 0, 0));
	SDL_Color black = {0, 0, 0, 255};

	/* 画面右下: HPバー */
	Car *myCar = scene->myCar;
	float hp_percentage = myCar->hp / 100.0f; 
	if (hp_percentage < 0) hp_percentage = 0;
	if (hp_percentage > 1) hp_percentage = 1;

	int bar_width = 250;
	int bar_height = 25;
	int bar_x = myGameManager.windowW - bar_width - 20;
	int bar_y = myGameManager.windowH - bar_height - 60; 

	//HPバーの背景(灰色)
	SDL_Rect hp_bar_bg = { bar_x, bar_y, bar_width, bar_height };
	SDL_FillRect(myGameManager.UI, &hp_bar_bg, SDL_MapRGB(myGameManager.UI->format, 128, 128, 128));

	//HPバーの前景(HP残量に応じて色が変わる)
	Uint32 hp_bar_color;
	if (hp_percentage <= 0.25f) {
		hp_bar_color = SDL_MapRGB(myGameManager.UI->format, 255, 50, 50); //赤
	} else if (hp_percentage <= 0.5f) {
		hp_bar_color = SDL_MapRGB(myGameManager.UI->format, 255, 255, 50); //黄
	} else {
		hp_bar_color = SDL_MapRGB(myGameManager.UI->format, 50, 255, 50); //緑
	}

	SDL_Rect hp_bar_fg = { bar_x, bar_y, (int)(bar_width * hp_percentage), bar_height };
	SDL_FillRect(myGameManager.UI, &hp_bar_fg, hp_bar_color);

	//HP 文字出力
	if (myGameManager.fonts[2]) {
		SDL_Surface *textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[2], "HP", black);
		
		if (textSurface) {
			//HPバーの左側に配置
			SDL_Rect dst = { bar_x - textSurface->w - 10, bar_y + (bar_height - textSurface->h) / 2, 0, 0 };
			SDL_BlitSurface(textSurface, NULL, myGameManager.UI, &dst);
			SDL_FreeSurface(textSurface);
		}
	}

	/*画面右下(HPバーの上): 弾数表示*/
	static SDL_Surface* bulletSurface = NULL;
	if (bulletSurface == NULL) {
		bulletSurface = IMG_Load("assets/images/bullet.png"); 
		if (bulletSurface == NULL) {
			fprintf(stderr, "Failed to load image: assets/pictures/bullet.jpg. Error: %s\n", IMG_GetError());
		}
	}
	
	Gun *myGun = myCar->gun;
	if (myGun) {
		if (myGun->reloadFrameNow > 0) {
			// リロード中はテキストを表示
			if (myGameManager.fonts[2]) {
				SDL_Surface *textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[2], "Reloading...", black);
				if (textSurface) {
					// HPバーの上に表示
					SDL_Rect dst = {bar_x, bar_y - textSurface->h - 10, textSurface->w, textSurface->h };
					SDL_BlitSurface(textSurface, NULL, myGameManager.UI, &dst);
					SDL_FreeSurface(textSurface);
				}
			}
		} 
		if (bulletSurface) {
			SDL_Rect dst;
			dst.w = bulletSurface->w;
			dst.h = bulletSurface->h; 
			dst.x = (myGameManager.windowW - dst.w) / 2; 
			dst.y = (myGameManager.windowH - dst.h) / 2;
			SDL_BlitSurface(bulletSurface, NULL, myGameManager.UI, &dst);
		}
	}

	/* 画面左下: 順位表示 */
	char rank_text[8];
	sprintf(rank_text, "%d", myCar->place);

	if (myGameManager.fonts[0]) {
		SDL_Color white = {255, 255, 255, 255};

		// 白い文字のSurfaceを作成
		SDL_Surface* textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[0], rank_text, white);
		if (textSurface) {
            float scale = 3.0f; 
			SDL_Rect dst;
            dst.w = (int)(textSurface->w * scale);
            dst.h = (int)(textSurface->h * scale);
			dst.x = 20; 
			dst.y = myGameManager.windowH - dst.h - 20; 

			SDL_BlitScaled(textSurface, NULL, myGameManager.UI, &dst);
			SDL_FreeSurface(textSurface);
		}
	}
	
}

/**
 * @brief 右下に弾の画像を描画する
 * @param gun プレイヤーの銃
 */
void UI_drawBullets(Gun *gun)
{
	if (gun == NULL || myGameManager.UI == NULL) return;

	// HPバーの位置情報を参考にする
	int bar_width = 250;
	int bar_height = 25;
	int bar_x = myGameManager.windowW - bar_width - 20;
	int bar_y = myGameManager.windowH - bar_height - 60;

	if (gun->reloadFrameNow > 0) {
		// リロード中はテキストを表示
		if (myGameManager.fonts[2]) {
			SDL_Color black = {0, 0, 0, 255};
			SDL_Surface *textSurface = TTF_RenderUTF8_Blended(myGameManager.fonts[2], "Reloading...", black);
			if (textSurface) {
				// HPバーの上に表示
				SDL_Rect dst = {bar_x, bar_y - textSurface->h - 10, textSurface->w, textSurface->h };
				SDL_BlitSurface(textSurface, NULL, myGameManager.UI, &dst);
				SDL_FreeSurface(textSurface);
			}
		}
		return; // リロード中は弾画像を表示しない
	}

	static SDL_Surface* bulletSurface = NULL;
	if (bulletSurface == NULL) {
		bulletSurface = IMG_Load("assets/pictures/bullet.png"); 
		if (bulletSurface == NULL) {
			fprintf(stderr, "Failed to load image: assets/pictures/bullet.png. Error: %s\n", IMG_GetError());
			return;
		}
	}

	int bullet_w = bulletSurface->w / 3; 
	int bullet_h = bulletSurface->h / 3;
	int padding = 2;

	// 弾の描画開始Y座標（HPバーの少し上）
	int draw_y = bar_y - bullet_h - 10;
	// 弾の描画ブロックの右端をHPバーの右端に合わせる
	int total_width = gun->bulletNum * (bullet_w + padding) - padding;
	if (gun->bulletNum == 0) total_width = 0;
	
	int start_x = (bar_x + bar_width) - total_width;

	for (int i = 0; i < gun->bulletNum; i++) {
		SDL_Rect dst = { start_x + i * (bullet_w + padding), draw_y, bullet_w, bullet_h };
		SDL_BlitScaled(bulletSurface, NULL, myGameManager.UI, &dst);
	}
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
    addListNode(myGameManager.UIList ,rtn , NULL);
    
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
