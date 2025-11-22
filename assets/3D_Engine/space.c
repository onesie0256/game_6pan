#include "3DEngine.h"

#define CAMERA_SENSITIVITY 3
#define CAMERA_V 0.2f

/**
 * @brief 未使用
 */
SDL_bool serchFloatSign (float a)
{
    //0.0だったらたまにうまくいかなかったので-0.00001にする
    if (a >= -0.00001) return SDL_TRUE;
    return SDL_FALSE;
}

/**
 * @brief 点pから点vを含む平面に垂線をおろし,垂線の足(点H)の座標と点pから平面までの距離を計算する
 * 
 * @param[out] H 垂線の足の座標の出力先
 * @param v 平面上の任意の点の座標
 * @param n 平面の法線ベクトル
 * @param p 空間上の任意の点の座標
 * 
 * @return 平面からの距離
 */
float lengthPointToPlaneAndH(Vec3f *H, Vec3f v , Vec3f n , Vec3f p)
{
    //平面の方程式 ax+by+cz+d=0の係数を計算する.
    float a , b , c , d;
    a = n.x;
    b = n.y;
    c = n.z;
    d = -a * v.x - b * v.y - c * v.z;

    //法線ベクトルの大きさの二乗を計算する
    float abs_n_pow2 = a * a + b * b + c * c;
    
    //垂線の足(点H)の座標
    float t = (a * p.x + b * p.y + c * p.z + d) / abs_n_pow2;

    Vec3f temp;

    temp.x = p.x - t * n.x;
    temp.y = p.y - t * n.y;
    temp.z = p.z - t * n.z;


    if (H != NULL){
        H->x = temp.x;
        H->y = temp.y;
        H->z = temp.z;
    }
    

    //点pから平面までの距離を計算 (ベクトル pH の長さを計算)
    Vec3f pH = vecSub(temp, p);
    return vecLength(pH);

}

/**
 * @brief 点pと点v1,v2からなる直線との距離を計算する
 * 
 * @param p 空間上の点の座標
 * @param v1v2 線上の2点
 * @param[out] t 点pから直線に垂線をおろし,垂線の足がv1からv2までの線分上にあった時0.0～1.0までの値を返す
 */
float lengthPointToLine(Vec3f p , Vec3f v1 , Vec3f v2 , float *t)
{
    Vec3f v1v2 = vecSub(v2 , v1);

    //点pから直線に垂線をおろし,垂線の足を点hとしたとき,v1h = s * v1v2となるようなsを求める
    float s = vecMulIn(vecSub(p , v1) , v1v2) / pow(vecLength(v1v2) , 2);

    if (t != NULL) *t = s;

    //phベクトルの長さを返す
    return vecLength(vecSub(p , vecAdd(v1 , vecMulSca(v1v2 , s))));

}

/**
 * @brief 点hがv1～v4で構成される四角形の内部に存在するか判定する
 * 
 * @param H 判定する点(v1～v4と同一平面上にあること)
 * @param v1v2v3v4 同一平面上の四角形を形成する4つのベクトル
 * 
 * @return 点が四角形の内部にあったらSDL_TRUE そうでなければSDL_FALSE
 */
SDL_bool isPointInRect(Vec3f H , Vec3f v1 , Vec3f v2 , Vec3f v3 , Vec3f v4 , Vec3f normal)
{
    Vec3f v1v2 , v2v3 , v3v4 , v4v1;
    Vec3f v1H , v2H , v3H , v4H;
    Vec3f a , b , c , d;
    
    //辺ベクトルを生成
    v1v2 = vecSub(v2 , v1);
    v2v3 = vecSub(v3 , v2);
    v3v4 = vecSub(v4 , v3);
    v4v1 = vecSub(v1 , v4);

    //頂点から点Hまでのベクトルを生成
    v1H = vecSub(H , v1);
    v2H = vecSub(H , v2);
    v3H = vecSub(H , v3);
    v4H = vecSub(H , v4);

    //辺ベクトルと頂点から点Hまでのベクトルの外積をとる
    a = vecMulOut(v1v2 , v1H);
    b = vecMulOut(v2v3 , v2H);
    c = vecMulOut(v3v4 , v3H);
    d = vecMulOut(v4v1 , v4H);

    /*
    float hoge1 = vecMulIn(vecSub(a , v1), normal);
    float hoge2 = vecMulIn(vecSub(b , v2), normal);
    float hoge3 = vecMulIn(vecSub(c , v3), normal);
    float hoge4 = vecMulIn(vecSub(d , v4), normal);
    */

    float hoge1 = vecMulIn(a, normal);
    float hoge2 = vecMulIn(b, normal);
    float hoge3 = vecMulIn(c, normal);
    float hoge4 = vecMulIn(d, normal);

    //法線ベクトルの内積の符号を調べることで,外積(a～b)が平面のどちら側を向いているのかが分かる
    if (serchFloatSign(hoge1) == serchFloatSign(hoge2) &&
        serchFloatSign(hoge2) == serchFloatSign(hoge3) &&
        serchFloatSign(hoge3) == serchFloatSign(hoge4))
        return SDL_TRUE;
    return SDL_FALSE;
}

/**
 * @brief v1~v4からなる平面上に点pがあるかを調べる
 */
SDL_bool isPointOnPlane4(Vec3f p , Vec3f v1 , Vec3f v2 , Vec3f v3 , Vec3f v4 , Vec3f normal)
{
    Vec3f H;
    lengthPointToPlaneAndH(&H , v1 , normal , p);
    if (isPointInRect(H , v1 , v2 , v3 , v4 , normal)) return SDL_TRUE;
    return SDL_FALSE;
}


/**
 * @brief 点が円の内部にあるか判定する
 */
SDL_bool isPointInCircle(Vec3f H , Vec3f center , float radius)
{
    if (vecLength(vecSub(H , center)) <= radius) return SDL_TRUE;
    return SDL_FALSE;
}

/**
 * @brief 点がvを含む平面の上にあるか下にあるか調べる
 */
SDL_bool isPointOnPlane(Vec3f p  , Vec3f v , Vec3f n)
{
    Vec3f vp = vecSub(p , v);
    float l = vecMulIn(vp , n);
    if (l >= -0.0001f)
        return SDL_TRUE;
    return SDL_FALSE;
}

/**
 * @brief 物体が平面に当たった時の跳ね返りの速度を求める   
 * 
 * @param velocity 物体の速度ベクトル
 * @param normal 平面の法線ベクトル
 * 
 * @return 跳ね返った後の物体の速度ベクトル
 */
Vec3f calcCollisionVel(Vec3f velocity , Vec3f normal)
{
    Vec3f rtn;
    //速度ベクトルの逆ベクトル
    Vec3f velocity_inv = vecMulSca(velocity , -1.0);

    //速度ベクトルの法線ベクトル方向の成分の大きさ
    float l = vecMulIn(velocity_inv , normal);

    //跳ね返った後の速度ベクトルは入射のベクトルに法線ベクトルの2倍を足したものである
    rtn = vecAdd(velocity , vecMulSca(normal , l * 2.0));

    rtn.y *= 0.2f;

    return rtn;
}

/**
 * @brief カメラを移動する
 */
void moveCamera(Camera *camera , float dx , float dy , float dz)
{
    Vec3f translation = {dx, dy, dz};
    camera->pos = vecAdd(camera->pos, translation);
}

/**
 * @brief カメラを回転する
 */
void rotateCamera(Camera *camera, float pitch_delta, float yaw_delta)
{
    // ヨー回転 (ワールドY軸周り)
    Quaternion yaw_rot = quaternion_from_axis_angle((Vec3f){0.0f, 1.0f, 0.0f}, yaw_delta);

    // ピッチ回転 (カメラのローカルX軸周り)
    Vec3f right_axis = quaternion_rotate_vector((Vec3f){1.0f, 0.0f, 0.0f}, camera->orientation);
    Quaternion pitch_rot = quaternion_from_axis_angle(right_axis, pitch_delta);

    // 現在の回転に新しい回転を合成する
    camera->orientation = quaternion_multiply(yaw_rot, camera->orientation);
    camera->orientation = quaternion_multiply(pitch_rot, camera->orientation);

    // 数値誤差の蓄積を防ぐために正規化
    camera->orientation = quaternion_normalize(camera->orientation);
}

/**
 * @brief カメラの位置,向きをアップデートする
 */
void updateCamera(Camera *camera)
{
    // カメラの向きから前方、右ベクトルを計算
    Vec3f forward = quaternion_rotate_vector((Vec3f){0.0f, 0.0f, -1.0f}, camera->orientation);
    Vec3f right   = quaternion_rotate_vector((Vec3f){1.0f, 0.0f,  0.0f}, camera->orientation);

    // 移動量を初期化
    Vec3f move_vec = {0.0f, 0.0f, 0.0f};
    /*
    //カメラ移動
    if (gameInfo.keyNow[K_D] == SDL_TRUE){
        move_vec = vecAdd(move_vec, right);
    }
    if (gameInfo.keyNow[K_A] == SDL_TRUE){
        move_vec = vecSub(move_vec, right);
    }
    if (gameInfo.keyNow[K_W] == SDL_TRUE){
        // Y成分を無視して水平に移動
        Vec3f forward_horizontal = {forward.x, 0.0f, forward.z};
        move_vec = vecAdd(move_vec, vecNormalize(forward_horizontal));
    }
    if (gameInfo.keyNow[K_S] == SDL_TRUE){
        // Y成分を無視して水平に移動
        Vec3f forward_horizontal = {forward.x, 0.0f, forward.z};
        move_vec = vecSub(move_vec, vecNormalize(forward_horizontal));
    }
    if (gameInfo.keyNow[K_SPACE] == SDL_TRUE){
        move_vec.y += 1.0f;
    }
    if (gameInfo.keyNow[K_SHIFT] == SDL_TRUE){
        move_vec.y -= 1.0f;
    }

    
    // 移動ベクトルを正規化し、速度を乗算して移動
    if (vecLength(move_vec) > 0.0f) {
        move_vec = vecMulSca(vecNormalize(move_vec), CAMERA_V);
        moveCamera(camera, move_vec.x, move_vec.y, move_vec.z);
    }

    //視点操作
    if (gameInfo.keyNow[K_UP] == SDL_TRUE){
        rotateCamera(camera, CAMERA_SENSITIVITY, 0.0f);
    }
    if (gameInfo.keyNow[K_DOWN] == SDL_TRUE){
        rotateCamera(camera, -CAMERA_SENSITIVITY, 0.0f);
    }
    if (gameInfo.keyNow[K_RIGHT] == SDL_TRUE){
        rotateCamera(camera, 0.0f, -CAMERA_SENSITIVITY);
    }
    if (gameInfo.keyNow[K_LEFT] == SDL_TRUE){
        rotateCamera(camera, 0.0f, CAMERA_SENSITIVITY);
    }
    */

    #ifdef USE_JOY
    // ジョイスティックの入力で視点を回転（Y軸周り）
    rotateCamera(camera, 0.0f, -gameInfo.joyStickX * CAMERA_SENSITIVITY);
    rotateCamera(camera, -gameInfo.joyStickY * CAMERA_SENSITIVITY, 0.0f);

    #endif
}
