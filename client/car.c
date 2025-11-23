#include "client.h"

Car *createCar(List *list , uint8_t id , Vec3f coord)
{
    Car *car = (Car *)malloc(sizeof(Car));
    car->id = id;
    car->center = coord;
    car->collisionBox = createRectangler(coord , (Vec3f){0.8f,0.5f,0.5f}, (Vec3f){1.0f,0.0f,0.0f} , 0 , 0 , 0 , NULL);
    car->velocity = (Vec3f){0.0f,0.0f,0.0f};
    car->direction = (Vec3f){-1.0f,0.0f,0.0f};
    memcpy(car->preCoordOfVertexs , car->collisionBox->data.rectangler->vertex , sizeof(Vec3f)*8);
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
    if (inputAry[K_LEFT]){
        car->direction = rotateXYZ(car->direction , 0 , 0 , CURVE_DEGREE);
        rotateCar(car , CURVE_DEGREE);
    }

    if (inputAry[K_RIGHT]){
        car->direction = rotateXYZ(car->direction , 0 , 0 , -CURVE_DEGREE);
        rotateCar(car , -CURVE_DEGREE);
    }

    if (inputAry[K_UP]){
        car->velocity = vecAdd(car->velocity , vecMulSca(car->direction , 0.1f));
    }

    if (inputAry[K_DOWN]){
        car->velocity = vecAdd(car->velocity , vecMulSca(car->direction , -0.1f));
    }
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
        
        collision(PolygonList , car->collisionBox->data.rectangler , car->preCoordOfVertexs , &car->velocity);
    
        //処理前の座標を保存
        for (int i = 0 ; i < 8 ; i++){
            car->preCoordOfVertexs[i] = car->collisionBox->data.rectangler->vertex[i];
        }

        updateCarCenter(car);

        //rotateCar(car);
        
        //car->direction = vecNormalize(car->velocity);
        
    }
}

//void destroyCar(Car *car)