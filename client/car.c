#include "client.h"

Car *createCar(List *list , uint8_t id , Vec3f coord)
{
    Car *car = (Car *)malloc(sizeof(Car));
    car->id = id;
    car->center = coord;
    car->collisionBox = createRectangler(coord , (Vec3f){0.5f,0.5f,0.8f}, (Vec3f){1.0f,0.0f,0.0f} , 0 , 0 , 0 , NULL);

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