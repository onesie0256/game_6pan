#include "client.h"

int main_scene(void);
int setupScene(void);
int setupSceneLate(void);
int destroyScene(void);

/**
 * @brief メインゲーム
 */
int mainScene(void)
{
    //setupSceneを呼び出すためのフラグ
    static SDL_bool setupFlag = SDL_TRUE;
    //エンドフラグ
    SDL_bool endFlag = SDL_FALSE;

    MainScene *scene = (MainScene *)myGameManager.scene;

    if (setupFlag){
        setupScene();
        setupFlag = SDL_FALSE;
        return 1;
    }

    if (myGameManager.quitRequest == SDL_TRUE) {
        endFlag = SDL_TRUE;
    }
    if (myGameManager.keyNow[K_ENTER]){
        fireGun(scene->myCar , scene->myCar->gun);
    }

    updateAmmos();
    moveCar(scene->cars , scene->polygonList);
    checkCarCheckPoint(scene->cars , scene->course);
    updatePlace();

    updateCamera(scene->myCar , scene->camera);
    draw(scene->camera);

    updateGuns(scene->cars);
    //printf("next point : %f , %f , %f\n" , scene->myCar->nextCheckPoint->coord.x , scene->myCar->nextCheckPoint->coord.y , scene->myCar->nextCheckPoint->coord.z);
    printf("順位: %d\n" , scene->myCar->place);



    if (endFlag){
        destroyScene();
        setupFlag = SDL_TRUE;
        return 0;
    }

    return 1;
}

int setupScene(void)
{
    MainScene *scene = (MainScene*)malloc(sizeof(MainScene));
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



    myGameManager.scene = scene;
    myGameManager.sceneID = Scene_Main;

    #ifdef DEGUG_3DE
    myGameManager.myID = 0;
    myGameManager.playerNum = 3;
    #endif

    scene->myCar = NULL;

    //createRectangler((Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.0f,1.0f,1.0f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 0 , 0 , scene->polygonList);
    scene->myCar = createCar(scene->cars , 0 , (Vec3f){0.0f,3.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    createCar(scene->cars , 1 , (Vec3f){0.0f,5.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    createCar(scene->cars , 2 , (Vec3f){0.0f,7.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    createPlane4((Vec3f){-5.0f,-0.1f,5.0f} , 20.0f , 20.0f , (Vec3f){0.0f,1.0f,0.0f} , 90 , 0 , 0 , PT_PLANE4 , scene->polygonList);
    createRectangler((Vec3f){-3.0f , 0.0f , 0.0f} , (Vec3f){1.0f , 1.0f , 1.0f} , (Vec3f){0.0f , 0.0f , 1.0f} , 0 , 0 , 30 , scene->polygonList);

    
    //Polygon *obj = createObj("assets/models/coco.obj" , "assets/models/coco.png" , (Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.5f,1.5f,1.5f} , 0 , 0 , 0 , scene->polygonList);

    //if (obj == NULL) 
    //printf("load obj error\n");
    

    setupSceneLate();
    return 0;
}

int setupSceneLate(void)
{
    carPlaceAlgorithmSetup();
}

int destroyScene(void)
{
    MainScene *scene = (MainScene*)myGameManager.scene;
    destroyList(scene->polygonList);
    destroyList(scene->cars);
    destroyList(scene->UIList);
    free(scene->camera);
    free(scene);
    return 0;
}
