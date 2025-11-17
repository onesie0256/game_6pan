#include "client.h"

GameManager myGameManager;

SDL_bool init(void);

int main(int argc , char* argv[])
{

  //引数チェック
    if(argc == 1){
        char* str_localhost = "localhost";
    	myGameManager.serverName = str_localhost;
    }
    else if(argc == 2){
    	myGameManager.serverName = argv[1];
    }
    else{
		fprintf(stderr, "Usage: %s, Cannot find a Server Name.\n", argv[0]);
		return -1;
    }

  if(!init()) return 1;

  //サーバーへ接続
  /*
  if(connectServer(myGameManager.serverName) == 0){
    printf("cannot connect server\n");
  }
  */
  #ifdef DEGUG_3DE
  myGameManager.sceneID = Scene_Main;
  #endif

  //ゲームループ
    int endFlag = 1;
    while(endFlag){
      endFlag = gameLoop();
    }
  

  printf("good job\n");
  closeWindow();

  SDL_Quit();
  TTF_Quit();
  IMG_Quit();

    return 0;
}


/**
 * @brief 初期化
 * @return 初期化に成功したらSDL_TRUE,失敗したらSDL_FALSE
 */
SDL_bool init(void)
{
    //初期化
    myGameManager.UI = NULL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        printf("SDL_init error %s\n" , SDL_GetError());
        return SDL_FALSE;
    }

    if(TTF_Init() == -1) {
        printf("TTF_Init error: %s\n", TTF_GetError());
        SDL_Quit();
        return SDL_FALSE;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) != (IMG_INIT_JPG | IMG_INIT_PNG)){
        printf("IMG_Init error %s\n" , IMG_GetError());
        TTF_Quit();
        SDL_Quit();
        return SDL_FALSE;
    }

    #ifdef USE_JOY
    
    if (joycon_open(&jc , JOYCON_R) != 0){
        printf("failed to open Joy-Con");
    }
    #endif

    myGameManager.windowW = WINDOW_WIDTH;
    myGameManager.windowH = WINDOW_HIGHT;

    //ウィンドウ作成
    //myGameManager.window = SDL_CreateWindow("3D_battle_kart" ,  SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED , myGameManager.windowW , myGameManager.windowH , SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    initWindow("3D_battle_kart");

    //エラー処理
    if (!myGameManager.window){
        printf("CreateWindow error %s\n" , SDL_GetError());
        SDL_Quit();
        TTF_Quit();
        IMG_Quit();
        return SDL_FALSE;
    }
    /*
    //レンダラ作成
    myGameManager.renderer = SDL_CreateRenderer(myGameManager.window  , -1 , SDL_RENDERER_ACCELERATED);
    //エラー処理
    if (!myGameManager.renderer){
        printf("CreateRenderer error %s\n" , SDL_GetError());
        SDL_DestroyWindow(myGameManager.window);
        TTF_Quit();
        SDL_Quit();
        IMG_Quit();
        return SDL_FALSE;
    }
    */
   myGameManager.UI = SDL_CreateRGBSurface(0 , myGameManager.windowW , myGameManager.windowH , 32 , 0x00ff0000 , 0x0000ff00 , 0x000000ff , 0xff000000);
   if (!myGameManager.UI){
        printf("CreateRGBSurface error %s\n" , SDL_GetError());
        SDL_DestroyWindow(myGameManager.window);
        TTF_Quit();
        SDL_Quit();
        IMG_Quit();
        return SDL_FALSE;
    }

    
}