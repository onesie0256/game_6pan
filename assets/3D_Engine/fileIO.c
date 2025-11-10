#include "3DEngine.h"

#define LINE_MAX 100

/**
 * @brief .dataファイルからステージの情報を読み込む
 * 
 * @param [out] list 出力先のリスト
 * @param filename 読み込むファイル名
 * 
 * @return 読み込みに成功したらSDL_TRUE,失敗したらSDL_FALSE
 */
SDL_bool loadStageData(List *list , const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("could not open file %s\n" , filename);
        return SDL_FALSE;
    }

    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, fp) != NULL) {
        //先頭が#の行は飛ばす
        if (line[0] == '#') {
            continue;
        }

        switch (line[0] - '0')
        {
        case PT_RECTANGLER:
        {
            Vec3f coord;
            Vec3f size;
            Vec3f color;
            int pitch;
            int roll;
            int yaw;
            // sscanf の戻り値を確認
            int items_read = sscanf(line , "0,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d" ,
                    &coord.x , &coord.y , &coord.z ,
                    &size.x , &size.y , &size.z ,
                    &color.x , &color.y , &color.z ,
                    &pitch , &roll , &yaw);
            if (items_read == 12) { // 12個の項目が読み込まれるはず
                if(createRectangler(coord , size , color , pitch , roll , yaw , list) == NULL) return SDL_FALSE;
            }
            break;
        }

        case PT_SPHERE:
        {
            Vec3f center;
            float radius;
            Vec3f color;
            int items_read = sscanf(line , "1,%f,%f,%f,%f,%f,%f,%f" ,
                    &center.x , &center.y , &center.z ,
                    &radius ,
                    &color.x , &color.y , &color.z);
            if (items_read == 7) { // 7個の項目が読み込まれるはず
                if (createSphere(center , radius , color , list) == NULL) return SDL_FALSE;
            }
            break;
        }

        case PT_PLANE4:
        {
            Vec3f coord;
            float sizeX;
            float sizeY;
            Vec3f color;
            int pitch;
            int roll;
            int yaw;
            int plane_type_int;
            int items_read = sscanf(line , "2,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d" ,
                    &coord.x , &coord.y , &coord.z ,
                    &sizeX , &sizeY ,
                    &color.x , &color.y , &color.z ,
                    &pitch , &roll , &yaw , &plane_type_int);
            if (items_read == 12) { // 12個の項目が読み込まれるはず
                if (createPlane4(coord , sizeX , sizeY , color , pitch , roll , yaw , (PlaneType)plane_type_int , list) == NULL) return SDL_FALSE;
            }
            break;
        }

        case PT_DISC:
        {
            Vec3f coord;
            float radius;
            Vec3f color;
            int items_read = sscanf(line , "3,%f,%f,%f,%f,%f,%f,%f" ,
                    &coord.x , &coord.y , &coord.z ,
                    &radius ,
                    &color.x , &color.y , &color.z);
            if (items_read == 7) { // 7個の項目が読み込まれるはず
                if (createDisc(coord , radius , color , list) == NULL) return SDL_FALSE;
            }
            break;
        }

        case PT_OBJ:
        {
            Vec3f coordOffset;
            Vec3f scale;
            int pitchOffset;
            int rollOffset;
            int yawOffset;
            char temp_filename[LINE_MAX];
            char obj_filename[LINE_MAX];
            char texture_filename[LINE_MAX];
            int items_read = sscanf(line , "4,%f,%f,%f,%f,%f,%f,%d,%d,%d,%s" ,
                    &coordOffset.x , &coordOffset.y , &coordOffset.z ,
                    &scale.x , &scale.y , &scale.z ,
                    &pitchOffset , &rollOffset , &yawOffset,
                    temp_filename);
            
            if (items_read == 10) { // 10個の項目が読み込まれるはず
                int strNum = 0;
                char* pstr = &temp_filename[0];
                while (*pstr != ','){
                    obj_filename[strNum] = *pstr;
                    pstr++;
                    strNum++;
                }
                obj_filename[strNum] = '\0';
                pstr++;
                strNum = 0;

                while (*pstr != '\0'){
                    texture_filename[strNum] = *pstr;
                    pstr++;
                    strNum++;
                }
                texture_filename[strNum] = '\0';

                if (createObj(obj_filename , texture_filename , coordOffset , scale , pitchOffset , rollOffset , yawOffset , list) == NULL) return SDL_FALSE;
            }
            break;
        }
        
        default:
            break;
        }
    }

    fclose(fp);
    return SDL_TRUE;
}

/**
 * @brief .objファイルから3dデータを読み込む
 * 
 * @param obj_filename .objファイル名
 * @param texture_filename テクスチャ用画像ファイル名
 * @param [out] obj 出力先のポインタ
 * 
 * @return 読み込みに成功したらSDL_TRUE,失敗したらSDL_FALSE
 */
SDL_bool loadOBJ(const char* obj_filename , const char* texture_filename ,  Obj *obj)
{
    FILE* fp = fopen(obj_filename, "r");
    if (fp == NULL) {
        printf("could not open file %s\n" , obj_filename);
        return SDL_FALSE;
    }

    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, fp) != NULL)
    {
        switch (line[0])
        {
        case '#':
            break;
        
        case 'v':
            if (line[1] == ' '){
                Vec3f v;
                sscanf(line , "v %f %f %f" , &v.x , &v.y , &v.z);
                obj->vertex[obj->vertNum] = v;
                obj->vertNum++;
            }
            else if (line[1] == 't'){
                Vec3f v;
                sscanf(line , "vt %f %f" , &v.x , &v.y);
                obj->texCoord[obj->texCoordNum] = v;
                obj->texCoordNum++;
            }
            else if (line[1] == 'n'){
                Vec3f v;
                sscanf(line , "vn %f %f %f" , &v.x , &v.y , &v.z);
                obj->normal[obj->normalNum] = v;
                obj->normalNum++;
            }
            break;

        case 'f':
        {
            int v[4], t[4], n[4]; // 頂点, テクスチャ, 法線のインデックス
            int items_read = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                                    &v[0], &t[0], &n[0],
                                    &v[1], &t[1], &n[1],
                                    &v[2], &t[2], &n[2],
                                    &v[3], &t[3], &n[3]);

            int vertex_count = 0;
            if (items_read == 12) { // 四角形の場合
                vertex_count = 4;
            } else if (items_read == 9) { // 三角形の場合
                vertex_count = 3;
            } else {
                // 対応していない形式か、読み込みエラー
                continue;
            }

            for (int i = 0; i < vertex_count; i++) {
                // 頂点座標をコピー
                obj->vertAry[obj->vertAryNum + 0] = obj->vertex[v[i] - 1].x;
                obj->vertAry[obj->vertAryNum + 1] = obj->vertex[v[i] - 1].y;
                obj->vertAry[obj->vertAryNum + 2] = obj->vertex[v[i] - 1].z;
                obj->vertAryNum += 3;

                // テクスチャ座標をコピー
                obj->texCoordAry[obj->texCoordAryNum + 0] = obj->texCoord[t[i] - 1].x;
                obj->texCoordAry[obj->texCoordAryNum + 1] = obj->texCoord[t[i] - 1].y;
                obj->texCoordAryNum += 2;

                // 法線ベクトルをコピー
                obj->normAry[obj->normAryNum + 0] = obj->normal[n[i] - 1].x;
                obj->normAry[obj->normAryNum + 1] = obj->normal[n[i] - 1].y;
                obj->normAry[obj->normAryNum + 2] = obj->normal[n[i] - 1].z;
                obj->normAryNum += 3;
            }
            break;
        }

        default:
            break;
        }
    }

    fclose(fp);
    return SDL_TRUE;
}