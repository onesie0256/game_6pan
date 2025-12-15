#include "client.h"

int main_scene(void);
int setupScene(void);
int setupSceneLate(void);
int destroyScene(void);
int receiveDataThreFunc();

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

    
    // 前のフレームの全プレイヤーデータを受信するまで待つ
    // ただし、ビジーループでブロックするのではなく、gameLoopに戻る
    if (scene->sendInputDataPlayerNum < myGameManager.playerNum) {
        if (myGameManager.quitRequest == SDL_TRUE) { // 待っている間に終了要求が来た場合
             endFlag = SDL_TRUE;
        } else {
            // まだ受信中なので、今回のフレームの送信・更新処理はスキップ
            // ただし描画などは行いたいので、関数の末尾まで処理は継続させる
        }
    } else {
        // 全員分のデータを受信したら、カウンタをリセットして、今回の自分の入力を送信する
        scene->sendInputDataPlayerNum = 0;
        if (myGameManager.quitRequest == SDL_FALSE) {
            send_input_data();
        }
    }

    for (int i = 0 ; i < myGameManager.playerNum ; i++){
        if (myGameManager.clients[i].keyNow[K_ENTER] == SDL_TRUE){
            Car *tmp = getCarFromId(scene->cars , i);
            if (tmp != NULL){
                fireGun(tmp , tmp->gun);
            }
            
        }
    }

    updateAmmos();
    moveCar(scene->cars , scene->polygonList);
    collisionAmmoCars(scene->cars);
    checkCarCheckPoint(scene->cars , scene->course);
    updatePlace();

    updateCamera(scene->myCar , scene->camera);

    UI_updateMainSurface(scene); 

    draw(scene->camera);

    updateGuns(scene->cars);
    //printf("next point : %f , %f , %f\n" , scene->myCar->nextCheckPoint->coord.x , scene->myCar->nextCheckPoint->coord.y , scene->myCar->nextCheckPoint->coord.z);
    //printf("順位: %d\n" , scene->myCar->place);
    //printf("c2.hp: %f\n" , c2->hp);
    //printf("弾数:%d\n" , scene->myCar->gun->bulletNum);



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
    

    SDL_FillRect(myGameManager.UI , NULL , SDL_MapRGBA(myGameManager.UI->format , 0 , 0 , 0 , 0));


    myGameManager.scene = scene;
    myGameManager.sceneID = Scene_Main;

    #ifdef DEGUG_3DE
    myGameManager.myID = 0;
    myGameManager.playerNum = 2;
    #endif

    scene->myCar = NULL;

    //createRectangler((Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.0f,1.0f,1.0f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 0 , 0 , scene->polygonList);
    #ifdef DEGUG_3DE
    scene->myCar = createCar(scene->cars , 0 , (Vec3f){0.0f,3.0f,0.0f} , Sniper , scene->checkPointPlaneZero , scene->checkPointZero);
    c2 = createCar(scene->cars , 1 , (Vec3f){0.0f,5.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
    #else
    for (int i = 0; i < myGameManager.playerNum; i++) {
    GunKinds kind = (i == myGameManager.myID)? myGameManager.gunId: myGameManager.clients[i].gunId;

    Car *car = createCar(
        scene->cars, i, (Vec3f){(float)i, 3.0f, 0.0f}, kind,scene->checkPointPlaneZero,scene->checkPointZero
    );

    if (i == myGameManager.myID) {
        scene->myCar = car;
    }
}


    #endif
    //createCar(scene->cars , 2 , (Vec3f){0.0f,7.0f,0.0f} , Pistol , scene->checkPointPlaneZero , scene->checkPointZero);
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
    send_input_data();
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