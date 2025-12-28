#include "client.h"

int main_scene(void);
int setupScene(void);
int setupSceneLate(void);
int destroyScene(void);
int receiveDataThreFunc();
void updateCar_c(void);

static Car *c2 = NULL;

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

    if (scene->sendCarInfoPlayerNum < myGameManager.playerNum) {
        if (myGameManager.quitRequest == SDL_TRUE) { 
             endFlag = SDL_TRUE;
        } else {
            ;
        }
    } else {
        scene->sendCarInfoPlayerNum = 0;
        if (myGameManager.quitRequest == SDL_FALSE) {
            send_input_data();
            updateCar_c();
        }

        if (scene->count >= 5 || scene->count == 0)
        {
            ListNode *node;
            foreach(node , scene->cars){
                Car *car = (Car*)node->data;
                if (car->shotFlag == SDL_TRUE){
                    fireGun(car , car->gun);
                }
            }
        }

        
        updateAmmos();
        collisionAmmoCars_c(scene->cars);
        updateGuns(scene->cars);

        checkCarCheckPoint(scene->cars , scene->course);
        updatePlace();
    }

    

    //updateAmmos();
    //moveCar(scene->cars , scene->polygonList);
    //collisionAmmoCars(scene->cars);

    updateCamera(scene->myCar , scene->camera);

    UI_updateMainSurface(scene); 
    if(scene->myCar) UI_drawBullets(scene->myCar->gun);

    draw(scene->camera);

    
    //printf("next point : %f , %f , %f\n" , scene->myCar->nextCheckPoint->coord.x , scene->myCar->nextCheckPoint->coord.y , scene->myCar->nextCheckPoint->coord.z);
    //printf("順位: %d\n" , scene->myCar->place);
    //printf("c2.hp: %f\n" , c2->hp);
    //printf("弾数:%d\n" , scene->myCar->gun->bulletNum);
    //printf("(x,y,z):(%f,%f,%f)\n" , scene->myCar->center.x , scene->myCar->center.y , scene->myCar->center.z);
    

    if (endFlag){
        destroyScene();
        setupFlag = SDL_TRUE;

        SDL_Delay(100);
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
    scene->sendInputDataPlayerNum = 0;
    scene->sendCarInfoPlayerNum = 0;
    scene->count = 4;

    /*
    myGameManager.carModels[Shotgun] = createObj("assets/models/shotgun_o.obj" , "assets/models/shotgun.png" , (Vec3f){0.0f,3.0f,0.0f} , (Vec3f){0.5f,0.5f,0.5f} , 0 , 0 , 0 , scene->polygonList);
    printf("shotgun: v:%d , n:%d , t:%d\n , va:%d , na:%d , ta:%d\n" , myGameManager.carModels[Shotgun]->data.obj->vertNum, myGameManager.carModels[Shotgun]->data.obj->normalNum , myGameManager.carModels[Shotgun]->data.obj->texCoordNum , myGameManager.carModels[Shotgun]->data.obj->vertAryNum , myGameManager.carModels[Shotgun]->data.obj->normAryNum , myGameManager.carModels[Shotgun]->data.obj->texCoordAryNum);
    
    myGameManager.carModels[Sniper] = createObj("assets/models/sniper_o.obj" , "assets/models/sniper.png" , (Vec3f){0.0f,3.0f,0.0f} , (Vec3f){0.5f,0.5f,0.5f} , 0 , 0 , 0 , scene->polygonList);
    printf("sniper: v:%d , n:%d , t:%d\n , va:%d , na:%d , ta:%d\n" , myGameManager.carModels[Sniper]->data.obj->vertNum, myGameManager.carModels[Sniper]->data.obj->normalNum , myGameManager.carModels[Sniper]->data.obj->texCoordNum , myGameManager.carModels[Sniper]->data.obj->vertAryNum , myGameManager.carModels[Sniper]->data.obj->normAryNum , myGameManager.carModels[Sniper]->data.obj->texCoordAryNum);


    myGameManager.carModels[Pistol] = createObj("assets/models/pistol_o.obj" , "assets/models/pistol.png" , (Vec3f){0.0f,3.0f,0.0f} , (Vec3f){0.5f,0.5f,0.5f} , 0 , 0 , 0 , scene->polygonList);
    printf("pistol: v:%d , n:%d , t:%d\n , va:%d , na:%d , ta:%d\n" , myGameManager.carModels[Pistol]->data.obj->vertNum, myGameManager.carModels[Pistol]->data.obj->normalNum , myGameManager.carModels[Pistol]->data.obj->texCoordNum , myGameManager.carModels[Pistol]->data.obj->vertAryNum , myGameManager.carModels[Pistol]->data.obj->normAryNum , myGameManager.carModels[Pistol]->data.obj->texCoordAryNum);
    */

    SDL_FillRect(myGameManager.UI , NULL , SDL_MapRGBA(myGameManager.UI->format , 0 , 0 , 0 , 0));


    scene->bulletTextureID = loadTexture("assets/pictures/bullet.png");
    if (scene->bulletTextureID == 0) {
        printf("Failed to load bullet texture!\n");
        // NOTE: Consider more robust error handling
    }

    myGameManager.scene = scene;
    myGameManager.sceneID = Scene_Main;
    myGameManager.sceneID = Scene_Main;

    #ifdef DEGUG_3DE
    myGameManager.myID = 0;
    myGameManager.playerNum = 2;
    #endif

    scene->myCar = NULL;

    //当たり判定のサポート
    //createRectangler((Vec3f){-5.2f,3.0f,-13.0f} , (Vec3f){9.0f,0.5f,1.5f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 3 , 0 , scene->polygonList);

    //createRectangler((Vec3f){-10.0f,1.9f,-12.8f} , (Vec3f){5.7f,0.5f,1.9f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 13 , 0 , scene->polygonList);
    //createRectangler((Vec3f){-18.0f,0.1f,-12.3f} , (Vec3f){9.0f,0.5f,2.4f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 13 , 0 , scene->polygonList);




    #ifdef DEGUG_3DE
    scene->myCar = createCar(scene->cars , 0 , (Vec3f){0.0f,3.0f,0.0f} , Sniper , scene->checkPointPlaneZero , scene->checkPointZero);
    c2 = createCar(scene->cars , 1 , (Vec3f){0.0f,5.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    #else
    for (int i = 0 ; i < myGameManager.playerNum ; i++){
        Vec3f coord = (Vec3f){0.3f + (float)i*0.5f , 2.0f , 0.7f - (float)i*0.3f};
        if (i >= 5) {
            coord.z += 1.35f;
        }
        Car *car = createCar(scene->cars , i , coord , myGameManager.clients[i].gunId , scene->checkPointPlaneZero , scene->checkPointZero);
        if (i == myGameManager.myID){
            scene->myCar = car;
        }
    }

    #endif
    //createCar(scene->cars , 2 , (Vec3f){0.0f,7.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    //createPlane4((Vec3f){-5.0f,-0.1f,5.0f} , 20.0f , 20.0f , (Vec3f){0.0f,1.0f,0.0f} , 90 , 0 , 0 , PT_PLANE4 , scene->polygonList);
    //createRectangler((Vec3f){-3.0f , 0.0f , 0.0f} , (Vec3f){1.0f , 1.0f , 1.0f} , (Vec3f){0.0f , 0.0f , 1.0f} , 0 , 0 , 30 , scene->polygonList);
    createObj("assets/models/course.obj" , "assets/models/course.png" , (Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.0f,1.0f,1.0f} , 0 , 0 , 0 , scene->polygonList);


    
    //Polygon *obj = createObj("assets/models/coco.obj" , "assets/models/coco.png" , (Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.5f,1.5f,1.5f} , 0 , 0 , 0 , scene->polygonList);

    //if (obj == NULL) 
    //printf("load obj error\n");
    

    setupSceneLate();

    waitUntilAck();
    send_input_data();
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

void updateCar_c(void)
{
    MainScene *scene = (MainScene *)myGameManager.scene;
    SDL_mutex *m = SDL_CreateMutex();
    ListNode *node;
    foreach(node , scene->cars){
        Car *car = (Car*)node->data;

        teleportCarEX(car);
        rotateCarEX(car);
        
        
        car->direction = quaternion_rotate_vector((Vec3f){-1.0f , 0.0f , 0.0f} , car->q);
        //car->direction = quaternion_rotate_vector_left(car->direction , car->q);
        updateCarCenter(car);
        car->preCenter = car->center;
    }
    SDL_DestroyMutex(m);
}
