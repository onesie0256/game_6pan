#include "common.h"

#define MAX_PLAYER 10

Course *createCourse(Polygon **checkPointPlaneZero , CheckPoint **checkPointZero);
void checkCarCheckPoint(List *carList , Course *course);
void destroyCourse(Course *course);
Polygon *set_checkPointPlaneZero(void);
CheckPoint *set_checkPointZero(void);
void carPlaceAlgorithmSetup(void);
void loadCheckPointData(char *filePath , Course *course);

static MainScene *scene = NULL;
static int placeAry[MAX_PLAYER];
static Car *carAry[MAX_PLAYER];

/**
 * @brief コースの構造体を作成する
 * @param checkPointPlaneZero 最初のチェックポイントの平面のポインタへのポインタ
 * @param checkPointZero 最初のチェックポイントの構造体のポインタへのポインタ
 */
Course *createCourse(Polygon **checkPointPlaneZero , CheckPoint **checkPointZero)
{
    Course *course = (Course *)malloc(sizeof(Course));
    course->courseModel = NULL;
    course->courseCollision = createList();
    course->checkPointPlaneList = createList();
    course->checkPointPointList = createList();
    course->checkPointNum = 0;

    /*
    Polygon *p = createPlane4((Vec3f){0.0f,-1.0f,0.0f} , 20.0f , 20.0f , (Vec3f){0.0f,1.0f,0.0f} , 0 , 0 , 0 , PT_PLANE4 , NULL);
    addListNode(course->checkPointPlaneList , p , "0");
    course->checkPointNum++;

    Polygon *p2 = createPlane4((Vec3f){0.0f,-1.0f,-3.0f} , 20.0f , 20.0f , (Vec3f){1.0f,0.0f,0.0f} , 0 , 0 , 0 , PT_PLANE4 , NULL);
    addListNode(course->checkPointPlaneList , p2 , "1");
    course->checkPointNum++;
    */
    loadCheckPointData("assets/data/checkpointdata.data" , course);

    ListNode *node;
    int id = 0;
    foreach(node , course->checkPointPlaneList){
        Polygon *p = (Polygon *)node->data;
        Plane4 *plane = p->data.plane4;
        
        CheckPoint *cp = (CheckPoint *)malloc(sizeof(CheckPoint));

        cp->coord = vecMulSca(vecAdd(plane->vertex[0] , plane->vertex[2]) , 0.5f);
        cp->normal = vecMulSca(plane->normal , -1.0);
        char tmp[20] = {0};
        sprintf(tmp , "%d" , id);
        addListNode(course->checkPointPointList , cp , tmp);

        if (id == 0){
            *checkPointPlaneZero = p;
            *checkPointZero = cp;
        }
        id++;
    }
    
    return course;
}

/**
 * @brief 車1つのチェックポイントを確認する
 * @details アルゴリズム:チェックポイントの座標をp,いまフレームの車の中心座表をB,前フレームの車の中心座表をA,チェックポイント平面の法線ベクトルをCとすると,
 * @param car 車の構造体
 * @param cp 車の次のチェックポイントへのポインタ
 * @param plane_ 車の次のチェックポイントの平面のポリゴンへのポインタ
 * 
 * @return 車がチェックポイントを通過したらSDL_True,そうでないならSDL_False
 */
SDL_bool checkCarPoint(Car *car , CheckPoint *cp , Polygon *plane_)
{
    Plane4 *plane = plane_->data.plane4;
    Vec3f pa = vecSub(car->preCenter , cp->coord);
    Vec3f pb = vecSub(car->center , cp->coord);
    Vec3f C = cp->normal;

    //isPointOnPlane4_(car->center , plane) && (vecMulIn(pa , C) < 0) && (vecMulIn(pb , C) > 0)

    if (isPointOnPlane4_(car->center , plane) && (vecMulIn(pb , C) > 0)){
        printf("No.%d : チェックポイントを%d通過\n" , car->id , car->checkPointNum);
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

/**
 * @brief リスト内全ての車のチェックポイントを確認する
 * 
 * @param carList 車のリスト
 * @param corse コースの構造体
 * 
 */
void checkCarCheckPoint(List *carList , Course *course)
{
    ListNode *node;
    foreach(node , carList){
        Car *car = (Car *)node->data;
        if (!checkCarPoint(car , car->nextCheckPoint , car->nextPlane)) continue;
        char id_char[20] = {0};
        car->checkPointNum++;
        if (car->checkPointNum >= course->checkPointNum){
            car->checkPointNum = 0;
            car->rapNum++;

            car->nextPlane = scene->checkPointPlaneZero;
            car->nextCheckPoint = scene->checkPointZero;


        printf("No.%d : %d周完了\n" , car->id , car->rapNum);
        }

        sprintf(id_char , "%d" , car->checkPointNum);
        ListNode *nodeNextPlene = serchListNode(course->checkPointPlaneList , id_char);
        if (nodeNextPlene == NULL) continue;
        car->nextPlane = (Polygon *)nodeNextPlene->data;
        

        ListNode *nodeNextCheckPoint = serchListNode(course->checkPointPointList , id_char);
        if (nodeNextCheckPoint == NULL) continue;
        car->nextCheckPoint = (CheckPoint *)nodeNextCheckPoint->data;
        
    }
}

/**
 * @brief 車の順位決定アルゴリズムのセットアップ,updatePlace()を呼び出す前にこれを呼び出す
 * 
 */
void carPlaceAlgorithmSetup(void)
{
    if (scene == NULL) scene = (MainScene *)myGameManager.scene;

    for (int i = 0 ; i < MAX_PLAYER ; i++){
        placeAry[i] = i;
    }

    ListNode *node;
    foreach(node , scene->cars){
        Car *car = (Car *)node->data;
        carAry[car->id] = car;
    }
}

/**
 * @brief updatePlace()内部で使用するqsortの比較関数,順位決定アルゴリズムの中核
 * 
 * @details 車のラップ数,チェックポイントの数で比較を行い,それで順位が決まらなかった場合は次のチェックポイントまでの距離とチェックポイントの法線の内積で決定する
 */
int comp_place(const void *a , const void *b)
{
    Car *carA = carAry[*(int *)a];
    Car *carB = carAry[*(int *)b];

    if (carA->rapNum < carB->rapNum) return 1;
    if (carA->rapNum > carB->rapNum) return -1;
    
    if (carA->checkPointNum < carB->checkPointNum) return 1;
    if (carA->checkPointNum > carB->checkPointNum) return -1;

    Vec3f AtoP = vecSub(carA->center , carA->nextCheckPoint->coord);
    Vec3f BtoP = vecSub(carB->center , carB->nextCheckPoint->coord);

    float lenA = vecLength(AtoP);
    float lenB = vecLength(BtoP);

    lenA = vecMulIn(AtoP , carA->nextCheckPoint->normal);
    lenB = vecMulIn(BtoP , carB->nextCheckPoint->normal);

    if (lenA < lenB) return 1;
    if (lenA > lenB) return -1;

    return 0;
}

/**
 * @brief 全ての車の順位を更新する
 */
void updatePlace(void)
{
    qsort(&placeAry[scene->goaledPlayerNum] , (size_t)(myGameManager.playerNum - scene->goaledPlayerNum) , sizeof(int) , comp_place);

    for (int i = scene->goaledPlayerNum ; i < myGameManager.playerNum ; i++){
        Car *car = carAry[placeAry[i]];
        car->place = i + 1;
    }
}

void loadCheckPointData(char *filePath , Course *course)
{
    FILE *fp = fopen(filePath , "r");
    if (fp == NULL){
        printf("couldn't open file\n");
        return;
    }

    List *list = course->checkPointPlaneList;

    char buf[256];
    while (fgets(buf , sizeof(buf) , fp) != NULL){
        Vec3f coord;
        float sizeX , sizeY;
        int yaw;

        sscanf(buf , "%f,%f,%f %f,%f %d" , &coord.x , &coord.y , &coord.z , &sizeX , &sizeY , &yaw);

        Polygon *p = createPlane4(coord , sizeX , sizeY , (Vec3f){0.0f , 1.0f , 0.0f} , 0 , 0 , yaw , PLT_Wall , NULL);
        course->checkPointNum++;

        static int id = 0;
        char tmp[20] = {0};
        sprintf(tmp , "%d" , id);
        addListNode(list , p , tmp);
        id++;
    }

    fclose(fp);
}