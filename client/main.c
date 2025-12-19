#include "client.h"

GameManager myGameManager;

SDL_bool init(void);
void loadModels(void);

static uint16_t port = 50100;
static char clientServerName[MAX_LEN_NAME];

int main(int argc , char* argv[])
{
  memset(myGameManager.serverName , 0 , MAX_LEN_NAME);
  
  #ifdef DEGUG_3D
  //引数チェック
    if(argc == 1){
        char* str_localhost = "localhost";
    	  strcpy(myGameManager.serverName , str_localhost);
    }
    else if(argc == 2){
    	strcpy(myGameManager.serverName , argv[1]);
    }
    else{
		fprintf(stderr, "Usage: %s, Cannot find a Server Name.\n", argv[0]);
		return -1;
    }
    #else
    //コマンドライン引数処理
  switch (argc) {
  case 1: //引数なし
  {
    char* str_localhost = "localhost";
    strcpy(myGameManager.serverName , str_localhost);
  }
    break;
  case 2: //サーバ名のみ指定
    strncpy(myGameManager.serverName, argv[1], MAX_LEN_NAME - 1);
    clientServerName[MAX_LEN_NAME - 1] = '\0';
    break;
  case 3: //サーバ名とポート番号指定
    strncpy(myGameManager.serverName, argv[1], MAX_LEN_NAME - 1);
    clientServerName[MAX_LEN_NAME - 1] = '\0';
    port = (uint16_t)atoi(argv[2]);
    break;
  default:
    fprintf(stderr, "Usage: %s [server name] [port number]\n", argv[0]);
    return -1;
  }
  #endif

  if(!init()) return 1; //初期化

  loadModels();

  setup_client(myGameManager.serverName, port);
  printf("connected server %s %d\n" , myGameManager.serverName , port);

  createThread(); //スレッド作成する

  //サーバーへ接続
  

  //ゲームループ
    int endFlag = 1;
    while(endFlag){
      endFlag = gameLoop();
    }
  
  //終了処理
  // まず、ネットワーク処理を行っているスレッドが終了するのを待つ
  SDL_WaitThread(myGameManager.key_thread , NULL); 

  SDL_WaitThread(myGameManager.net_thread , NULL);
  
  #ifdef USE_JOY
  // joy_threadも終了を待つ
  SDL_WaitThread(myGameManager.joy_thread, NULL); 
  #endif

  // スレッドが終了した後で、ネットワーク接続を閉じる
  terminate_client();

  printf("good job\n");
  closeWindow();

  #ifdef USE_JOY
  joycon_close(&jc);
  #endif

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

    myGameManager.quitRequest = SDL_FALSE;
    myGameManager.ackRequest = 0;


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
    
    if (joycon_open(&myGameManager.jc , JOYCON_R) != 0){
        printf("failed to open Joy-Con");
        SDL_DestroyWindow(window); // ウィンドウを破棄
        SDL_Quit(); // SDLを終了
        return 1; // プログラム終了
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

    myGameManager.quitRequest = SDL_FALSE;


  return SDL_TRUE;
}

void loadModels(void)
{
  createObjEX(&myGameManager.models[Shotgun] , "assets/models/shotgun_o.obj" , "assets/models/shotgun.png" , NULL);
  createObjEX(&myGameManager.models[Sniper] , "assets/models/sniper_o.obj" , "assets/models/sniper.png" , NULL);
  createObjEX(&myGameManager.models[Pistol] , "assets/models/pistol_o.obj" , "assets/models/pistol.png" , NULL);
  printf("pistl:: v:%d , n:%d , t:%d\nva:%d , na:%d , ta:%d\n" , myGameManager.models[Pistol].vertNum , myGameManager.models[Pistol].normalNum , myGameManager.models[Pistol].texCoordNum , myGameManager.models[Pistol].vertAryNum , myGameManager.models[Pistol].normAryNum , myGameManager.models[Pistol].texCoordAryNum);

}