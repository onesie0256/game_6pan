#include "client.h"

/**
 * @brief イベント処理を行う.マルチスレッド化
 * 
 *
 */

 int net_func(void *args);

void createThread(void)
{
    //スレッド作成
    myGameManager.net_thread = SDL_CreateThread(net_func, "net_thread", NULL);

    if (!myGameManager.net_thread){
        printf("SDL_CreateThread error (net_thread)%s\n" , SDL_GetError());

        SDL_DestroyWindow(myGameManager.window);
        SDL_GL_DeleteContext(myGameManager.context);
        SDL_Quit();
        return ;
    }

    myGameManager.key_thread = SDL_CreateThread(key_func, "key_thread", NULL); 

    //エラー処理
    if (!(myGameManager.key_thread || myGameManager.joy_thread)){
        printf("SDL_CreateThread error (SDL_event)%s\n" , SDL_GetError());

        SDL_DestroyWindow(myGameManager.window);
        SDL_GL_DeleteContext(myGameManager.context);
        SDL_Quit();
        return ;
    }

    #ifdef USE_JOY

    //スレッド作成
    myGameManager.joy_thread = SDL_CreateThread(joy_func, "joy_thread", NULL); // joy_threadを作成し、スレッド関数joy_funcを実行（引数なし）
    
    //エラー処理
    if (!myGameManager.joy_thread){
        printf("SDL_CreateThread error (joycon)%s\n" , SDL_GetError());

        SDL_WaitThread(myGameManager.joy_thread , NULL);
        SDL_DestroyWindow(myGameManager.window);
        SDL_GL_DeleteContext(myGameManager.context);
        SDL_Quit();

        return ;
    }

    #endif    


}

/**
 * @brief イベント処理を行う.マルチスレッド化
 * 
 * @param arg この関数を終了するかどうかのフラグへのポインタ
 */
int key_func(void *args)
{
    static int strPointer = 0;
    while (!myGameManager.quitRequest)
    {
        if (SDL_PollEvent(&myGameManager.event))
        {
            switch (myGameManager.event.type)
            {
            case SDL_QUIT: //終了処理へ
                //myGameManager.quitRequest = SDL_TRUE;
                send_Quit();
                break;

            case SDL_WINDOWEVENT: //ウィンドウのサイズ変更
                if (myGameManager.event.window.event == SDL_WINDOWEVENT_RESIZED){
                    myGameManager.windowW = myGameManager.event.window.data1;
                    myGameManager.windowH = myGameManager.event.window.data2;
                }
                break;

            case SDL_KEYDOWN: //キーが押された
                switch (myGameManager.event.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    myGameManager.keyNow[K_RIGHT] = SDL_TRUE;
                    break;

                case SDLK_LEFT:
                    myGameManager.keyNow[K_LEFT] = SDL_TRUE;
                    break;
                
                case SDLK_UP:
                    myGameManager.keyNow[K_UP] = SDL_TRUE;
                    break;
                
                case SDLK_DOWN:
                    myGameManager.keyNow[K_DOWN] = SDL_TRUE;
                    break;

                case SDLK_SPACE:
                    myGameManager.keyNow[K_SPACE] = SDL_TRUE;
                    break;

                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    myGameManager.keyNow[K_SHIFT] = SDL_TRUE;
                    break;

                case SDLK_RETURN:
                    myGameManager.keyNow[K_ENTER] = SDL_TRUE;
                    break;

                case SDLK_ESCAPE:
                    myGameManager.keyNow[K_ESCAPE] = SDL_TRUE;
                    break;

                case SDLK_BACKSPACE:
                    if (myGameManager.sceneID == Scene_Title){
                        myGameManager.myName[strPointer] = '\0';
                        if (strPointer != 0){
                            strPointer--;
                        }
                    }
                    break;

                }
                break;

            case SDL_KEYUP:
                switch (myGameManager.event.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    myGameManager.keyNow[K_RIGHT] = SDL_FALSE;
                    break;

                case SDLK_LEFT:
                    myGameManager.keyNow[K_LEFT] = SDL_FALSE;
                    break;
                
                case SDLK_UP:
                    myGameManager.keyNow[K_UP] = SDL_FALSE;
                    break;
                
                case SDLK_DOWN:
                    myGameManager.keyNow[K_DOWN] = SDL_FALSE;
                    break;

                case SDLK_SPACE:
                    myGameManager.keyNow[K_SPACE] = SDL_FALSE;
                    break;

                case SDLK_LSHIFT:
                case SDLK_RSHIFT:
                    myGameManager.keyNow[K_SHIFT] = SDL_FALSE;
                    break;

                case SDLK_RETURN:
                    myGameManager.keyNow[K_ENTER] = SDL_FALSE;
                    break;

                case SDLK_ESCAPE:
                    myGameManager.keyNow[K_ESCAPE] = SDL_FALSE;
                    break;
                    
                }
                break;

            case SDL_TEXTINPUT:
                if (strlen(myGameManager.myName)+strlen(myGameManager.event.text.text) <= MYNAME_MAX)
                {
                    strcat(myGameManager.myName , myGameManager.event.text.text);
                    strPointer = strlen(myGameManager.myName) - 1;
                }
                break;
            /*
            case SDL_MOUSEMOTION:
                camera.pitch  += event.motion.yrel * CAMERA_SENSITIVITY;
                if (camera.pitch > 80.0f) camera.pitch = 80.0f;
                if (camera.pitch < -80.0f) camera.pitch = -80.0f;

                camera.yaw -= event.motion.xrel * CAMERA_SENSITIVITY;
                break;
            */
            }
            
        }

        
    }
    
    return 0;
}

int net_func(void *args)
{
    while (myGameManager.quitRequest != SDL_TRUE)
    {
        // control_requestsが0を返したら、それは終了を意味する
        if (control_requests() == 0) {
            myGameManager.quitRequest = SDL_TRUE;
        }
    }
    return 0;
}
#ifdef USE_JOY
// Joy-Conの状態を監視する関数
int joy_func(void *args)
{

    int running = 1;

    SDL_bool *is_JoyConInputContenue = (SDL_bool *)args;

    while (running)
    {
        if (joycon_get_state(&myGameManager.jc) != 0){
            //printf("failed to update joycon");
            //SDL_Delay(1);
            continue;
        }

        if (myGameManager.jc.button.btn.Home){
            myGameManager.joyBotton[JOY_Home] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_Home] = SDL_FALSE;
        }        

        if (myGameManager.jc.button.btn.Plus){
            myGameManager.joyBotton[JOY_Plus] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_Plus] = SDL_FALSE;
        }

        if (myGameManager.jc.button.btn.Minus){
            myGameManager.joyBotton[JOY_Minus] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_Minus] = SDL_FALSE;
        }

        if (myGameManager.jc.button.btn.ZL){
            myGameManager.joyBotton[JOY_ZL] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_ZL] = SDL_FALSE;
        }

        if (myGameManager.jc.button.btn.ZR){
            myGameManager.joyBotton[JOY_ZR] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_ZR] = SDL_FALSE;
        }

        if (myGameManager.jc.button.btn.A){
            myGameManager.joyBotton[JOY_A] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_A] = SDL_FALSE;
        }

        if (myGameManager.jc.button.btn.B){
            myGameManager.joyBotton[JOY_B] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_B] = SDL_FALSE;
        }    

        if (myGameManager.jc.button.btn.X){
            myGameManager.joyBotton[JOY_X] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_X] = SDL_FALSE;
        }      
        
        if (myGameManager.jc.button.btn.X){
            myGameManager.joyBotton[JOY_Y] = SDL_TRUE;
        }
        else {
            myGameManager.joyBotton[JOY_Y] = SDL_FALSE;
        }  

        //ジョイコンの傾き
        myGameManager.StickX = myGameManager.jc.stick.x;
        myGameManager.StickY = myGameManager.jc.stick.y;

        //SDL_Delay(300);
    }

    return 0;  
    
}

#endif

/**
 * @brief キーの長押しを検知したくない時にお使いください
 * 
 * @param KEY_NAME キーのenum
 */
SDL_bool isKeyDowned(int KEY_NAME)
{
    if (myGameManager.keyNow[KEY_NAME] == SDL_TRUE){
        myGameManager.keyNow[KEY_NAME] = SDL_FALSE;
        return SDL_TRUE;
    }
    else{
        return SDL_FALSE;
    }
}