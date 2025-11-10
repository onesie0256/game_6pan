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