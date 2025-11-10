#include "3DEngine.h"
#include <math.h>

#define DEG_TO_RAD 0.017453


/**
 * @brief ベクトルの加算を行う
 * 
 * @param a ベクトル
 * @param b ベクトル
 * 
 * @return 加算されたベクトル
 */
Vec3f vecAdd(Vec3f a , Vec3f b)
{
    Vec3f rtn;

    rtn.x = a.x + b.x;
    rtn.y = a.y + b.y;
    rtn.z = a.z + b.z;

    return rtn;
}

/**
 * @brief ベクトルの減算を行う
 * 
 * @param a 引かれるベクトル
 * @param b 引く
 * 
 * @return 減算されたベクトル
 */
Vec3f vecSub(Vec3f a , Vec3f b)
{
    Vec3f rtn;

    rtn.x = a.x - b.x;
    rtn.y = a.y - b.y;
    rtn.z = a.z - b.z;

    return rtn;
}

/**
 * @brief ベクトルの内積を計算する
 * 
 * @param a ベクトル
 * @param b ベクトル
 * 
 * @return 内積
 */
float vecMulIn(Vec3f a , Vec3f b)
{
    float rtn;

    rtn = a.x * b.x + a.y * b.y + a.z * b.z;

    return rtn;
}

/**
 * @brief ベクトルの外積を計算する
 * 
 * @param a ベクトル
 * @param b ベクトル
 * 
 * @return 外積
 */
Vec3f vecMulOut(Vec3f a , Vec3f b)
{
    Vec3f rtn;

    rtn.x = a.y * b.z - a.z * b.y;
    rtn.y = a.z * b.x - a.x * b.z;
    rtn.z = a.x * b.y - a.y * b.x;

    if (-0.0001 < rtn.x && rtn.x < 0.0001) rtn.x = 0.0f;
    if (-0.0001 < rtn.y && rtn.y < 0.0001) rtn.y = 0.0f;
    if (-0.0001 < rtn.z && rtn.z < 0.0001) rtn.z = 0.0f;

    return rtn;
}

/**
 * @brief ベクトルをスカラー倍する
 * 
 * @param a ベクトル
 * @param b 倍率
 * 
 * @return スカラー倍されたベクトル
 */
Vec3f vecMulSca(Vec3f a , float b)
{
    Vec3f rtn;

    rtn.x = a.x * b;
    rtn.y = a.y * b;
    rtn.z = a.z * b;

    return rtn;
}

/**
 * @brief 頂点を原点を中心にx軸周りに回転させる
 * 
 * @param vert 対象のベクトル
 * @param pitch 回転角度.単位は[°]
 * 
 * @return 計算結果
 */
Vec3f rotateX(Vec3f vert , int pitch)
{
    Vec3f rtn;

    float pitch_rad = -pitch * DEG_TO_RAD;

    rtn.x = vert.x;
    rtn.y = vert.y * cos(pitch_rad) - vert.z * sin(pitch_rad);
    rtn.z = vert.y * sin(pitch_rad) + vert.z * cos(pitch_rad);

    return rtn;
}

/**
 * @brief 頂点を原点を中心にz軸周りに回転させる
 * 
 * @param vert 対象のベクトル
 * @param roll 回転角度.単位は[°]
 * 
 * @return 計算結果
 */
Vec3f rotateZ(Vec3f vert , int roll)
{
    Vec3f rtn;

    float roll_rad = roll * DEG_TO_RAD;

    rtn.x = vert.x * cos(roll_rad) - vert.y * sin(roll_rad);
    rtn.y = vert.x * sin(roll_rad) + vert.y * cos(roll_rad);
    rtn.z = vert.z;

    return rtn;
}

/**
 * @brief 頂点を原点を中心にy軸周りに回転させる
 * 
 * @param vert 対象のベクトル
 * @param yaw 回転角度.単位は[°]
 * 
 * @return 計算結果
 */
Vec3f rotateY(Vec3f vert , int yaw)
{
    Vec3f rtn;

    float yaw_rad = yaw * DEG_TO_RAD;

    rtn.x = vert.x * cos(yaw_rad) + vert.z * sin(yaw_rad);
    rtn.y = vert.y;
    rtn.z = -vert.x * sin(yaw_rad) + vert.z * cos(yaw_rad);

    return rtn;
}

/**
 * @brief 頂点を原点を中心に回転させる
 * 
 * @param vert 対象の頂点
 * @param pitch ピッチ角(x軸周りの回転).単位は[°]
 * @param roll ローリング角(y軸周りの回転).単位は[°]
 * @param yaw ヨーイング角(z軸周りの回転).単位は[°]
 */
Vec3f rotateXYZ(Vec3f vert , int pitch , int roll , int yaw)
{
    Vec3f rtn , rotatedZ , rotatedY;

    rotatedZ = rotateZ(vert , roll);
    rotatedY = rotateY(rotatedZ , yaw);
    rtn = rotateX(rotatedY , pitch);

    //調整
    if (-0.0001 < rtn.x && rtn.x < 0.0001) rtn.x = 0.0f;
    if (-0.0001 < rtn.y && rtn.y < 0.0001) rtn.y = 0.0f;
    if (-0.0001 < rtn.z && rtn.z < 0.0001) rtn.z = 0.0f;

    return rtn;
}

/**
 * @brief ベクトルの大きさを計算する
 * 
 */
float vecLength(Vec3f a)
{
    float rtn;

    rtn = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);

    return rtn;
}

/**
 * @brief ベクトルの正規化を行う
 * 
 * @param a 正規化するベクトル
 * 
 * @return 正規化されたベクトル
 */
Vec3f vecNormalize(Vec3f a)
{
    Vec3f rtn;
    float length = vecLength(a);

    //長さが0の場合
    if (length == 0.0) return a;

    //xyzそれぞれの成分をベクトルの長さで割る
    rtn.x = a.x / length;
    rtn.y = a.y / length;
    rtn.z = a.z / length;

    return rtn;
}

/**
 * @brief 直線ab,acからなる法線ベクトルを計算する
 * 
 * @param a 頂点の座標
 * @param b 頂点の座標
 * @param c 頂点の座標
 * 
 * @return 法線ベクトル
 */
Vec3f calcNormalVec(Vec3f a , Vec3f b , Vec3f c)
{
    Vec3f ab , ac , rtn;

    ab = vecSub(b , a);
    ac = vecSub(c , a);

    //外積を計算する
    rtn = vecMulOut(ab , ac);

    //ベクトルを正規化する
    rtn = vecNormalize(rtn);

    return rtn;
}

/**
 * @brief ベクトル成分の符号を調べる
 * 
 * @param a ベクトル
 * 
 * @return 符号が正なら1,負なら0.ただし,最小ビットからx,y,z成分の情報が格納されている
 */
uint8_t serchVecSign(Vec3f a)
{
    uint8_t rtn = 0;

    if (a.x >= 0.000) rtn += 1;
    if (a.y >= 0.000) rtn += 2;
    if (a.z >= 0.000) rtn += 4;

    return rtn;
}

/**
 * @brief 点を任意の点を中心に回転させる
 * 
 * @param target 回転させる点
 * @param center 回転の中心
 * @param pitch ピッチ角(x軸周りの回転).単位は[°]
 * @param roll ローリング角(y軸周りの回転).単位は[°]
 * @param yaw ヨーイング角(z軸周りの回転).単位は[°]
 * 
 * @return 回転後の座標
 */
Vec3f roteteP(Vec3f target , Vec3f center , int pitch , int roll , int yaw)
{
    Vec3f rtn;

    rtn = vecSub(target , center);
    rtn = rotateXYZ(rtn , pitch , roll , yaw);
    rtn = vecAdd(rtn , center);

    return rtn;
}