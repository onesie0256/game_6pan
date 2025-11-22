#pragma once

/* vector.c */

/**
 * @brief 3次元ベクトル
 */
typedef struct vec3f_t{
    float x, y, z;
}Vec3f;

/* list.c */

/**
 * @brief リスト構造体のノード
 */
typedef struct ListNode_t {
    char key[20];               //キー
    void *data;                 //データへのポインタ
    struct ListNode_t *next;    //次のノードへのポインタ
    struct ListNode_t *prev;    //前のノードへのポインタ
} ListNode;

/**
 * @brief リスト構造体
 */
typedef struct list_t {
    ListNode *head; //リストの先頭ノードへのポインタ
    int count;      //リスト内の要素数
}List;

/* polygone.c */

/**
 * @brief 図形の種類
 */
typedef enum{
    PT_RECTANGLER, //直方体
    PT_SPHERE,     //球体
    PT_PLANE4,      //4つの頂点からなる平面
    PT_DISC,        //円盤
    PT_OBJ,         //.obj形式
    PT_CYLINDER    //円錐
}PolygonType;

/**
 * @brief 頂点の位置
 * 
 * @details 左から順にx,y,z.
 * また,R...右 L...左 U...上 D...下 F...手前 B...裏
 */
typedef enum{
    PP_LDF, //手前左下
    PP_RDF, //手前右下
    PP_RUF, //手前右上
    PP_LUF, //手前左上
    PP_RDB, //裏右下
    PP_LDB, //裏左下
    PP_LUB, //裏左上
    PP_RUB //裏右上
}PointPosition;


/**
 * @brief 直方体の構造体
 */
typedef struct{
    Vec3f vertex[8]; //頂点
    Vec3f normals[6]; //法線ベクトル

    //6面4角形xyzの3要素(6*4*3 = 72)
    float vertAry[72];
    float normAry[72];
    float colorAry[72];

    int pitch; //x軸周りの回転[°]
    int roll;  //z軸方向の回転
    int yaw;   //y軸方向の回転

    Vec3f color;

    int surfaceOnBallNum; //ボールが直方体のどの面の上にあるか
}Rectangler;

/**
 * @brief 球体の構造体
 */
typedef struct sphere_t{
    Vec3f center; //中心
    float radius; //半径
    Vec3f color;  //色

    GLUquadric *qobj; //gluで定義される四角形オブジェクト
}Sphere;

/**
 * @brief 平面のタイプ
 */
typedef enum{
    PLT_Wall=0,
    PLT_Floar=1,
}PlaneType;

/**
 * @brief 平面の構造体
 */
typedef struct{
    Vec3f vertex[4]; //頂点
    Vec3f normal;    //法線ベクトル
    Vec3f color;     //色
    PlaneType type;  //タイプ

    float vertAry[12];
    float normAry[12];
    float colorAry[12];

    int pitch; //x軸周りの回転[°]
    int roll;  //z軸方向の回転
    int yaw;   //y軸方向の回転

    SDL_bool isBallOnPlane; //ボールがこの平面の表側にあるか裏側にあるか
}Plane4;

/**
 * @brief 円盤の構造体
 */
typedef struct{
    Vec3f center; //中心
    float radius; //半径
    Vec3f color;  //色
    Vec3f normal; //法線ベクトル


    GLUquadric *qobj; //gluで定義される四角形オブジェクト
}Disc;

/**
 * @brief objファイル
 */
typedef struct obj_t{
    Vec3f vertex[30000];    //頂点
    int vertNum;            //頂点数

    float vertAry[30000];   //頂点(openGL方式)
    int vertAryNum;         //頂点数(openGL方式)

    Vec3f normal[30000];    //法線ベクトル
    int normalNum;          //法線ベクトル数

    float normAry[30000];   //法線ベクトル
    int normAryNum;         //法線ベクトル


    Vec3f texCoord[30000];  //テクスチャマッピング
    int texCoordNum;        //テクスチャマッピング数

    float texCoordAry[30000];   //テクスチャマッピング
    int texCoordAryNum;     //テクスチャマッピング数

    GLuint texture;         //テクスチャID
    float texW;             //テクスチャ横幅
    float texH;             //テクスチャ縦幅

    Vec3f CoordOffSet;      //初期座標
    Vec3f coord;            //座標
    Vec3f scale;            //スケール

    int pitch;              //ピッチ角[°]
    int roll;               //ロール角[°]
    int yaw;                //ヨー角[°]

    int pitchOffset;        //初期ピッチ角[°]
    int rollOffset;         //初期ロール角[°]
    int yawOffset;          //初期ヨー角[°]
}Obj;

/**
 * @brief 円柱形
 */
typedef struct{
    Vec3f center;   //中心座標
    float radius;   //半径
    float height;   //高さ
    Vec3f color;    //カラー
    int yaw;        //ヨー角

    GLUquadric *qobj;
}Cylinder;



/**
 * @brief ポリゴン構造体のメンバー(様々な形状の図形のデータ)
 */
typedef union {
    Rectangler *rectangler;
    Sphere *sphere;
    Plane4 *plane4;
    Disc *disc;
    Obj *obj;
    Cylinder *cylinder;
}PolygonData;

/**
 * @brief リストに格納されるポリゴンのデータ
 */
typedef struct polygon_t{

    SDL_bool isDisplay; //表示するかどうか
    PolygonType type;   //図形の種類

    PolygonData data;   //図形のデータ
}Polygon;

/* quaternion.c */
/**
 * @brief クォータニオン
 */
typedef struct {
    float w, x, y, z;
} Quaternion;

/* space.c */
/**
 * @brief カメラの情報
 */
typedef struct camera_t{
    Vec3f pos;              //カメラの存在する座標
    Quaternion orientation; // 注視点(center)は pos と orientation から動的に計算される
}Camera;