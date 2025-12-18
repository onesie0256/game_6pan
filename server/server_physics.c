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
    scene->count = 0;

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
        Vec3f coord = (Vec3f){0.3f + (float)i*0.5f , 2.0f , 0.7f - (float)i*0.3f};
        if (i >= 5) {
            coord.z += 1.5f;
            coord.x -= 2.2f;
        }
        Car *car = createCar(scene->cars , i , coord , myGameManager.clients[i].gunId , scene->checkPointPlaneZero , scene->checkPointZero);
    }

    #endif
    //createCar(scene->cars , 2 , (Vec3f){0.0f,7.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    //当たり判定のサポート
    createRectangler((Vec3f){-5.2f,3.0f,-13.0f} , (Vec3f){9.0f,0.5f,1.5f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 3 , 0 , scene->polygonList);

    createRectangler((Vec3f){-10.0f,1.9f,-12.8f} , (Vec3f){5.7f,0.5f,1.9f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 13 , 0 , scene->polygonList);
    createRectangler((Vec3f){-18.0f,0.1f,-12.3f} , (Vec3f){9.0f,0.5f,2.4f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 13 , 0 , scene->polygonList);

    loadOBJ_collison("assets/models/course.obj" , scene->polygonList);


    
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

    if (scene_->count >= 300){
        for (int i = 0 ; i < myGameManager.playerNum ; i++){
            if (myGameManager.clients[i].keyNow[K_ENTER] == SDL_TRUE){
                Car *tmp = getCarFromId(scene_->cars , i);
                if (tmp != NULL){
                    fireGun(tmp , tmp->gun);
                }
            
            }
        }
    }

    updateAmmos();
    moveCar(scene_->cars , scene_->polygonList , scene_->count);
    collisionAmmoCars(scene_->cars);
    checkCarCheckPoint(scene_->cars , scene_->course);
    updatePlace();

    //updateCamera(scene_->myCar , scene_->camera);
    //draw(scene_->camera);

    updateGuns(scene_->cars);

    sendCarInfo();

    scene_->count++;

    switch (scene_->count)
    {
    case 120:
        sendCount(3);
        break;
    
    case 180:
        sendCount(2);
        break;

    case 240:
        sendCount(1);
        break;
    
    case 300:
        sendCount(0);
        break;

    case 360:
        sendCount(5);
        break;

    case 1000000:
        scene_->count = 361;
        break;
    
    default:
        break;
    }
    //printf("%d,%d\n" , myGameManager.playerNum , scene_->goaledPlayerNum);
    if (scene_->goaledPlayerNum == myGameManager.playerNum){
        
        static int endCount = 0;
        endCount++;
        if (endCount == 180){
            printf("all player goaled , send quit\n");
            sendQuit();
            myGameManager.quitRequest = SDL_TRUE;
        }
    }

    return interval;
}