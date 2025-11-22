#include "client.h"

Car *createCar(List *list , uint8_t id , Vec3f coord)
{
    Car *car = (Car *)malloc(sizeof(Car));
    car->id = id;
    car->center = coord;
    car->collisionBox = createRectangler(coord , (Vec3f){0.8f,0.5f,0.5f}, (Vec3f){1.0f,0.0f,0.0f} , 0 , 0 , 0 , NULL);
    car->velocity = (Vec3f){-1.0f,0.0f,0.0f};
    car->direction = (Vec3f){1.0f,0.0f,0.0f};
    memcpy(car->preCoordOfVertexs , car->collisionBox->data.rectangler->vertex , sizeof(Vec3f)*8);
    addListNode(list , car , NULL);
    return car;
}

void displayCars(List *list)
{
    ListNode *node;
    foreach(node , list){
        Car *car = ((Car *)node->data);

        displayRectangler(car->collisionBox->data.rectangler);
    }
}

void rotateCar(Car *car)
{
    Rectangler *r = car->collisionBox->data.rectangler;

    Vec3f eulurs = euler_from_vectors((Vec3f){1.0f , 0.0f , 0.0f} , car->direction);
    rotateRectacglerTo(r , (int)eulurs.x , (int)eulurs.z , (int)eulurs.y , car->center);
}

void moveCar(List *carList , List *PolygonList)
{
    ListNode *node;
    foreach(node , carList){
        Car *car = ((Car *)node->data);
        Rectangler *r = car->collisionBox->data.rectangler;

        

        //car->velocity.x = -1.0f;
        car->velocity = vecAdd(car->velocity , (Vec3f){0.0f,-0.1f,0.0f});
        car->center = vecAdd(car->center , car->velocity);
        moveRectacgler(car->collisionBox->data.rectangler , car->velocity , 1.0f/60.0f);
        
        collision(PolygonList , car->collisionBox->data.rectangler , car->preCoordOfVertexs , &car->velocity);
    
        //処理前の座標を保存
        for (int i = 0 ; i < 8 ; i++){
            car->preCoordOfVertexs[i] = car->collisionBox->data.rectangler->vertex[i];
        }

        car->center = vecMulSca(vecAdd(r->vertex[0] , r->vertex[7]) , 0.50f);
        
        //rotateCar(car);
        
        car->direction = vecNormalize(car->velocity);
        
    }
}

//void destroyCar(Car *car)