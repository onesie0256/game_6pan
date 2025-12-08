#include "client.h"

Car *createCar(List *list , uint8_t id , Vec3f coord , GunKinds kind)
{
    Car *car = (Car *)malloc(sizeof(Car));
    car->id = id;
    car->center = coord;
    Vec3f color = (Vec3f){1.0f,0.0f,0.0f};
    switch (id)
    {
    case 1:
        color = (Vec3f){0.0f,0.0f,1.0f};
        break;

    case 2:
        color = (Vec3f){0.0f,1.0f,0.0f};
        break;

    case 3:
        color = (Vec3f){1.0f,1.0f,0.0f};
        break;

    case 4:
        color = (Vec3f){1.0f,0.0f,1.0f};
        break;

    case 5:
        color = (Vec3f){0.0f,1.0f,1.0f};
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
    car->gun = createGun(kind);
    addListNode(list , car , NULL);
    return car;
}

void updateCarCenter(Car *car)
{
    Rectangler *r = car->collisionBox->data.rectangler;
    car->center = vecMulSca(vecAdd(r->vertex[0] , r->vertex[7]) , 0.50f);
}

void displayCars(List *list)
{
    ListNode *node;
    foreach(node , list){
        Car *car = ((Car *)node->data);

        displayRectangler(car->collisionBox->data.rectangler);
    }
}

void rotateCar(Car *car , int deg)
{
    Rectangler *r = car->collisionBox->data.rectangler;

    //Vec3f eulurs = euler_from_vectors((Vec3f){1.0f , 0.0f , 0.0f} , car->direction);
    rotateRectacglerTo(r , 0 , 0 , deg , car->center);
}

#define CURVE_DEGREE 1

void forwardCar(Car *car , SDL_bool inputAry[])
{
<<<<<<< Updated upstream
    if (car->id != 0) return;
=======

    int curve_deg = CURVE_DEGREE;
    SDL_bool *inputAry = myGameManager.clients[car->id].keyNow;
    SDL_bool *preinputAry = myGameManager.clients[car->id].keyPrev;

    if (inputAry[K_SHIFT]){
        curve_deg *= 2;
    }

>>>>>>> Stashed changes
    if (inputAry[K_LEFT]){
        car->direction = rotateXYZ(car->direction , 0 , 0 , CURVE_DEGREE);
        rotateCar(car , CURVE_DEGREE);
        /*
        if (inputAry[K_SHIFT]){
            car->direction = rotateXYZ(car->direction , 0 , 0 , CURVE_DEGREE);
            rotateCar(car , CURVE_DEGREE/2);
        }
        */
    }

    if (inputAry[K_RIGHT]){
        car->direction = rotateXYZ(car->direction , 0 , 0 , -CURVE_DEGREE);
        rotateCar(car , -CURVE_DEGREE);
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

void teleportCar(Car *car , Vec3f coord)
{
    Vec3f delta = vecSub(coord , car->center);
    car->center = coord;
    moveRectacgler(car->collisionBox->data.rectangler , delta , 1.0f);
    updateCarCenter(car);
}

void moveCar(List *carList , List *PolygonList)
{
    ListNode *node;
    foreach(node , carList){
        Car *car = ((Car *)node->data);
        Rectangler *r = car->collisionBox->data.rectangler;

        updateCarCenter(car);

        forwardCar(car , myGameManager.keyNow);

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

//void destroyCar(Car *car)