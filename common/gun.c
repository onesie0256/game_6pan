#include "common.h"

const WeaponInfo sniper_ = (WeaponInfo){.kind = Sniper , .name = "Sniper"};
const WeaponInfo shotgun_ = (WeaponInfo){.kind = Shotgun , .name = "Shotgun"};
const WeaponInfo pistol_ = (WeaponInfo){.kind = Pistol , .name = "Pistol"};

//スナイパーの基礎ステータス
static Gun sniper = (Gun){  
    .kind = Sniper ,
    .maxBulletNum = 3 ,
    .reloadFrame = 180 ,
    .fireCoolFrame = 60 ,
    .damage = 80 ,
    .model = NULL ,
    .ammoSpeed = 40.0f ,
    .maxAmmoLivingFrame = 60 ,
    .ammoRadius = 0.5f};

//ショットガンの基礎ステータス
static Gun shotgun = (Gun){
    .kind = Shotgun ,
    .maxBulletNum = 5 ,
    .reloadFrame = 180 ,
    .fireCoolFrame = 20 ,
    .damage = 20 ,
    .model = NULL ,
    .ammoSpeed = 30.0f ,
    .maxAmmoLivingFrame = 20 ,
    .ammoRadius = 0.1f};

//ピストルの基礎ステータス
static Gun pistol = (Gun){
    .kind = Pistol ,
    .maxBulletNum = 10 ,
    .reloadFrame = 50 ,
    .fireCoolFrame = 12 ,
    .damage = 15 ,
    .model = NULL ,
    .ammoSpeed = 30.0f ,
    .maxAmmoLivingFrame = 60 ,
    .ammoRadius = 0.3f};

static List *ammoList = NULL;
void register_ammoList(List *list);
Gun* createGun(GunKinds kind , int carId);
void fireGun(Car *car , Gun *gun);
Ammo* createAmmo(Gun *gun , Vec3f coord , Vec3f direct);
void updateAmmos(void);
void destroyAmmo(Ammo *ammo);
void displayAmmos(void);
void updateGuns(List *carList);

/**
 * @brief セットアップ
 * @brief 銃弾の構造体のリスト
 */
void register_ammoList(List *list)
{
    ammoList = list;
}

/**
 * @brief 銃を作成する
 * 
 * @param kind 銃の種類
 * 
 * @return 銃の構造体へのポインタ
 */
Gun* createGun(GunKinds kind , int carId)
{
    Gun *rtn = (Gun *)malloc(sizeof(Gun));
    switch (kind)
    {
    case Sniper:
        memcpy(rtn , &sniper , sizeof(Gun));
        break;
    case Shotgun:
        memcpy(rtn , &shotgun , sizeof(Gun));
        break;
    case Pistol:
        memcpy(rtn , &pistol , sizeof(Gun));
        break;
    default:
        return NULL;
    }

    rtn->bulletNum = rtn->maxBulletNum;
    rtn->reloadFrameNow = 0;
    rtn->fireCoolFrameNow = 0;

    rtn->carId = carId;

    return rtn;
}

#define SHOTGUN_AMMO_ROTATION 4

/**
 * @brief 銃を打ち,銃弾を生成する
 * 
 * @param car 車
 * @param gum 銃の構造体
 * 
 */
void fireGun(Car *car , Gun *gun)
{
    if (gun->bulletNum <= 0 || gun->fireCoolFrameNow > 0) return;
    gun->fireCoolFrameNow = gun->fireCoolFrame;
    gun->bulletNum--;
    createAmmo(gun , car->center , car->direction);
    if (gun->kind != Shotgun) return;
    createAmmo(gun , car->center , rotateVecWithQuaternion(car->direction , SHOTGUN_AMMO_ROTATION , (Vec3f){0.0f , 1.0f , 0.0f}));
    createAmmo(gun , car->center , rotateVecWithQuaternion(car->direction , -SHOTGUN_AMMO_ROTATION , (Vec3f){0.0f , 1.0f , 0.0f}));

    Vec3f tmp = rotateVecWithQuaternion(car->direction , -SHOTGUN_AMMO_ROTATION , (Vec3f){1.0f , 0.0f , 0.0f});
    createAmmo(gun , car->center , rotateVecWithQuaternion(tmp , SHOTGUN_AMMO_ROTATION , (Vec3f){0.0f , 1.0f , 0.0f}));
    createAmmo(gun , car->center , rotateVecWithQuaternion(tmp , 0 , (Vec3f){0.0f , 1.0f , 0.0f}));
    createAmmo(gun , car->center , rotateVecWithQuaternion(tmp , -SHOTGUN_AMMO_ROTATION , (Vec3f){0.0f , 1.0f , 0.0f}));

    tmp = rotateVecWithQuaternion(tmp , -SHOTGUN_AMMO_ROTATION , (Vec3f){1.0f , 0.0f , 0.0f});
    createAmmo(gun , car->center , rotateVecWithQuaternion(tmp , SHOTGUN_AMMO_ROTATION , (Vec3f){0.0f , 1.0f , 0.0f}));
    createAmmo(gun , car->center , rotateVecWithQuaternion(tmp , 0 , (Vec3f){0.0f , 1.0f , 0.0f}));
    createAmmo(gun , car->center , rotateVecWithQuaternion(tmp , -SHOTGUN_AMMO_ROTATION , (Vec3f){0.0f , 1.0f , 0.0f}));


}

/**
 * @brief 銃弾の構造体を生成する
 * 
 * @param gun 銃弾
 * @param coord 銃弾の初期座標
 * @param direct 銃弾の方向のベクトル
 * 
 * @return 銃弾の構造体
 */
Ammo* createAmmo(Gun *gun , Vec3f coord , Vec3f direct)
{
    static int id = 0;
    Ammo *ammo = (Ammo *)malloc(sizeof(Ammo));
    ammo->kind = gun->kind;
    ammo->center = coord;
    ammo->preCoord = coord;
    ammo->velocity = vecMulSca(direct , gun->ammoSpeed);
    ammo->damage = gun->damage;
    ammo->color = (Vec3f){1.0f , 0.0f , 0.0f};
    ammo->livingFrameNow = 0;
    ammo->maxLivingFrame = gun->maxAmmoLivingFrame;
    ammo->carId = gun->carId;
    SDL_itoa(id++ , ammo->id , 10);
    ammo->carId = gun->carId;
    ammo->radius = gun->ammoRadius;

    Vec3f color = (Vec3f){1.0f , 0.0f , 0.0f};
    switch (gun->carId)
    {
    case 0:
        color = (Vec3f){1.0f , 0.0f , 0.0f};
        break;
    case 1:
        color = (Vec3f){0.0f , 0.0f , 1.0f};
        break;
    case 2:
        color = (Vec3f){0.0f , 0.0f , 0.5f};
        break;

    case 3:
        color = (Vec3f){1.0f , 1.0f , 0.0f};
        break;

    case 4:
        color = (Vec3f){0.8627f , 0.0784f , 0.2352f};
        break;

    case 5:
        color = (Vec3f){0.5411f , 0.1686f , 0.8862f};
        break;
    
    case 6:
        color = (Vec3f){0.0f , 1.0f , 0.0f};
        break;
        
    case 7:
        color = (Vec3f){1.0f , 0.5f , 0.0f};
        break;

    case 8:
        color = (Vec3f){0.4117f , 0.4117f , 0.4117f};
        break;

    case 9:
        color = (Vec3f){0.9411f , 0.0f , 0.0f};
        break;

    default:
        break;
    }
    ammo->color = color;
    ammo->model = createSphere(ammo->center , gun->ammoRadius , ammo->color , NULL);
    
    addListNode(ammoList , ammo , ammo->id);

    return ammo;
}

/**
 * @brief 銃弾の位置などを更新する
 */
void updateAmmos(void)
{
    if (ammoList == NULL) return;
    ListNode *node;
    foreach(node , ammoList){
        Ammo *ammo = node->data;
        Sphere *s = ammo->model->data.sphere;
        ammo->preCoord = ammo->center;
        s->center = vecAdd(ammo->center , vecMulSca(ammo->velocity , 1.0f/60.0f));
        ammo->center = vecAdd(ammo->center , vecMulSca(ammo->velocity , 1.0f/60.0f));
        ammo->livingFrameNow++;
        if (ammo->livingFrameNow >= ammo->maxLivingFrame){
            destroyAmmo(ammo);
        }
    }
}



/**
 * @brief 銃弾を削除し,リストから取り除く
 */
void destroyAmmo(Ammo *ammo)
{
    char id[6];
    strcpy(id , ammo->id);
    destroyPolygon(ammo->model);
    ammo->model = NULL;
    free(ammo);
    deleteListNodeKey(ammoList , id);
}

/**
 * @brief 銃弾を表示する
 */
void displayAmmos(void)
{
    ListNode *node;
    foreach(node , ammoList){
        Ammo *ammo = (Ammo *)node->data;
        displaySphere(ammo->model->data.sphere);
    }
}

/**
 * @brief 銃の状態を更新する
 */
void updateGuns(List *carList)
{
    ListNode *node;
    
    foreach(node , carList){
        Car *car = node->data;
        Gun *gun = car->gun;
        if (gun->reloadFrameNow > 0){
            gun->reloadFrameNow--;
        }
        if (gun->fireCoolFrameNow > 0){
            gun->fireCoolFrameNow--;
        }

        if (gun->bulletNum != 0) continue;

        gun->reloadFrameNow++;

        if (gun->reloadFrameNow++ != gun->reloadFrame) continue;

        gun->bulletNum = gun->maxBulletNum;
        gun->reloadFrameNow = 0;
    }
}

/**
 * @brief 車と銃弾の衝突判定を行う
 */
SDL_bool collisonAmmoCar(Ammo *ammo , Car *car)
{
    Rectangler *r = car->collisionBox->data.rectangler;

    Vec3f v = r->vertex[0];
    Vec3f n = r->normals[0];

    
        if (isPointOnPlane4(ammo->center , r->vertex[0] , r->vertex[1] , r->vertex[2] , r->vertex[3] , n) &&
            ((isPointOnPlane(ammo->preCoord , v , n) && !isPointOnPlane(ammo->center , v , n)) ||
            lengthPointToPlaneAndH(NULL , v , n , ammo->center) < ammo->radius)){
            
            return SDL_TRUE;
        }
    

    v = r->vertex[1];
    n = r->normals[1];

    
        if (isPointOnPlane4(ammo->center , r->vertex[1] , r->vertex[4] , r->vertex[7] , r->vertex[2] , n) &&
            ((isPointOnPlane(ammo->preCoord , v , n) && !isPointOnPlane(ammo->center , v , n)) ||
            lengthPointToPlaneAndH(NULL , v , n , ammo->center) < ammo->radius)){
            
            return SDL_TRUE;
        }
    

    v = r->vertex[4];
    n = r->normals[2];

    
        if (isPointOnPlane4(ammo->center , r->vertex[4] , r->vertex[5] , r->vertex[6] , r->vertex[7] , n) &&
            ((isPointOnPlane(ammo->preCoord , v , n) && !isPointOnPlane(ammo->center , v , n)) ||
            lengthPointToPlaneAndH(NULL , v , n , ammo->center) < ammo->radius)){
            
            return SDL_TRUE;
        }
    

    v = r->vertex[5];
    n = r->normals[3];

    
        if (isPointOnPlane4(ammo->center , r->vertex[5] , r->vertex[0] , r->vertex[3] , r->vertex[6] , n) &&
            ((isPointOnPlane(ammo->preCoord , v , n) && !isPointOnPlane(ammo->center , v , n)) ||
            lengthPointToPlaneAndH(NULL , v , n , ammo->center) < ammo->radius)){
            
            return SDL_TRUE;
        }
    
    
    v = r->vertex[3];
    n = r->normals[4];

    
        if (isPointOnPlane4(ammo->center , r->vertex[3] , r->vertex[2] , r->vertex[7] , r->vertex[6] , n) &&
            ((isPointOnPlane(ammo->preCoord , v , n) && !isPointOnPlane(ammo->center , v , n)) ||
            lengthPointToPlaneAndH(NULL , v , n , ammo->center) < ammo->radius)){
            
            return SDL_TRUE;
        }

    v = r->vertex[0];
    n = r->normals[5];

    
        if (isPointOnPlane4(ammo->center , r->vertex[0] , r->vertex[5] , r->vertex[4] , r->vertex[1] , n) &&
            ((isPointOnPlane(ammo->preCoord , v , n) && !isPointOnPlane(ammo->center , v , n)) ||
            lengthPointToPlaneAndH(NULL , v , n , ammo->center) < ammo->radius)){
            
            return SDL_TRUE;
        }
    
    return SDL_FALSE;
}

void collisionAmmoCars(List *carList)
{
    ListNode *i;
    foreach(i , carList){
        ListNode *j;
        foreach(j , ammoList){
            Car *car = (Car *)i->data;
            Ammo *ammo = (Ammo *)j->data;
            if (car->id == ammo->carId) continue;
            
            if (collisonAmmoCar(ammo , car)){
                damageCar(car , ammo->damage);
                destroyAmmo(ammo);
            }
        }
    }
}

void collisionAmmoCars_c(List *carList)
{
    ListNode *i;
    foreach(i , carList){
        ListNode *j;
        foreach(j , ammoList){
            Car *car = (Car *)i->data;
            Ammo *ammo = (Ammo *)j->data;
            if (car->id == ammo->carId) continue;
            
            if (collisonAmmoCar(ammo , car)){
                destroyAmmo(ammo);
            }
        }
    }
}