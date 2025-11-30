/**
 * @brief これをインクルードすれば3D使用可能
 */
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_opengl.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <stdio.h>
#include "3Dstructs.h"

#define DEG_TO_RAD 0.01745329
#define RAD_TO_DEG 57.2957795

/* 3DEmain.c */


/* vector.c */
Vec3f vecAdd(Vec3f a , Vec3f b);
Vec3f vecSub(Vec3f a , Vec3f b);
float vecMulIn(Vec3f a , Vec3f b);
Vec3f vecMulOut(Vec3f a , Vec3f b);
Vec3f vecMulSca(Vec3f a , float b);
Vec3f rotateX(Vec3f vert , int pitch);
Vec3f rotateZ(Vec3f vert , int roll);
Vec3f rotateY(Vec3f vert , int yaw);
Vec3f rotateXYZ(Vec3f vert , int pitch , int roll , int yaw);
float vecLength(Vec3f a);
Vec3f vecNormalize(Vec3f a);
Vec3f calcNormalVec(Vec3f a , Vec3f b , Vec3f c);
uint8_t serchVecSign(Vec3f a);
Vec3f roteteP(Vec3f target , Vec3f center , int pitch , int roll , int yaw);
Vec3f rotateCenter(Vec3f vert , int pitch , int roll , int yaw , Vec3f center);

/* list.c */
List* createList(void);
void addListNode(List *list, void *data , char *key);
ListNode *serchListNode(List *list , char *key);
void deleteListNode(List *list, ListNode *node);
void deleteListNodeKey(List *list , char *key);
void deleteListNodeAll(List *list);
void destroyList(List *list);

/* quaternion.c */
Quaternion quaternion_identity(void);
Quaternion quaternion_from_axis_angle(Vec3f axis, float angle_deg);
Quaternion quaternion_multiply(Quaternion q1, Quaternion q2);
Quaternion quaternion_normalize(Quaternion q);
Vec3f quaternion_rotate_vector(Vec3f v, Quaternion q);
Vec3f quaternion_to_euler(Quaternion q);
Vec3f euler_from_vectors(Vec3f v1, Vec3f v2);

/* fileIO.c */
SDL_bool loadOBJ(const char* obj_filename , const char* texture_filename , Obj *obj);

/* polygone.c */
Polygon* createRectangler(Vec3f coord , Vec3f size , Vec3f color , int pitch , int roll , int yaw , List *list);
Polygon* createSphere(Vec3f center , float radius , Vec3f color , List *list);
Polygon* createPlane4(Vec3f coord , float sizeX , float sizeY , Vec3f color , int pitch , int roll , int yaw , PlaneType type , List *list);
Polygon* createObj(const char* obj_filename , const char* texture_filename , Vec3f coordOffset , Vec3f scale , int pitchOffset , int rollpitchOffset , int yawpitchOffset , List *list);
Polygon* createDisc(Vec3f center , float radius , Vec3f color , List *list);
Polygon* createCylinder(List *list);
void displayPolygons(List *list);
void displayRectangler(Rectangler *rectangler);
void displaySphere(Sphere *sphere);
void displayPlane4(Plane4 *plane4);
void displayObj(Obj *obj);
void displayDisc(Disc *disc);
void displayCylinder(Cylinder *cylinder);
void moveRectacgler(Rectangler *rectangler , Vec3f velocity , float dt);
void rotateRectacglerTo(Rectangler *rectangler , int pitch , int roll , int yaw , Vec3f center);
void destroyPolygon(Polygon *polygon);

/* space.c */
float lengthPointToPlaneAndH(Vec3f *H, Vec3f v , Vec3f n , Vec3f p);
float lengthPointToLine(Vec3f p , Vec3f v1 , Vec3f v2 , float *t);
//float lengthPointToPlane(Vec3f p , float a , float b , float c , float d);
SDL_bool isPointInRect(Vec3f H , Vec3f v1 , Vec3f v2 , Vec3f v3 , Vec3f v4 , Vec3f normal);
SDL_bool isPointInCircle(Vec3f H , Vec3f center , float radius);
SDL_bool isPointOnPlane(Vec3f p  , Vec3f v , Vec3f n);
SDL_bool isPointOnPlane4(Vec3f p , Vec3f v1 , Vec3f v2 , Vec3f v3 , Vec3f v4 , Vec3f normal);
Vec3f calcCollisionVel(Vec3f velocity , Vec3f normal);
void moveCamera(Camera *camera , float dx , float dy , float dz);
void rotateCamera(Camera *camera, float pitch_delta, float yaw_delta);
//void updateCamera(Camera *camera);

/* collision.c */
void collision(List *list , Rectangler *rectangler , Vec3f preCoordOfVertex[] , Vec3f *velocity);

/* list */
/**
 * @brief c#のforeach文に感銘を受けたので,リスト構造体をforeachで回せるようにしてみました. x:ListNode型のポインタ list:List型のポインタ
 */
#define foreach(x,list) for(x=(list)->head;x!=NULL;x=x->next)
#define foreach_(x,node) for(x=node->next;x!=NULL;x=x->next)

/* space.c */

#define isPointOnPlane4_(p , plane4) isPointOnPlane4(p , plane4->vertex[0] , plane4->vertex[1] , plane4->vertex[2] , plane4->vertex[3] , plane4->normal)
