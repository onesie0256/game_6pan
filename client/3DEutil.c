#include "client.h"

void drawUI(void);
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
    myGameManager.window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED , SDL_WINDOWPOS_CENTERED , WINDOW_WIDTH , WINDOW_HIGHT , SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (myGameManager.window == NULL)
    {
        return 0;
    }

    /*
    myGameManager.renderer = SDL_CreateRenderer(myGameManager.window , -1 , SDL_RENDERER_ACCELERATED);
    if (myGameManager.renderer == NULL)
    {
        return 0;
    }
    */

    //OpenGL設定
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);        //ダブルバッファ有効化
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);         //デプスバッファを24に


    myGameManager.context = SDL_GL_CreateContext(myGameManager.window);

    if (myGameManager.context == NULL)
    {
        SDL_DestroyWindow(myGameManager.window);
        myGameManager.window = NULL;
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
 * @param camera カメラ(カメラを使わなければNULLを入力)
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

    Camera camera_;

    if (camera == NULL){
        camera_.orientation = (Quaternion){0.0f,0.0f,0.0f,0.0f};
        camera_.pos = (Vec3f){0.0f,0.0f,0.0f};
        camera = &camera_;
    }
    // クォータニオンから前方ベクトルと上方ベクトルを計算してgluLookAtを設定
    Vec3f forward_vec = quaternion_rotate_vector((Vec3f){0.0f, 0.0f, -1.0f}, camera->orientation);
    Vec3f up_vec = quaternion_rotate_vector((Vec3f){0.0f, 1.0f, 0.0f}, camera->orientation);
    Vec3f center_pos = vecAdd(camera->pos, forward_vec);

    //gluLookAt(camera->pos.x, camera->pos.y, camera->pos.z, center_pos.x, center_pos.y, center_pos.z, up_vec.x, up_vec.y, up_vec.z);
    gluLookAt(camera->pos.x, camera->pos.y, camera->pos.z, camera->center.x, camera->center.y, camera->center.z, 0.0, 1.0, 0.0);

    if (myGameManager.sceneID == Scene_Main){
        MainScene *scene = (MainScene *)myGameManager.scene;
        displayPolygons(scene->polygonList);
        displayCars(scene->cars);
        displayAmmos();
        //displayPolygons(scene->course->checkPointPlaneList);
    }


    drawUI();
    
    glFlush();
    SDL_GL_SwapWindow(myGameManager.window);
    return 1;
}

/**
 * @brief myGameManager.UIをUIとして画面に貼り付ける
 */
void drawUI(void)
{
    if (!myGameManager.UI) return;

    // --- 3D描画の状態を完全に保存 ---
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // --- 2D描画用に状態を設定 ---
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // --- 2D描画用の行列を設定 ---
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, myGameManager.windowW, myGameManager.windowH, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //openGL用のテクスチャを生成する
    GLuint textureID;
    SDL_Surface *data = myGameManager.UI;

    SDL_Surface *convertedSurface = SDL_ConvertSurfaceFormat(data, SDL_PIXELFORMAT_RGBA32, 0);
    if (!convertedSurface) {
        printf("SDL_ConvertSurfaceFormat error: %s\n", SDL_GetError());
        return;
    }





    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //GLenum format = (data->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, convertedSurface->w, convertedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, convertedSurface->pixels);

    SDL_FreeSurface(convertedSurface);

    float x = 0;
    float y = 0;
    float w = myGameManager.windowW;
    float h = myGameManager.windowH;

    glBegin(GL_QUADS);
        // テクスチャ座標のYを修正して、上下反転を直す
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y + h);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + h);
    glEnd();

    // 3. 作成したテクスチャを削除
    glDeleteTextures(1, &textureID);
    
    

    // --- 3D描画の状態を完全に復元 ---
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}


/**
 * @brief カメラを更新する
 * 
 * @param car 自分の操作する車
 * @param camera カメラ
 */
void updateCamera(Car *car , Camera *camera)
{
    Rectangler *r = car->collisionBox->data.rectangler;

    camera->center = car->center;
    camera->pos = vecAdd(car->center , vecAdd((Vec3f){0.0f,0.6f,0.0f} , vecMulSca(car->direction , -1.5f)));
}
