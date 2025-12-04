#include "client.h"

void drawUI(void);
int initWindow(char *title);
void closeWindow(void);
int draw(Camera *camera);
void calcCollisionCarVel(Vec3f *v1 , Vec3f *v2);

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
        displayPolygons(scene->course->checkPointPlaneList);
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

SDL_bool collisionCar(Car *c1 , Car *c2)
{
    Rectangler *r1 = c1->collisionBox->data.rectangler;
    Rectangler *r2 = c2->collisionBox->data.rectangler;

    
    Vec3f v = r2->vertex[0];
    Vec3f n = r2->normals[0];


    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(c1->preCoordOfVertexs[i] , v , n) && !isPointOnPlane(r1->vertex[i] , v , n) && isPointOnPlane4(r1->vertex[i] , r2->vertex[0] , r2->vertex[1] , r2->vertex[2] , r2->vertex[3] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , r1->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(r1 , moveVec , 1.0f);
            calcCollisionCarVel(&c1->velocity , &c2->velocity);
            return SDL_TRUE;
        }
    }

    v = r2->vertex[1];
    n = r2->normals[1];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(c1->preCoordOfVertexs[i] , v , n) && !isPointOnPlane(r1->vertex[i] , v , n) && isPointOnPlane4(r1->vertex[i] , r2->vertex[1] , r2->vertex[4] , r2->vertex[7] , r2->vertex[2] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , r1->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(r1 , moveVec , 1.0f);
            calcCollisionCarVel(&c1->velocity , &c2->velocity);
            return SDL_TRUE;
        }
    }

    v = r2->vertex[4];
    n = r2->normals[2];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(c1->preCoordOfVertexs[i] , v , n) && !isPointOnPlane(r1->vertex[i] , v , n) && isPointOnPlane4(r1->vertex[i] , r2->vertex[4] , r2->vertex[5] , r2->vertex[6] , r2->vertex[7] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , r1->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(r1 , moveVec , 1.0f);
            calcCollisionCarVel(&c1->velocity , &c2->velocity);
            return SDL_TRUE;
        }
    }

    v = r2->vertex[5];
    n = r2->normals[3];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(c1->preCoordOfVertexs[i] , v , n) && !isPointOnPlane(r1->vertex[i] , v , n) && isPointOnPlane4(r1->vertex[i] , r2->vertex[5] , r2->vertex[0] , r2->vertex[3] , r2->vertex[6] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , r1->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(r1 , moveVec , 1.0f);
            calcCollisionCarVel(&c1->velocity , &c2->velocity);
            return SDL_TRUE;
        }
    }
    
    v = r2->vertex[3];
    n = r2->normals[4];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(c1->preCoordOfVertexs[i] , v , n) && !isPointOnPlane(r1->vertex[i] , v , n) && isPointOnPlane4(r1->vertex[i] , r2->vertex[3] , r2->vertex[2] , r2->vertex[7] , r2->vertex[6] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , r1->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(r1 , moveVec , 1.0f);
            calcCollisionCarVel(&c1->velocity , &c2->velocity);
            return SDL_TRUE;
        }
    }
    

    v = r2->vertex[0];
    n = r2->normals[5];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(c1->preCoordOfVertexs[i] , v , n) && !isPointOnPlane(r1->vertex[i] , v , n) && isPointOnPlane4(r1->vertex[i] , r2->vertex[0] , r2->vertex[5] , r2->vertex[4] , r2->vertex[1] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , r1->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(r1 , moveVec , 1.0f);
            calcCollisionCarVel(&c1->velocity , &c2->velocity);
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

void collisionCars(List *carList)
{
    ListNode *i;
    foreach(i,carList){
        ListNode *j;
        foreach_(j , i){
            Car *c1 = ((Car *)i->data);
            Car *c2 = ((Car *)j->data);
            collisionCar(c1 , c2);
            collisionCar(c2 , c1);
        }
        
    }
}

/**
 * @brief 車が別の車に当たった時の跳ね返りの速度を求める   
 * 
 * @param velocity1 車の速度ベクトル1
 * @param velocity2 車の速度ベクトル1
 * 
 * @return なし
 */
void calcCollisionCarVel(Vec3f *v1 , Vec3f *v2)
{
    float e = 0.90;
    float mass = 100.0f;

    Vec3f v1_ , v2_;

    v1_.x = (mass * v1->x + mass * v2->x + v2->x * e * mass - v1->x * e * mass) / (mass * 2);
    v2_.x = -e * (v2->x - v1->x) + v1_.x;

    v1_.y = (mass * v1->y + mass * v2->y + v2->y * e * mass - v1->y * e * mass) / (mass * 2);
    v2_.y = -e * (v2->y - v1->y) + v1_.y;

    v1_.z = (mass * v1->z + mass * v2->z + v2->z * e * mass - v1->z * e * mass) / (mass * 2);
    v2_.z = -e * (v2->z - v1->z) + v1_.z;

    *v1 = v1_;
    *v2 = v2_;

    /*
    ci_v1.x = (ci->mass * ci_v0.x + cj->mass * cj_v0.x + cj_v0.x * e * cj->mass - ci_v0.x * e * cj->mass) / (ci->mass + cj->mass);
        cj_v1.x = -e * (cj_v0.x - ci_v0.x) + ci_v1.x;

    ci_v1.y = (ci->mass * ci_v0.y + cj->mass * cj_v0.y + cj_v0.y * e * cj->mass - ci_v0.y * e * cj->mass) / (ci->mass + cj->mass);
    cj_v1.y = -e * (cj_v0.y - ci_v0.y) + ci_v1.y;
    */
}

void updateCamera(Car *car , Camera *camera)
{
    Rectangler *r = car->collisionBox->data.rectangler;

    camera->center = car->center;
    camera->pos = vecAdd(car->center , vecAdd((Vec3f){0.0f,2.0f,0.0f} , vecMulSca(car->direction , -3.0f)));
}
