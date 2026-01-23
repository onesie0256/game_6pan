#include "common.h"

void calcCollisionCarVel(Vec3f *v1 , Vec3f *v2);
SDL_bool collisionThre(Car *car , List *polygonList);

/**
 * @brief Car構造体を作成する
 * 
 * @param list Carを格納するリスト
 * @param id id
 * @param coord 初期座標
 * @param kind 銃の種類
 * @param nextPlaneZero 最初のチェックポイントのポリゴン
 * @param nextCheckPointZero 最初のチェックポイントの構造体
 * 
 * @return 車の構造体へのポインタ
 */
Car *createCar(List *list , uint8_t id , Vec3f coord , GunKinds kind , Polygon *nextPlaneZero , CheckPoint *nextCheckPointZero)
{
    Car *car = (Car *)malloc(sizeof(Car));
    car->id = id;
    car->center = coord;
    car->preCenter = coord;
    Vec3f color = (Vec3f){1.0f , 0.0f , 0.0f};
    switch (id)
    {
    case 0:
        color = (Vec3f){1.0f , 0.0f , 0.0f};
        break;
    case 1:
        color = (Vec3f){0.0f , 0.0f , 1.0f};
        break;
    case 2:
        color = (Vec3f){0.0f , 0.0f , 0.5f};
        break;

    case 3:
        color = (Vec3f){1.0f , 1.0f , 0.0f};
        break;

    case 4:
        color = (Vec3f){0.8627f , 0.0784f , 0.2352f};
        break;

    case 5:
        color = (Vec3f){0.5411f , 0.1686f , 0.8862f};
        break;
    
    case 6:
        color = (Vec3f){0.0f , 1.0f , 0.0f};
        break;
        
    case 7:
        color = (Vec3f){1.0f , 0.5f , 0.0f};
        break;

    case 8:
        color = (Vec3f){0.4117f , 0.4117f , 0.4117f};
        break;

    case 9:
        color = (Vec3f){0.9411f , 0.0f , 0.0f};
        break;

    default:
        break;
    }

    car->model = createObjInfo((Vec3f){0.0f,-0.1f,0.0f} , (Vec3f){0.025f,0.025f,0.025f} , 0,0,270);

    car->collisionBox = createRectangler(coord , (Vec3f){0.3f,0.15f,0.15f} , color , 0 , 0 , 0 , NULL);
    car->velocity = (Vec3f){0.0f,0.0f,0.0f};
    car->direction = (Vec3f){-1.0f,0.0f,0.0f};
    memcpy(car->preCoordOfVertexs , car->collisionBox->data.rectangler->vertex , sizeof(Vec3f)*8);
    car->hp = 100.0f;
    car->speed = 0.0f;
    car->shotFlag = SDL_FALSE;
    car->gun = createGun(kind , id);
    car->isGoaled = SDL_FALSE;
    car->isOnGround = SDL_FALSE;

    char id_char[6] = {0};
    sprintf(id_char , "%d" , id);
    addListNode(list , car , id_char);

    car->rapNum = 0;
    car->checkPointNum = 0;
    car->nextPlane = nextPlaneZero;
    car->nextCheckPoint = nextCheckPointZero;
    car->q = quaternion_identity();
    car->q_pre = quaternion_identity();

    updateCarCenter(car);

    car->centerZero = car->center;
    for (int i = 0 ; i < 8 ; i++){
        car->vertZero[i] = ((Rectangler *)(car->collisionBox->data.rectangler))->vertex[i];
    }


    return car;
}

/**
 * @brief 車の中心座標を更新する
 * @param car 車の構造体
 * 
 * @return なし
 */
void updateCarCenter(Car *car)
{
    Rectangler *r = car->collisionBox->data.rectangler;
    car->center = vecMulSca(vecAdd(r->vertex[0] , r->vertex[7]) , 0.50f);
}

/**
 * @brief リストの中の全ての車を表示する
 * 
 * @param list 車が格納されたリスト構造体
 * 
 * @return なし
 */
void displayCars(List *list)
{
    ListNode *node;
    foreach(node , list){
        Car *car = ((Car *)node->data);

        //displayRectangler(car->collisionBox->data.rectangler);

        car->model->coord = car->center;
        car->model->yaw = car->gun->model->yaw;

        Vec3f eulurs = quaternion_to_euler(car->q);
        car->gun->model->yaw = eulurs.y*RAD_TO_DEG;
        //printf("yaw: %d\n" , car->gun->model->yaw);

        /*
        if (car->isOnGround){
            Vec3f tmp = vecNormalize(vecSub(car->center , car->preCenter));
            eulurs = euler_from_vectors(tmp , car->direction);

            car->gun->model->pitch = eulurs.x*RAD_TO_DEG;

            if (car->gun->model->pitch == 180){
                car->gun->model->pitch = 0;
            }

            printf("pitch: %d\n" , car->gun->model->pitch);
        }
        else {
            car->gun->model->pitch = 0.0f;
        }
        */

        displayObjEX(&myGameManager.models[GunKinds_Max+car->id] , car->model);
        displayObjEX(&myGameManager.models[car->gun->kind] , car->gun->model);
    }
}

/**
 * @brief 車をヨー角で回転させる
 * 
 * @param car 車の構造体
 * @param deg 回転角
 * 
 * @return なし
 */
void rotateCar(Car *car , int deg)
{
    //Rectangler *r = car->collisionBox->data.rectangler;

    //Vec3f eulurs = euler_from_vectors((Vec3f){1.0f , 0.0f , 0.0f} , car->direction);
    //rotateRectacgler(r , 0 , 0 , deg , car->center);
    Quaternion q_ = quaternion_from_axis_angle((Vec3f){0.0f , 1.0f , 0.0f} , deg);
    car->q = quaternion_multiply(car->q , q_);
    rotateCarEX(car);
    //rotateRectacglerQuaternion(r , car->q , car->center);
}

void rotateCarEX(Car *car)
{
    Rectangler *r = car->collisionBox->data.rectangler;
    Vec3f vert[8];

    // モデルのローカル座標系での中心を計算
    Vec3f modelCenterOffset = vecMulSca(vecAdd(car->vertZero[0], car->vertZero[7]), 0.5f);
    // ワールド空間での車の移動量を計算
    Vec3f delta = vecSub(car->center , car->centerZero);
    
    for (int i = 0 ; i < 8 ; i++){
        // 1. 初期頂点を取得
        vert[i] = car->vertZero[i];
        // 2. モデルの中心が原点に来るように移動
        vert[i] = vecSub(vert[i], modelCenterOffset);
        // 3. 原点中心で回転
        vert[i] = quaternion_rotate_vector(vert[i] , car->q);
        // 4. モデルの中心を元の位置に戻す
        vert[i] = vecAdd(vert[i], modelCenterOffset);
        // 5. 車全体のワールド空間での移動を適用
        vert[i] = vecAdd(vert[i] , delta);
    }

    for (int i = 0 ; i < 8 ; i++){
        r->vertex[i] = vert[i];
    }

    upadteRectangler(r);
}

#define CURVE_DEGREE 1

/**
 * @brief 車を入力情報に従って移動させる
 * @param car 車
 * @param inputAry 廃止予定
 * 
 * @return なし
 */
void forwardCar(Car *car)
{

    int curve_deg = CURVE_DEGREE;
    SDL_bool *inputAry = myGameManager.clients[car->id].keyNow;
    SDL_bool *inputJoy = myGameManager.clients[car->id].joyBotton;
    SDL_bool *preinputAry = myGameManager.clients[car->id].keyPrev;

    Vec3f dir = car->direction;

    int joyStickFlag = 0;

    if (myGameManager.clients[car->id].stickY <= -0.5f){
        joyStickFlag = -1;
    }
    else if (myGameManager.clients[car->id].stickY >= 0.5f){
        joyStickFlag = 1;
    }



    if (car->isOnGround){
        Vec3f delta = vecSub(car->center , car->preCenter);
        //printf("delta.y = %f\n" , delta.y);
        if (delta.y >= 0.002f){
            dir.y = delta.y*100.0f;
            //dir = vecNormalize(dir);
            //printf("directon = (%f , %f , %f)\n" , dir.x , dir.y , dir.z);
        }
    }

    if (inputAry[K_SHIFT] || inputJoy[JOY_SR]){
        curve_deg *= 2;
    }

    if (inputAry[K_LEFT] || joyStickFlag == -1){
        rotateCar(car , curve_deg);
        car->direction = rotateVecWithQuaternion(car->direction , curve_deg , (Vec3f){0.0f , 1.0f , 0.0f});

        /*
        if (inputAry[K_SHIFT]){
            car->direction = rotateXYZ(car->direction , 0 , 0 , CURVE_DEGREE);
            rotateCar(car , CURVE_DEGREE/2);
        }
        */
    }

    if (inputAry[K_RIGHT] || joyStickFlag == 1){
        rotateCar(car , -curve_deg);
        car->direction = rotateVecWithQuaternion(car->direction , -curve_deg , (Vec3f){0.0f , 1.0f , 0.0f});

        /*
        if (inputAry[K_SHIFT]){
            car->direction = rotateXYZ(car->direction , 0 , 0 , CURVE_DEGREE);
            rotateCar(car , -CURVE_DEGREE);
        }
        */
    }

    if (inputAry[K_UP] || inputJoy[JOY_Y]){
        car->velocity = vecAdd(car->velocity , vecMulSca(dir , 0.2f));
    }

    if (inputAry[K_DOWN] || inputJoy[JOY_X]){
        car->velocity = vecAdd(car->velocity , vecMulSca(dir , -0.2f));
    }
    /*
    if (inputAry[K_SPACE] && !preinputAry[K_SPACE]){
        car->velocity = vecAdd(car->velocity , (Vec3f){0.0f,2.0f,0.0f});
    }
    */
}

/**
 * @brief 車を指定の座標に移動させる
 * 
 * @param car 車
 * @return なし
 */
void teleportCar(Car *car , Vec3f coord)
{
    updateCarCenter(car);
    Vec3f delta = vecSub(coord , car->center);
    car->center = coord;
    moveRectacgler(car->collisionBox->data.rectangler , delta , 1.0f);
    updateCarCenter(car);
}

/**
 * @brief 車を指定の座標に移動させる
 * 
 * @param car 車
 * @return なし
 */
void teleportCarEX(Car *car)
{
    Vec3f delta = vecSub(car->center , car->preCenter);
    moveRectacgler(car->collisionBox->data.rectangler , delta , 1.0f);
    updateCarCenter(car);
}

/**
 * @brief リスト内全ての車に移動・物理演算を行う
 * 
 * @param carList 車のリスト
 * @param PolygonList 衝突判定させるポリゴンのリスト
 * @return なし
 */
void moveCar(List *carList , List *PolygonList , int count)
{
    ListNode *node;
    foreach(node , carList){
        Car *car = ((Car *)node->data);
        Rectangler *r = car->collisionBox->data.rectangler;

        updateCarCenter(car);

        if (count > 120 + 60*3)
            forwardCar(car);

        car->preCenter = car->center;

        //car->velocity.x = -1.0f;
        car->velocity = vecAdd(car->velocity , (Vec3f){0.0f,-0.1f,0.0f});

        moveRectacgler(car->collisionBox->data.rectangler , car->velocity , 1.0f/60.0f);

        if (r->vertex[0].y < -10.0f){
            teleportCar(car , (Vec3f){0.0f,5.0f,0.7f});
        }

        updateCarCenter(car);


        car->isOnGround = SDL_FALSE;
        car->isOnGround = collision(PolygonList , car->collisionBox->data.rectangler , car->preCoordOfVertexs , &car->velocity);

    
        

        updateCarCenter(car);

        //rotateCar(car);
        
        //car->direction = vecNormalize(car->velocity);
        
        car->speed = vecLength((Vec3f){car->velocity.x , 0.0f , car->velocity.z});
    }

    collisionCars(carList);

    foreach(node , carList){
        Car *car = (Car *)node->data;
        //処理前の座標を保存
        for (int i = 0 ; i < 8 ; i++){
            car->preCoordOfVertexs[i] = car->collisionBox->data.rectangler->vertex[i];
        }
    }
}

void damageCar(Car *car , float damage)
{
    car->hp -= damage;
    if (car->hp <= 0.0f){
        car->hp = 100.0f;
        car->velocity = vecMulSca(car->velocity , 0.15f);
        car->velocity.y += 8.0f;
    }
}

Car *getCarFromId(List *carList , uint8_t id)
{
    ListNode *node;
    char id_char[6] = {0};
    sprintf(id_char , "%d" , id);
    foreach(node , carList){
        if (strcmp(node->key , id_char) == 0){
            return (Car *)node->data;
        }
    }
    return NULL;
}


/**
 * @brief 2つ車間で衝突処理を行う
 * 
 * @param c1 車1
 * @param c2 車2
 * 
 * @return 衝突したらSDL_True,そうでないならSDL_False
 */
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

/**
 * @brief リスト全ての車と衝突処理を行う
 * @param carList 車のリスト
 */
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

#define THREAD_NUM 6

typedef struct threadData_t{
    Car *car;
    List *polygonList;
    SDL_bool flag;
    int start;
    int end;
}threadData;


int tmp(void *arg)
{
    threadData *data = (threadData *)arg;
    Car *car = data->car;
    List *polygonList = data->polygonList;
    int start = data->start;
    int end = data->end;

    ListNode *startNode = polygonList->head;

    for (int i = 0 ; i < start ; i++){
        startNode = startNode->next;
    }

    data->flag = collision_n(startNode , car->collisionBox->data.rectangler , car->preCoordOfVertexs , &car->velocity , end - start + 1);

    return 0;
}

SDL_bool collisionThre(Car *car , List *polygonList)
{
    SDL_bool flag = SDL_FALSE;

    SDL_Thread *threads[THREAD_NUM];
    threadData data[THREAD_NUM];

    int count = polygonList->count/THREAD_NUM;
    int start = 0;
    int end = count-1;
    for (int i = 0 ; i < THREAD_NUM-1 ; i++){
        data[i].car = car;
        data[i].polygonList = polygonList;
        data[i].flag = SDL_FALSE;
        data[i].start = start;
        data[i].end = end;
        
        start += count;
        end += count;

        threads[i] = SDL_CreateThread(tmp , "collision" , &data[i]);
    }

    int last = THREAD_NUM-1;

    data[last].car = car;
    data[last].polygonList = polygonList;
    data[last].flag = SDL_FALSE;
    data[last].start = start;
    data[last].end = polygonList->count;

    threads[last] = SDL_CreateThread(tmp , "collision" , &data[last]);

    for (int i = 0 ; i < THREAD_NUM ; i++){
        SDL_WaitThread(threads[i] , NULL);
        flag = flag || data[i].flag;
    }

    return flag;
}