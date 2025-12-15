#include "client.h"

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
    car->collisionBox = createRectangler(coord , (Vec3f){0.8f,0.5f,0.5f} , color , 0 , 0 , 0 , NULL);
    car->velocity = (Vec3f){0.0f,0.0f,0.0f};
    car->direction = (Vec3f){-1.0f,0.0f,0.0f};
    memcpy(car->preCoordOfVertexs , car->collisionBox->data.rectangler->vertex , sizeof(Vec3f)*8);
    car->hp = 100.0f;
    car->speed = 0.0f;
    car->gun = createGun(kind , id);

    char id_char[6] = {0};
    sprintf(id_char , "%d" , id);
    addListNode(list , car , id_char);

    car->rapNum = 0;
    car->checkPointNum = 0;
    car->nextPlane = nextPlaneZero;
    car->nextCheckPoint = nextCheckPointZero;

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

        displayRectangler(car->collisionBox->data.rectangler);
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
    Rectangler *r = car->collisionBox->data.rectangler;

    //Vec3f eulurs = euler_from_vectors((Vec3f){1.0f , 0.0f , 0.0f} , car->direction);
    rotateRectacglerTo(r , 0 , 0 , deg , car->center);
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
    SDL_bool *preinputAry = myGameManager.clients[car->id].keyPrev;

    if (inputAry[K_SHIFT]){
        curve_deg *= 2;
    }

    if (inputAry[K_LEFT]){
        car->direction = rotateXYZ(car->direction , 0 , 0 , curve_deg);
        rotateCar(car , curve_deg);
        /*
        if (inputAry[K_SHIFT]){
            car->direction = rotateXYZ(car->direction , 0 , 0 , CURVE_DEGREE);
            rotateCar(car , CURVE_DEGREE/2);
        }
        */
    }

    if (inputAry[K_RIGHT]){
        car->direction = rotateXYZ(car->direction , 0 , 0 , -curve_deg);
        rotateCar(car , -curve_deg);
        /*
        if (inputAry[K_SHIFT]){
            car->direction = rotateXYZ(car->direction , 0 , 0 , CURVE_DEGREE);
            rotateCar(car , -CURVE_DEGREE);
        }
        */
    }

    if (inputAry[K_UP]){
        car->velocity = vecAdd(car->velocity , vecMulSca(car->direction , 0.1f));
    }

    if (inputAry[K_DOWN]){
        car->velocity = vecAdd(car->velocity , vecMulSca(car->direction , -0.1f));
    }

    if (inputAry[K_ENTER] && !preinputAry[K_ENTER]){
        car->velocity = vecAdd(car->velocity , (Vec3f){0.0f,3.0f,0.0f});
    }
}

/**
 * @brief 車を指定の座標に移動させる
 * 
 * @param car 車
 * @return なし
 */
void teleportCar(Car *car , Vec3f coord)
{
    Vec3f delta = vecSub(coord , car->center);
    car->center = coord;
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
void moveCar(List *carList , List *PolygonList)
{
    ListNode *node;
    foreach(node , carList){
        Car *car = ((Car *)node->data);
        Rectangler *r = car->collisionBox->data.rectangler;
        car->preCenter = car->center;

        updateCarCenter(car);

        forwardCar(car);

        //car->velocity.x = -1.0f;
        car->velocity = vecAdd(car->velocity , (Vec3f){0.0f,-0.1f,0.0f});

        moveRectacgler(car->collisionBox->data.rectangler , car->velocity , 1.0f/60.0f);

        if (r->vertex[0].y < -10.0f){
            teleportCar(car , (Vec3f){0.0f,3.0f,0.0f});
        }
        collision(PolygonList , car->collisionBox->data.rectangler , car->preCoordOfVertexs , &car->velocity);
    
        

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
        car->velocity = vecMulSca(car->velocity , 0.3f);
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


