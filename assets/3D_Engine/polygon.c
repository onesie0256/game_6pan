#include "3DEngine.h"

/**
 * @brief 直方体の頂点インデックス 未使用
 */
GLubyte rectIdxAry[] = {
    0,1,2,3,
    1,4,7,2,
    4,5,6,7,
    5,0,3,6,
    3,2,7,6,
    0,5,4,1
};


/**
 * @brief math.cのabs()のfloat型版
 */
float abs_f(float num)
{
    if (num < 0) return -num;
    return num;
}


/**
 * @brief 直方体を生成する
 * 
 * @param coord 直方体のxyが最小でzが最大の頂点の座標(サイズの設定によりこの条件の点でなくても構わない)
 * @param size  サイズ(負の数を入力することで直方体の伸びる向きを変えることができる)
 * @param pitch x軸周りの回転[°]
 * @param roll  z軸方向の回転
 * @param yaw   y軸方向の回転
 * 
 * @return 生成した直方体格納されたポリゴン構造体へのポインタ
 */
Polygon* createRectangler(Vec3f coord , Vec3f size , Vec3f color , int pitch , int roll , int yaw , List *list)
{
    Polygon *rtn = (Polygon *)malloc(sizeof(Polygon));
    Rectangler *rectangler = (Rectangler *)malloc(sizeof(Rectangler));


    //色を格納
    rectangler->color = color;

    Vec3f temp[8];

    //サイズの値がマイナスの時に,直方体のxyが最小でzが最大の頂点の座標を変更
    if (size.x < 0.0f){
        coord.x += size.x;
        size.x = -size.x;
    }

    if (size.y < 0.0f){
        coord.y += size.y;
        size.y = -size.y;
    }

    if (size.z < 0.0f){
        coord.z -= size.z;
        size.z = -size.z;
    }

    /*原点周りに回転させ,並行移動する*/
    temp[0] = (Vec3f){0.0f, 0.0f, 0.0f};        //手前左下
    temp[1] = (Vec3f){size.x, 0.0f, 0.0f};      //手前右下
    temp[2] = (Vec3f){size.x, size.y, 0.0f};    //手前右上
    temp[3] = (Vec3f){0.0f, size.y, 0.0f};      //手前左上
    temp[4] = (Vec3f){size.x, 0.0f, -size.z};   //裏右下
    temp[5] = (Vec3f){0.0f, 0.0f, -size.z};     //裏左下
    temp[6] = (Vec3f){0.0f, size.y, -size.z};   //裏左上
    temp[7] = (Vec3f){size.x, size.y, -size.z}; //裏右上

    //点の回転と並行移動
    for (int i = 0 ; i < 8 ; i++){
        temp[i] = rotateXYZ(temp[i] , pitch , roll , yaw);  //ローカル座標的に回転
        rectangler->vertex[i] = vecAdd(temp[i] , coord);    //並行移動して空間上に設置
    }

    rectangler->pitch = pitch;
    rectangler->roll  = roll;
    rectangler->yaw   = yaw;

    //法線ベクトルを計算
    rectangler->normals[0] = calcNormalVec(rectangler->vertex[PP_LDF] , rectangler->vertex[PP_RDF] , rectangler->vertex[PP_LUF]);
    rectangler->normals[1] = calcNormalVec(rectangler->vertex[PP_RDF] , rectangler->vertex[PP_RDB] , rectangler->vertex[PP_RUF]);
    rectangler->normals[2] = calcNormalVec(rectangler->vertex[PP_RDB] , rectangler->vertex[PP_LDB] , rectangler->vertex[PP_RUB]);
    rectangler->normals[3] = calcNormalVec(rectangler->vertex[PP_LDB] , rectangler->vertex[PP_LDF] , rectangler->vertex[PP_LUB]);
    rectangler->normals[4] = calcNormalVec(rectangler->vertex[PP_LUF] , rectangler->vertex[PP_RUF] , rectangler->vertex[PP_LUB]);
    rectangler->normals[5] = calcNormalVec(rectangler->vertex[PP_LDF] , rectangler->vertex[PP_LDB] , rectangler->vertex[PP_RDF]);

    //頂点データ
    //前面
    rectangler->vertAry[0] = rectangler->vertex[PP_LDF].x; rectangler->vertAry[1] = rectangler->vertex[PP_LDF].y; rectangler->vertAry[2] = rectangler->vertex[PP_LDF].z;
    rectangler->vertAry[3] = rectangler->vertex[PP_RDF].x; rectangler->vertAry[4] = rectangler->vertex[PP_RDF].y; rectangler->vertAry[5] = rectangler->vertex[PP_RDF].z;
    rectangler->vertAry[6] = rectangler->vertex[PP_RUF].x; rectangler->vertAry[7] = rectangler->vertex[PP_RUF].y; rectangler->vertAry[8] = rectangler->vertex[PP_RUF].z;
    rectangler->vertAry[9] = rectangler->vertex[PP_LUF].x; rectangler->vertAry[10] = rectangler->vertex[PP_LUF].y; rectangler->vertAry[11] = rectangler->vertex[PP_LUF].z;

    //右面
    rectangler->vertAry[12] = rectangler->vertex[PP_RDF].x; rectangler->vertAry[13] = rectangler->vertex[PP_RDF].y; rectangler->vertAry[14] = rectangler->vertex[PP_RDF].z;
    rectangler->vertAry[15] = rectangler->vertex[PP_RDB].x; rectangler->vertAry[16] = rectangler->vertex[PP_RDB].y; rectangler->vertAry[17] = rectangler->vertex[PP_RDB].z;
    rectangler->vertAry[18] = rectangler->vertex[PP_RUB].x; rectangler->vertAry[19] = rectangler->vertex[PP_RUB].y; rectangler->vertAry[20] = rectangler->vertex[PP_RUB].z;
    rectangler->vertAry[21] = rectangler->vertex[PP_RUF].x; rectangler->vertAry[22] = rectangler->vertex[PP_RUF].y; rectangler->vertAry[23] = rectangler->vertex[PP_RUF].z;
    
    //背面
    rectangler->vertAry[24] = rectangler->vertex[PP_RDB].x; rectangler->vertAry[25] = rectangler->vertex[PP_RDB].y; rectangler->vertAry[26] = rectangler->vertex[PP_RDB].z;
    rectangler->vertAry[27] = rectangler->vertex[PP_LDB].x; rectangler->vertAry[28] = rectangler->vertex[PP_LDB].y; rectangler->vertAry[29] = rectangler->vertex[PP_LDB].z;
    rectangler->vertAry[30] = rectangler->vertex[PP_LUB].x; rectangler->vertAry[31] = rectangler->vertex[PP_LUB].y; rectangler->vertAry[32] = rectangler->vertex[PP_LUB].z;
    rectangler->vertAry[33] = rectangler->vertex[PP_RUB].x; rectangler->vertAry[34] = rectangler->vertex[PP_RUB].y; rectangler->vertAry[35] = rectangler->vertex[PP_RUB].z;

    //左面
    rectangler->vertAry[36] = rectangler->vertex[PP_LDB].x; rectangler->vertAry[37] = rectangler->vertex[PP_LDB].y; rectangler->vertAry[38] = rectangler->vertex[PP_LDB].z;
    rectangler->vertAry[39] = rectangler->vertex[PP_LDF].x; rectangler->vertAry[40] = rectangler->vertex[PP_LDF].y; rectangler->vertAry[41] = rectangler->vertex[PP_LDF].z;
    rectangler->vertAry[42] = rectangler->vertex[PP_LUF].x; rectangler->vertAry[43] = rectangler->vertex[PP_LUF].y; rectangler->vertAry[44] = rectangler->vertex[PP_LUF].z;
    rectangler->vertAry[45] = rectangler->vertex[PP_LUB].x; rectangler->vertAry[46] = rectangler->vertex[PP_LUB].y; rectangler->vertAry[47] = rectangler->vertex[PP_LUB].z;

    //上面
    rectangler->vertAry[48] = rectangler->vertex[PP_LUF].x; rectangler->vertAry[49] = rectangler->vertex[PP_LUF].y; rectangler->vertAry[50] = rectangler->vertex[PP_LUF].z;
    rectangler->vertAry[51] = rectangler->vertex[PP_RUF].x; rectangler->vertAry[52] = rectangler->vertex[PP_RUF].y; rectangler->vertAry[53] = rectangler->vertex[PP_RUF].z;
    rectangler->vertAry[54] = rectangler->vertex[PP_RUB].x; rectangler->vertAry[55] = rectangler->vertex[PP_RUB].y; rectangler->vertAry[56] = rectangler->vertex[PP_RUB].z;
    rectangler->vertAry[57] = rectangler->vertex[PP_LUB].x; rectangler->vertAry[58] = rectangler->vertex[PP_LUB].y; rectangler->vertAry[59] = rectangler->vertex[PP_LUB].z;

    //下面
    rectangler->vertAry[60] = rectangler->vertex[PP_LDF].x; rectangler->vertAry[61] = rectangler->vertex[PP_LDF].y; rectangler->vertAry[62] = rectangler->vertex[PP_LDF].z;
    rectangler->vertAry[63] = rectangler->vertex[PP_LDB].x; rectangler->vertAry[64] = rectangler->vertex[PP_LDB].y; rectangler->vertAry[65] = rectangler->vertex[PP_LDB].z;
    rectangler->vertAry[66] = rectangler->vertex[PP_RDB].x; rectangler->vertAry[67] = rectangler->vertex[PP_RDB].y; rectangler->vertAry[68] = rectangler->vertex[PP_RDB].z;
    rectangler->vertAry[69] = rectangler->vertex[PP_RDF].x; rectangler->vertAry[70] = rectangler->vertex[PP_RDF].y; rectangler->vertAry[71] = rectangler->vertex[PP_RDF].z;

    //法線データ
    for (int i = 0 ; i < 24 ; i++){
        rectangler->normAry[i*3 + 0] = rectangler->normals[i/4].x;
        rectangler->normAry[i*3 + 1] = rectangler->normals[i/4].y;
        rectangler->normAry[i*3 + 2] = rectangler->normals[i/4].z;
    }

    //カラーデータ
    for (int i = 0 ; i < 24 ; i++){
        rectangler->colorAry[i*3 + 0] = rectangler->color.x;
        rectangler->colorAry[i*3 + 1] = rectangler->color.y;
        rectangler->colorAry[i*3 + 2] = rectangler->color.z;
    }

    // 値の設定
    rtn->isDisplay       = SDL_TRUE;
    rtn->type            = PT_RECTANGLER;
    rtn->data.rectangler = rectangler;

    //双方向リストに格納
    if (list != NULL)
    addListNode(list , rtn , NULL);

    return rtn;
}

/**
 * @brief 球体を1つ生成する
 */
Polygon* createSphere(Vec3f center , float radius , Vec3f color , List *list)
{
    Polygon *rtn = (Polygon *)malloc(sizeof(Polygon));
    Sphere *sphere = (Sphere *)malloc(sizeof(Sphere));

    //データを格納
    sphere->center = center;
    sphere->radius = radius;
    sphere->color  = color;
    sphere->qobj   = gluNewQuadric(); //四角形オブジェクトを作成
    gluQuadricNormals(sphere->qobj , GLU_FLAT); //球体をポリゴンチック
    
    rtn->isDisplay   = SDL_TRUE;
    rtn->type        = PT_SPHERE;
    rtn->data.sphere = sphere;

    //双方向リストに格納
    addListNode(list , rtn, NULL);

    return rtn;
}

/**
 * @brief 平面を1つ生成する
 * 
 * @param coord 
 */
Polygon* createPlane4(Vec3f coord , float sizeX , float sizeY , Vec3f color , int pitch , int roll , int yaw , PlaneType type , List *list)
{
    Polygon *rtn = (Polygon *)malloc(sizeof(Polygon));
    Plane4 *plane4 = (Plane4 *)malloc(sizeof(Plane4));

    //サイズの値がマイナスの時に,直方体のxyが最小でzが最大の頂点の座標を変更
    if (sizeX < 0.0f){
        coord.x += sizeX;
        sizeX = -sizeX;
    }

    if (sizeY < 0.0f){
        coord.y += sizeY;
        sizeY = -sizeY;
    }

    Vec3f temp[4];
    temp[0] = (Vec3f){0.0f , 0.0f , 0.0f};
    temp[1] = (Vec3f){sizeX , 0.0f , 0.0f};
    temp[2] = (Vec3f){sizeX , sizeY , 0.0f};
    temp[3] = (Vec3f){0.0f , sizeY , 0.0f};

    for (int i = 0 ; i < 4 ; i++){
        temp[i] = rotateXYZ(temp[i] , pitch , roll , yaw);
        plane4->vertex[i] = vecAdd(temp[i] , coord);
    }

    plane4->normal = calcNormalVec(plane4->vertex[0] , plane4->vertex[1] , plane4->vertex[3]);
    plane4->color  = color;
    plane4->type   = type;

    plane4->pitch = pitch;
    plane4->roll  = roll;
    plane4->yaw   = yaw;
    plane4->isBallOnPlane = SDL_TRUE;

    //頂点データ
    //前面
    plane4->vertAry[0] = plane4->vertex[0].x; plane4->vertAry[1] = plane4->vertex[0].y; plane4->vertAry[2] = plane4->vertex[0].z;
    plane4->vertAry[3] = plane4->vertex[1].x; plane4->vertAry[4] = plane4->vertex[1].y; plane4->vertAry[5] = plane4->vertex[1].z;
    plane4->vertAry[6] = plane4->vertex[2].x; plane4->vertAry[7] = plane4->vertex[2].y; plane4->vertAry[8] = plane4->vertex[2].z;
    plane4->vertAry[9] = plane4->vertex[3].x; plane4->vertAry[10] = plane4->vertex[3].y; plane4->vertAry[11] = plane4->vertex[3].z;

    //法線データ
    for (int i = 0 ; i < 4 ; i++){
        plane4->normAry[i*3 + 0] = plane4->normal.x;
        plane4->normAry[i*3 + 1] = plane4->normal.y;
        plane4->normAry[i*3 + 2] = plane4->normal.z;
    }

    //カラーデータ
    for (int i = 0 ; i < 4 ; i++){
        plane4->colorAry[i*3 + 0] = plane4->color.x;
        plane4->colorAry[i*3 + 1] = plane4->color.y;
        plane4->colorAry[i*3 + 2] = plane4->color.z;
    }

    rtn->data.plane4 = plane4;
    rtn->isDisplay   = SDL_TRUE;
    rtn->type        = PT_PLANE4;

    //双方向リストに格納
    addListNode(list , rtn, NULL);

    return rtn;
}

/**
 * @brief 円盤を1つ生成する
 */
Polygon* createDisc(Vec3f center , float radius , Vec3f color , List *list)
{
    Polygon *rtn = (Polygon *)malloc(sizeof(Polygon));
    Disc *disc = (Disc *)malloc(sizeof(Disc));
    disc->center = center;
    disc->radius = radius;
    disc->color  = color;

    disc->qobj   = gluNewQuadric(); //四角形オブジェクトを作成
    disc->normal = (Vec3f){0.0f , 1.0f , 0.0f};

    rtn->data.disc = disc;
    rtn->isDisplay   = SDL_TRUE;
    rtn->type        = PT_DISC;

    //双方向リストに格納
    addListNode(list , rtn, NULL);

    return rtn;
}

/**
 * @brief obj
 */
Polygon* createObj(const char* obj_filename , const char* texture_filename , Vec3f coordOffset , Vec3f scale , int pitchOffset , int rollOffset , int yawOffset , List *list)
{
    Polygon *rtn = (Polygon *)malloc(sizeof(Polygon));
    Obj *obj = (Obj *)malloc(sizeof(Obj));
    obj->vertNum = 0;
    obj->vertAryNum = 0;
    obj->texCoordNum = 0;
    obj->texCoordAryNum = 0;
    obj->normalNum = 0;
    obj->normAryNum = 0;

    obj->coord = (Vec3f){0.0f , 0.0f , 0.0f};
    obj->CoordOffSet = coordOffset;
    obj->scale = scale;
    obj->pitch = 0;
    obj->roll = 0;
    obj->yaw = 0;
    obj->pitchOffset = pitchOffset;
    obj->rollOffset = rollOffset;
    obj->yawOffset = yawOffset;

    loadOBJ(obj_filename , texture_filename , obj);

    SDL_Surface *surface = IMG_Load(texture_filename);

    if (!surface) {
        printf("IMG_Load error: %s\n", IMG_GetError());
        free(obj);
        return NULL;
    }

    // OpenGLテクスチャを生成
    glGenTextures(1, &obj->texture);
    glBindTexture(GL_TEXTURE_2D, obj->texture);

    // テクスチャのパラメータ設定 (ラッピング、フィルタリング)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // SDL_Surfaceのピクセルフォーマットを判定
    GLenum format = (surface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

    // SDL_SurfaceのピクセルデータをOpenGLテクスチャに転送
    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    obj->texW = (float)surface->w;
    obj->texH = (float)surface->h;
    //obj->texture = 0;

    SDL_FreeSurface(surface);

    rtn->data.obj    = obj;
    rtn->isDisplay   = SDL_TRUE;
    rtn->type        = PT_OBJ;

    //双方向リストに格納
    addListNode(list , rtn, NULL);

    return rtn;
}

Polygon* createCylinder(List *list)
{
    Polygon *rtn = (Polygon *)malloc(sizeof(Polygon));
    Cylinder *cylinder = (Cylinder *)malloc(sizeof(Cylinder));

    cylinder->center = (Vec3f){0.0f , 0.0f , 0.0f};
    cylinder->radius = 0.02f;
    cylinder->height = 0.5f;
    cylinder->color = (Vec3f){1.0f , 0.0f , 0.0f};
    cylinder->qobj = gluNewQuadric();
    cylinder->yaw = 180;


    rtn->data.cylinder = cylinder;
    rtn->isDisplay = SDL_TRUE;
    rtn->type = PT_CYLINDER;

    addListNode(list , rtn , NULL);

    return rtn;
}

/**
 * @brief 直方体を1つ描画する
 * 
 * @param rectangler 直方体の構造体へのポインタ
 */
void displayRectangler(Rectangler *rectangler)
{

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3 , GL_FLOAT , 0 , rectangler->vertAry);
    glNormalPointer(GL_FLOAT , 0 , rectangler->normAry);
    glColorPointer(3 , GL_FLOAT , 0 , rectangler->colorAry);
    //glDrawElements(GL_QUADS , 24 , GL_UNSIGNED_BYTE , rectIdxAry);
    glDrawArrays(GL_QUADS , 0 , 24);

    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

/**
 * @brief 球体を1つ描画する
 * 
 * @param sphere 球体の構造体へのポインタ
 */
void displaySphere(Sphere *sphere)
{
    glPushMatrix();

    //色を指定する
    glColor3f(sphere->color.x , sphere->color.y , sphere->color.z); 
    //球体を並行移動する
    glTranslatef(sphere->center.x , sphere->center.y , sphere->center.z);
    //球体を描画する.第3,第4引数はそれぞれ経線,緯線の数(数が大きいほど滑らかな球体になる)
    gluSphere(sphere->qobj , sphere->radius , 16 , 16);

    glPopMatrix();
}

/**
 * @brief 平面を描画する
 * 
 * @param plane4 平面の構造体
 */
void displayPlane4(Plane4 *plane4)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3 , GL_FLOAT , 0 , plane4->vertAry);
    glNormalPointer(GL_FLOAT , 0 , plane4->normAry);
    glColorPointer(3 , GL_FLOAT , 0 , plane4->colorAry);
    //glDrawElements(GL_QUADS , 24 , GL_UNSIGNED_BYTE , rectIdxAry);
    glDrawArrays(GL_QUADS , 0 , 4);

    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

/**
 * @brief 円盤を表示
 * 
 * @param disc 円盤の構造体
 */
void displayDisc(Disc *disc)
{
    glPushMatrix();

    //色を指定する
    glColor3f(disc->color.x , disc->color.y , disc->color.z);

    //並行移動→回転移動の順番に行列計算
    //球体を並行移動する
    glTranslatef(disc->center.x , disc->center.y , disc->center.z);
    //x軸周りに90°回転
    glRotatef(-90.0f , 1.0f , 0.0f , 0.0f);
    
    //球体を描画する.
    gluDisk(disc->qobj , 0.0f , disc->radius , 16 , 16);

    glPopMatrix();
}

/**
 * @brief obj表示
 */
void displayObj(Obj *obj)
{
    glPushMatrix();

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //glDisable(GL_COLOR_MATERIAL);

    
    glBindTexture(GL_TEXTURE_2D, obj->texture);
    glEnable(GL_TEXTURE_2D);


    
    
    //glRotatef(-90.0f , 0.0f , 0.0f , 1.0f);
    glTranslatef(obj->CoordOffSet.x + obj->coord.x, obj->CoordOffSet.y + obj->coord.y , obj->CoordOffSet.z + obj->coord.z);
    //printf("%f %f %f\n" , obj->CoordOffSet.x + obj->coord.x, obj->CoordOffSet.y + obj->coord.y , obj->CoordOffSet.z + obj->coord.z);
    //glTranslatef(obj->coord.x , obj->coord.y , obj->coord.z);
    

    //glScalef(50.0f , 50.0f , 50.0f);

    glTranslatef(0.0f , 1.0f , 0.0f);

    glRotatef(obj->pitch , 1.0f , 0.0f , 0.0f);
    glRotatef(obj->roll , 0.0f , 0.0f , 1.0f);
    glRotatef(obj->yaw , 0.0f , 1.0f , 0.0f);


    glTranslatef(0.0f , -1.0f , 0.0f);


    glRotatef(obj->pitchOffset, 1.0f , 0.0f , 0.0f);
    glRotatef(obj->rollOffset, 0.0f , 0.0f , 1.0f);
    glRotatef(obj->yawOffset, 0.0f , 1.0f , 0.0f);

    glScalef(obj->scale.x , obj->scale.y ,obj->scale.z);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3 , GL_FLOAT , 0 , &(obj->vertAry[0]));
    glTexCoordPointer(2 , GL_FLOAT , 0 , &(obj->texCoordAry[0]));
    glNormalPointer(GL_FLOAT , 0 , &(obj->normAry[0]));

    glDrawArrays(GL_QUADS , 0 , obj->vertAryNum / 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    glDisable(GL_TEXTURE_2D);
    // デフォルトのテクスチャ環境モードに戻す
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    //glEnable(GL_COLOR_MATERIAL);
    glPopMatrix();
}

/**
 * @brief 円錐を表示
 */
void displayCylinder(Cylinder *cylinder)
{
    glPushMatrix();
    //色を指定する
    glColor3f(cylinder->color.x , cylinder->color.y , cylinder->color.z);
    //球体を並行移動する
    glTranslatef(cylinder->center.x , cylinder->center.y - 0.5f , cylinder->center.z);
    glRotatef(cylinder->yaw , 0.0f , 1.0f , 0.0f);
    //glRotatef(90.0f , 1.0f , 0.0f , 0.0f);
    glTranslatef(0.0f , 0.5f ,0.0f);
    gluCylinder(cylinder->qobj , cylinder->radius , 0.0f , cylinder->height , 16 , 16);
    glPopMatrix();
}


/**
 * @brief リストPolygonsに格納された立体を全て表示する.マルチスレッド化不可能
 */
void displayPolygons(List *list)
{
    ListNode *tmp;
    foreach(tmp , list){
        Polygon *data = (Polygon*)tmp->data;
        //見えない立体を表示しない
        if (data->isDisplay == SDL_FALSE){
            continue;
        }
        switch (data->type)
        {
        case PT_RECTANGLER:
            displayRectangler(data->data.rectangler);
            break;

        case PT_SPHERE:
            displaySphere(data->data.sphere);
            break;

        case PT_PLANE4:
            displayPlane4(data->data.plane4);
            break;

        case PT_DISC:
            displayDisc(data->data.disc);
            break;

        case PT_OBJ:
            displayObj(data->data.obj);
            break;

        case PT_CYLINDER:
            displayCylinder(data->data.cylinder);
            break;

        default:
            break;
        }
    }
}