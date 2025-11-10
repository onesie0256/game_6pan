#include "3DEngine.h"

int initWindow(char *title);
void closeWindow(void);
int draw(Camera *camera);

/**
 * @brief ウィンドウを初期化する
 * @param title ゲームタイトル
 * @return ウィンドウ生成に成功したら1,失敗したら0を返す
 */
int initWindow(char *title)
{
    myGameManager.window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED , 1920 , 1080 , SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (myGameManager.window == NULL)
    {
        return 0;
    }

    myGameManager.renderer = SDL_CreateRenderer(myGameManager.window , -1 , SDL_RENDERER_ACCELERATED);
    if (myGameManager.renderer == NULL)
    {
        return 0;
    }

    //OpenGL設定
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);        //ダブルバッファ有効化
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);         //デプスバッファを24に


    myGameManager.context = SDL_GL_CreateContext(myGameManager.window);

    if (myGameManager.context == NULL)
    {
        return 0;
    }

    GLfloat light_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_position[] = {1.0f, 0.75f, -0.3f, 0.0f};

    //OpenGL初期化
    glEnable(GL_DEPTH_TEST);    //デプスバッファ使用
    glEnable(GL_CULL_FACE);     //見えない面を描画しない
    glCullFace(GL_BACK);        //背面を描画しない
    glLightfv(GL_LIGHT0 , GL_AMBIENT , light_ambient);
    glLightfv(GL_LIGHT0 , GL_DIFFUSE , light_diffuse);
    glLightfv(GL_LIGHT0 , GL_SPECULAR , light_specular);
    glLightfv(GL_LIGHT0 , GL_POSITION , light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColorMaterial(GL_FRONT , GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    return 1;
}

/**
 * @brief ウィンドウを閉じる
 */
void closeWindow(void)
{
    SDL_DestroyWindow(myGameManager.window);
    SDL_GL_DeleteContext(myGameManager.context);
    SDL_DestroyRenderer(myGameManager.renderer);
}

/**
 * @brief 画面を描画する
 * 
 * @param camera カメラ
 * 
 */
int draw(Camera *camera)
{
    //画面全体を表示
    glViewport(0, 0, myGameManager.windowW, myGameManager.windowH);

    //背景を白色に
    glClearColor(0.902f , 0.902f , 0.980f , 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //投影行列:視野角の設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //視野角60°
    gluPerspective(60.0f, (float)myGameManager.windowW / (float)myGameManager.windowH, 0.1f, 100.0f);

    //モデルビュー行列:カメラの視点設定とオブジェクトの描画
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Vec3f forward_vec = quaternion_rotate_vector((Vec3f){0.0f, 0.0f, -1.0f}, camera->orientation);
    Vec3f up_vec = quaternion_rotate_vector((Vec3f){0.0f, 1.0f, 0.0f}, camera->orientation);
    Vec3f center_pos = vecAdd(camera->pos, forward_vec);

    gluLookAt(camera->pos.x, camera->pos.y, camera->pos.z, center_pos.x, center_pos.y, center_pos.z, up_vec.x, up_vec.y, up_vec.z);

    return 1;
}