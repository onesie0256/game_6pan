/**
 * @file quaternion.c
 * @brief クォータニオン関連の実装
 */
#include "3DEngine.h"

/**
 * @brief 単位クォータニオン（無回転状態）を返す
 * 
 * @return 単位クォータニオン
 */
Quaternion quaternion_identity(void) {
    return (Quaternion){1.0f, 0.0f, 0.0f, 0.0f};
}

/**
 * @brief 指定された軸と角度（度数法）からクォータニオンを生成する
 * 
 * @param  axis 回転軸を表す単位ベクトル 例:x軸周りなら(1.0,0.0,0.0)
 * @param angle_deg 回転角[°]
 */
Quaternion quaternion_from_axis_angle(Vec3f axis, float angle_deg) {
    float angle_rad = angle_deg * DEG_TO_RAD;
    float half_angle = angle_rad * 0.5f;
    float s = sinf(half_angle);
    
    Vec3f normalized_axis = vecNormalize(axis);
    
    Quaternion q;
    q.w = cosf(half_angle);
    q.x = normalized_axis.x * s;
    q.y = normalized_axis.y * s;
    q.z = normalized_axis.z * s;
    
    return q;
}

/**
 * @brief 2つのクォータニオンを乗算（回転を合成）する
 */
Quaternion quaternion_multiply(Quaternion q1, Quaternion q2) {
    Quaternion result;
    result.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
    result.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    result.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    result.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    return result;
}

/**
 * @brief クォータニオンを正規化する
 */
Quaternion quaternion_normalize(Quaternion q) {
    float mag = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    if (mag > 0.0001f) {
        q.w /= mag;
        q.x /= mag;
        q.y /= mag;
        q.z /= mag;
    } else {
        return quaternion_identity();
    }
    return q;
}

/**
 * @brief クォータニオンでベクトルを回転させる
 */
Vec3f quaternion_rotate_vector(Vec3f v, Quaternion q) {
    Quaternion p = {0.0f, v.x, v.y, v.z};
    Quaternion q_conj = {q.w, -q.x, -q.y, -q.z}; // 共役クォータニオン
    Quaternion temp = quaternion_multiply(q, p);
    Quaternion result_q = quaternion_multiply(temp, q_conj);
    return (Vec3f){result_q.x, result_q.y, result_q.z};
}

/**
 * @brief クォータニオンをオイラー角(pitch, yaw, roll)に変換する
 */
Vec3f quaternion_to_euler(Quaternion q) {
    Vec3f euler; // x: pitch, y: yaw, z: roll

    // pitch (x-axis rotation)
    float sinp_cosy = 2 * (q.w * q.x + q.y * q.z);
    float cosp_cosy = 1 - 2 * (q.x * q.x + q.y * q.y);
    euler.x = atan2f(sinp_cosy, cosp_cosy);

    // yaw (y-axis rotation)
    /*float siny = 2 * (q.w * q.y - q.z * q.x);
    
    if (fabsf(siny) >= 1) {
        // ジンバルロック状態
        euler.y = copysignf(3.1415 / 2, siny); 
    } else {
        euler.y = asinf(siny);
    }
    */
   float tany = 2 * (q.x * q.z + q.y * q.w);
   float tanx = 2 * (q.x * q.x + q.w * q.w) - 1;
   euler.y = atan2(tany, tanx);
   //printf("tany: %f , cosy: %f , euler.y: %f\n" , tany , tanx , euler.y);


    // roll (z-axis rotation)
    float sinr_cosy = 2 * (q.w * q.z + q.x * q.y);
    float cosr_cosy = 1 - 2 * (q.y * q.y + q.z * q.z);
    euler.z = atan2f(sinr_cosy, cosr_cosy);

    return euler;
}

/**
 * @brief 2つの単位ベクトル間の回転を表すオイラー角を導出する
 * 
 * @param v1 回転元となる単位ベクトル
 * @param v2 回転先となる単位ベクトル
 * @return オイラー角 (x: pitch, y: yaw, z: roll)
 */
Vec3f euler_from_vectors(Vec3f v1, Vec3f v2) {
    // ベクトルを正規化する (念のため)
    v1 = vecNormalize(v1);
    v2 = vecNormalize(v2);

    // 回転軸を計算する (外積)
    Vec3f axis = vecMulOut(v1, v2);
    float axis_length = vecLength(axis);

    // ベクトルが同一直線上にある場合 (外積がゼロベクトルになる)
    if (axis_length < 0.0001f) {
        // ベクトルが平行 (同じ方向)
        if (vecMulIn(v1, v2) > 0.9999f) {
            return (Vec3f){0.0f, 0.0f, 0.0f}; // 回転なし
        } else { // ベクトルが反平行 (反対方向)
            // 180度の回転。任意の軸 (例えばY軸) の周りに回転させる
            // Y軸と平行な場合はX軸を使う
            if (fabsf(v1.y) < 0.9999f) {
                axis = (Vec3f){0.0f, 1.0f, 0.0f};
            } else {
                axis = (Vec3f){1.0f, 0.0f, 0.0f};
            }
            return quaternion_to_euler(quaternion_from_axis_angle(axis, 180.0f));
        }
    }

    axis = vecNormalize(axis); // 軸を正規化する

    // 回転角度を計算する (内積)
    float angle_rad = acosf(vecMulIn(v1, v2));
    float angle_deg = angle_rad * RAD_TO_DEG;

    // 軸-角度からクォータニオンに変換する
    Quaternion q = quaternion_from_axis_angle(axis, angle_deg);

    // クォータニオンからオイラー角に変換する
    return quaternion_to_euler(q);
}

/**
 * @brief ベクトルをクォータニオンで回転させる
 * 
 * @param v 回転させるベクトル
 * @param angle 回転角度
 * @param axis 回転軸のベクトル
 * 
 * @return 回転後のベクトル
 */
Vec3f rotateVecWithQuaternion(Vec3f v , float angle , Vec3f axis)
{
    Quaternion q = quaternion_from_axis_angle(axis , angle);
    return quaternion_rotate_vector(v , q);
}