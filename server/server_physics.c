#include "server.h"

void setupSceneLate(void);

static MainScene *scene_ = NULL;

void setupPhysics(void)
{
    MainScene *scene = (MainScene *)malloc(sizeof(MainScene));

    scene->polygonList = createList();
    scene->cars = createList();
    scene->UIList = createList();
    scene->ammoList = createList();
    register_ammoList(scene->ammoList);
    scene->course = createCourse(&(scene->checkPointPlaneZero) , &(scene->checkPointZero));
    scene->camera = (Camera*)malloc(sizeof(Camera));
    scene->camera->pos = (Vec3f){0.0f,0.0f,0.0f};
    scene->camera->orientation = (Quaternion){0.0f,0.0f,0.0f,0.0f};
    scene->myCar = NULL;
    scene->goaledPlayerNum = 0;
    scene->sendInputDataPlayerNum = 0;
    scene->sendCarInfoPlayerNum = 0;

    myGameManager.scene = scene;
    myGameManager.sceneID = Scene_Main;

    myGameManager.myID = 0;

    scene->myCar = NULL;

    //createRectangler((Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.0f,1.0f,1.0f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 0 , 0 , scene->polygonList);
    #ifdef DEGUG_3DE
    scene->myCar = createCar(scene->cars , 0 , (Vec3f){0.0f,3.0f,0.0f} , Sniper , scene->checkPointPlaneZero , scene->checkPointZero);
    c2 = createCar(scene->cars , 1 , (Vec3f){0.0f,5.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    #else
    for (int i = 0 ; i < myGameManager.playerNum ; i++){
        Car *car = createCar(scene->cars , i , (Vec3f){(float)i, 3.0f , 0.0f} , Shotgun , scene->checkPointPlaneZero , scene->checkPointZero);
    }

    #endif
    //createCar(scene->cars , 2 , (Vec3f){0.0f,7.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    //createPlane4((Vec3f){-5.0f,-0.1f,5.0f} , 20.0f , 20.0f , (Vec3f){0.0f,1.0f,0.0f} , 90 , 0 , 0 , PT_PLANE4 , scene->polygonList);
    //createRectangler((Vec3f){-3.0f , 0.0f , 0.0f} , (Vec3f){1.0f , 1.0f , 1.0f} , (Vec3f){0.0f , 0.0f , 1.0f} , 0 , 0 , 30 , scene->polygonList);
    loadOBJ_collison("assets/obj/course.obj" , scene->polygonList);


    
    //Polygon *obj = createObj("assets/models/coco.obj" , "assets/models/coco.png" , (Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.5f,1.5f,1.5f} , 0 , 0 , 0 , scene->polygonList);

    //if (obj == NULL) 
    //printf("load obj error\n");
    

    setupSceneLate();

    scene_ = scene;
}

void setupSceneLate(void)
{
    carPlaceAlgorithmSetup();
}

Uint32 update_physics(Uint32 interval , void *name)
{
    if (myGameManager.quitRequest == SDL_TRUE){
        return 0;
    }

    
    for (int i = 0 ; i < myGameManager.playerNum ; i++){
        if (myGameManager.clients[i].keyNow[K_ENTER] == SDL_TRUE){
            Car *tmp = getCarFromId(scene_->cars , i);
            if (tmp != NULL){
                fireGun(tmp , tmp->gun);
            }
            
        }
    }

    updateAmmos();
    moveCar(scene_->cars , scene_->polygonList);
    collisionAmmoCars(scene_->cars);
    //checkCarCheckPoint(scene_->cars , scene_->course);
    //updatePlace();

    //updateCamera(scene_->myCar , scene_->camera);
    //draw(scene_->camera);

    updateGuns(scene_->cars);

    sendCarInfo();

    return interval;
}