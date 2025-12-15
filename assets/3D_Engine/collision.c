#include "3DEngine.h"

void collision(List *list , Rectangler *rectangler , Vec3f preCoordOfVertex[] , Vec3f *velocity);
SDL_bool collisionPlane(Rectangler *rectangler , Plane4 *plane , Vec3f preCoordOfVertex[]);
SDL_bool collisionRectangler(Rectangler *rectangler_s , Rectangler *rectangler_d , Vec3f preCoordOfVertex[] , Vec3f *velocity);
SDL_bool collisionPlane3(Rectangler *rectangler , Plane3 *plane , Vec3f preCoordOfVertex[]);

/**
 * @brief 四角形と衝突処理する
 */
void collision(List *list , Rectangler *rectangler , Vec3f preCoordOfVertex[] , Vec3f *velocity)
{
    ListNode *tmp;
    foreach(tmp , list){
        Polygon *polygon = (Polygon *)tmp->data;
        switch (polygon->type)
        {
        case PT_PLANE4:
            if(collisionPlane(rectangler , polygon->data.plane4 , preCoordOfVertex))
                *velocity = calcCollisionVel(*velocity , polygon->data.plane4->normal);
                *velocity = vecMulSca(*velocity , 0.98f);
            break;

        case PT_RECTANGLER:
            collisionRectangler(polygon->data.rectangler , rectangler , preCoordOfVertex , velocity);
            break;
        
        case PT_PLANE3:
            if(collisionPlane3(rectangler , polygon->data.plane3 , preCoordOfVertex)){
                *velocity = calcCollisionVel(*velocity , polygon->data.plane3->normal);
                *velocity = vecMulSca(*velocity , 0.98f);
            }
            break;
        

        default:
            break;
        }
    }
}

SDL_bool collisionPlane(Rectangler *rectangler , Plane4 *plane , Vec3f preCoordOfVertex[])
{
    Vec3f v = plane->vertex[0];
    Vec3f n = plane->normal;
    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler->vertex[i] , v , n) && isPointOnPlane4_(rectangler->vertex[i] , plane)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler , moveVec , 1.0f);
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

/**
 * @param rectangler_s 跳ね返らない方
 * @param rectangler_d 跳ね返る方
 */
SDL_bool collisionRectangler(Rectangler *rectangler_s , Rectangler *rectangler_d , Vec3f preCoordOfVertex[] , Vec3f *velocity)
{
    Vec3f v = rectangler_s->vertex[0];
    Vec3f n = rectangler_s->normals[0];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler_d->vertex[i] , v , n) && isPointOnPlane4(rectangler_d->vertex[i] , rectangler_s->vertex[0] , rectangler_s->vertex[1] , rectangler_s->vertex[2] , rectangler_s->vertex[3] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler_d->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler_d , moveVec , 1.0f);
            *velocity = calcCollisionVel(*velocity , n);
            return SDL_TRUE;
        }
    }

    v = rectangler_s->vertex[1];
    n = rectangler_s->normals[1];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler_d->vertex[i] , v , n) && isPointOnPlane4(rectangler_d->vertex[i] , rectangler_s->vertex[1] , rectangler_s->vertex[4] , rectangler_s->vertex[7] , rectangler_s->vertex[2] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler_d->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler_d , moveVec , 1.0f);
            *velocity = calcCollisionVel(*velocity , n);
            return SDL_TRUE;
        }
    }

    v = rectangler_s->vertex[4];
    n = rectangler_s->normals[2];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler_d->vertex[i] , v , n) && isPointOnPlane4(rectangler_d->vertex[i] , rectangler_s->vertex[4] , rectangler_s->vertex[5] , rectangler_s->vertex[6] , rectangler_s->vertex[7] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler_d->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler_d , moveVec , 1.0f);
            *velocity = calcCollisionVel(*velocity , n);
            return SDL_TRUE;
        }
    }

    v = rectangler_s->vertex[5];
    n = rectangler_s->normals[3];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler_d->vertex[i] , v , n) && isPointOnPlane4(rectangler_d->vertex[i] , rectangler_s->vertex[5] , rectangler_s->vertex[0] , rectangler_s->vertex[3] , rectangler_s->vertex[6] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler_d->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler_d , moveVec , 1.0f);
            *velocity = calcCollisionVel(*velocity , n);
            return SDL_TRUE;
        }
    }
    
    v = rectangler_s->vertex[3];
    n = rectangler_s->normals[4];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler_d->vertex[i] , v , n) && isPointOnPlane4(rectangler_d->vertex[i] , rectangler_s->vertex[3] , rectangler_s->vertex[2] , rectangler_s->vertex[7] , rectangler_s->vertex[6] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler_d->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler_d , moveVec , 1.0f);
            *velocity = calcCollisionVel(*velocity , n);
            return SDL_TRUE;
        }
    }
    

    v = rectangler_s->vertex[0];
    n = rectangler_s->normals[5];

    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler_d->vertex[i] , v , n) && isPointOnPlane4(rectangler_d->vertex[i] , rectangler_s->vertex[0] , rectangler_s->vertex[5] , rectangler_s->vertex[4] , rectangler_s->vertex[1] , n)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler_d->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler_d , moveVec , 1.0f);
            *velocity = calcCollisionVel(*velocity , n);
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}

SDL_bool collisionPlane3(Rectangler *rectangler , Plane3 *plane , Vec3f preCoordOfVertex[])
{
    Vec3f v = plane->vertex[0];
    Vec3f n = plane->normal;
    for (int i = 0 ; i < 8 ; i++){
        if (isPointOnPlane(preCoordOfVertex[i] , v , n) && !isPointOnPlane(rectangler->vertex[i] , v , n) && isPointOnPlane3(rectangler->vertex[i] , plane->vertex[0] , plane->vertex[1] , plane->vertex[2] , plane->normal)){
            Vec3f H; float l;
            l = lengthPointToPlaneAndH(&H , v , n , rectangler->vertex[i]);
            Vec3f moveVec = vecMulSca(n , l);
            moveRectacgler(rectangler , moveVec , 1.0f);
            return SDL_TRUE;
        }
    }
    return SDL_FALSE;
}