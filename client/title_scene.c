#include "client.h"

/**
 * @name titleScene
 * @brief タイトルを表示する
 * @return ゲームが終了するなら0,そうでなければ1を返す
 */
int titleScene(void)
{

    
    // fontを開く
    List *partsUI = createList();
    myGameManager.fonts[0] = TTF_OpenFont("./../assets/font/azuki.ttf" , 200);
    myGameManager.fonts[1] = TTF_OpenFont("./../assets/font/HigureGothic-Regular.ttf" , 100);
    myGameManager.fonts[2] = TTF_OpenFont("./../assets/font/HigureGothic-Regular.ttf" , 60);

    // フォント読み込み失敗時の処理
    if (myGameManager.fonts[0] == NULL || myGameManager.fonts[1] == NULL || myGameManager.fonts[2] == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        SDL_GL_DeleteContext(myGameManager.context);
        SDL_DestroyRenderer(myGameManager.renderer);
        SDL_DestroyWindow(myGameManager.window);
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return SDL_FALSE;
    }

    SDL_Color black = {0, 0, 0, 255};
    SDL_Color white = {255, 255, 255, 255};

    // 背景画像を読み込み
    createUIwithIMG(Scene_Title, "./../assets/image/title_bg.png",
                                      (SDL_Rect){0, 0, myGameManager.windowW, myGameManager.windowH});

    // タイトル文字
    createUIwithText(Scene_Title, "3D Battle Gun Kart",
                                              (SDL_Rect){myGameManager.windowW / 8,
                                                         myGameManager.windowH / 6,
                                                         myGameManager.windowW / 8 * 6,
                                                         myGameManager.windowH / 3},
                                              0, black);

    // "Press A Key" 文字
    createUIwithText(Scene_Title, "Press A Key to Start",
                                              (SDL_Rect){myGameManager.windowW / 4,
                                                         myGameManager.windowH / 3 * 2,
                                                         myGameManager.windowW / 2,
                                                         80},
                                              2, white);

    myGameManager.UI = SDL_CreateRGBSurface(0, myGameManager.windowW, myGameManager.windowH, 32,
                                            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!myGameManager.UI) {
        printf("Failed to create main UI surface: %s\n", SDL_GetError());
        return SDL_FALSE;
    }

    // 背景を描画
    if (bg)
        SDL_BlitScaled(bg, NULL, myGameManager.UI, NULL);

    // タイトル文字を描画
    if (titleText)
        SDL_BlitSurface(titleText, NULL, myGameManager.UI, &(SDL_Rect){myGameManager.windowW / 8,
                                                                       myGameManager.windowH / 6,
                                                                       0, 0});

    // "Press Any Key" テキストを描画
    if (pressText)
        SDL_BlitSurface(pressText, NULL, myGameManager.UI, &(SDL_Rect){myGameManager.windowW / 4,
                                                                       myGameManager.windowH / 3 * 2,
                                                                       0, 0});

    return 1;
}