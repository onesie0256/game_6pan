#include "client.h"

int main_scene(void);
int setupScene(void);
int destroyScene(void);

/**
 * @brief メインゲーム
 */
int mainScene(void)
{
    //setupSceneを呼び出すためのフラグ
    static SDL_bool setupFlag = SDL_TRUE;
    //エンドフラグ
    SDL_bool endFlag = SDL_TRUE;

    MainScene *scene = (MainScene *)myGameManager.scene;

    if (setupFlag){
        setupScene();
        setupFlag = SDL_FALSE;
        return 1;
    }
    

    draw(scene->camera);
    SDL_Delay(4000);

    if (endFlag){
        destroyScene();
        setupFlag = SDL_TRUE;
        return 0;
    }

    //だいたい16msで60fps
    SDL_Delay(16);
    return 0;
}

int setupScene(void)
{
    MainScene *scene = (MainScene*)malloc(sizeof(MainScene));
    scene->polygonList = createList();
    scene->cars = createList();
    scene->UIList = createList();
    scene->camera = (Camera*)malloc(sizeof(Camera));
    scene->camera->pos = (Vec3f){0.0f,0.0f,0.0f};
    scene->camera->orientation = (Quaternion){0.0f,0.0f,0.0f,0.0f};
    myGameManager.UI = NULL;

    myGameManager.scene = scene;
    myGameManager.sceneID = Scene_Main;

    createRectangler((Vec3f){0.0f,0.0f,0.0f} , (Vec3f){1.0f,1.0f,1.0f} , (Vec3f){0.0f,0.0f,1.0f} , 0 , 0 , 0 , scene->polygonList);

    return 0;
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
