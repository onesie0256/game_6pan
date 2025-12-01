#include "client.h"

const WeaponInfo sniper_ = (WeaponInfo){.kind = Sniper , .name = "Sniper"};
const WeaponInfo shotgun_ = (WeaponInfo){.kind = Shotgun , .name = "Shotgun"};
const WeaponInfo pistol_ = (WeaponInfo){.kind = Pistol , .name = "Pistol"};

static Gun sniper = (Gun){.kind = Sniper , .bulletNum = 10 , .maxBulletNum = 10 , .reloadFrame = 10 , .fireCoolFrame = 90 , .damage = 10 , .model = NULL , .ammoSpeed = 3.0f , .maxAmmoLivingFrame = 60};
static Gun shotgun = (Gun){.kind = Shotgun , .bulletNum = 10 , .maxBulletNum = 10 , .reloadFrame = 10 , .fireCoolFrame = 90 , .damage = 10 , .model = NULL , .ammoSpeed = 3.0f , .maxAmmoLivingFrame = 60};
static Gun pistol = (Gun){.kind = Pistol , .bulletNum = 10 , .maxBulletNum = 1 , .reloadFrame = 10 , .fireCoolFrame = 30 , .damage = 10 , .model = NULL , .ammoSpeed = 3.0f , .maxAmmoLivingFrame = 60};

static List *ammoList = NULL;
void register_ammoList(List *list);
Gun* createGun(GunKinds kind);
void fireGun(Car *car , Gun *gun);
Ammo* createAmmo(Gun *gun , Vec3f coord , Vec3f direct);
void updateAmmos(void);
void destroyAmmo(Ammo *ammo);
void displayAmmos(void);
void updateGuns(List *carList);


void register_ammoList(List *list)
{
    ammoList = list;
}

Gun* createGun(GunKinds kind)
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

    return rtn;
}

void fireGun(Car *car , Gun *gun)
{
    if (gun->bulletNum <= 0 || gun->fireCoolFrameNow > 0) return;
    gun->fireCoolFrameNow = gun->fireCoolFrame;
    gun->bulletNum--;
    createAmmo(gun , car->center , car->direction);
}

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
    ammo->model = createSphere(ammo->center , 0.8f , ammo->color , NULL);
    ammo->livingFrameNow = 0;
    ammo->maxLivingFrame = gun->maxAmmoLivingFrame;
    SDL_itoa(id++ , ammo->id , 10);
    
    addListNode(ammoList , ammo , ammo->id);

    return ammo;
}

void updateAmmos(void)
{
    if (ammoList == NULL) return;
    ListNode *node;
    foreach(node , ammoList){
        Ammo *ammo = node->data;
        Sphere *s = ammo->model->data.sphere;
        s->center = vecAdd(ammo->center , vecMulSca(ammo->velocity , 1.0f/60.0f));
        ammo->center = vecAdd(ammo->center , vecMulSca(ammo->velocity , 1.0f/60.0f));
        ammo->livingFrameNow++;
        if (ammo->livingFrameNow >= ammo->maxLivingFrame){
            destroyAmmo(ammo);
        }
        ammo->preCoord = ammo->center;
    }
}

void destroyAmmo(Ammo *ammo)
{
    char id[6];
    strcpy(id , ammo->id);
    destroyPolygon(ammo->model);
    ammo->model = NULL;
    free(ammo);
    deleteListNodeKey(ammoList , id);
}

void displayAmmos(void)
{
    ListNode *node;
    foreach(node , ammoList){
        Ammo *ammo = (Ammo *)node->data;
        displaySphere(ammo->model->data.sphere);
    }
}

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
        
    }
}