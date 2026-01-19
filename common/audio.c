/**
 * @name audio.c
 * @brief オーデイオの中身
 */
#include "common.h"
#include <stdio.h>
#include <math.h>

/* SE ファイルパス対応表 */
static const char *seFilePath[SE_MAX] = {
    [SE_GUNSHOT_PISTOL]  = "assets/audio/se/Gunshot_pistol.wav",
    [SE_GUNSHOT_SHOTGUN] = "assets/audio/se/Gunshot_Shotgun.mp3",
    [SE_GUNSHOT_SNIPER]  = "assets/audio/se/Gunshot_sniper.wav",
    [SE_RELOAD]          = "assets/audio/se/Gun_Cock.wav",
    [SE_ACCEL]           = "assets/audio/se/Car_Accel.mp3",
    [SE_EXPLOSION]       = "assets/audio/se/Explosion.mp3",
    [SE_CURSOR]          = "assets/audio/se/Cursor.mp3",
    [SE_KEBOARD]         = "assets/audio/se/Keyboard.wav",
    [SE_COUNTDOWN]       = "assets/audio/se/CountDown.wav",
    [SE_START]           = "assets/audio/se/Start.wav",
};

const SeID gunShotSeTable[] = {
    [Pistol]  = SE_GUNSHOT_PISTOL,
    [Shotgun] = SE_GUNSHOT_SHOTGUN,
    [Sniper]  = SE_GUNSHOT_SNIPER,
};

static const BgmID sceneBgmTable[Scene_Max] = {
    [Scene_Title]  = BGM_TITLE,
    [Scene_Wait]   = BGM_TITLE,
    [Scene_Main]   = BGM_MAIN,
    [Scene_Result] = BGM_RESULT,
};

/*
void Audio_OnSceneChanged(uint8_t newScene)
{
    AudioManager *audio = &myGameManager.audio;

    if (newScene >= Scene_Max) return;

    BgmID next = sceneBgmTable[newScene];

    if (audio->currentBgmID == next) return;

    if (next == BGM_NONE) {
        Mix_HaltMusic();
        audio->currentBgmID = BGM_NONE;
        return;
    }

    Mix_PlayMusic(audio->bgm[next], -1);
    audio->currentBgmID = next;
}
*/

void Audio_OnSceneChanged(uint8_t newScene)
{
    AudioManager *audio = &myGameManager.audio;

    printf("[Audio] SceneChanged: %d\n", newScene);

    if (newScene >= Scene_Max) {
        printf("[Audio] invalid scene\n");
        return;
    }

    BgmID next = sceneBgmTable[newScene];
    printf("[Audio] nextBgm=%d current=%d\n", next, audio->currentBgmID);

    if (audio->currentBgmID == next) {
        printf("[Audio] same BGM, skip\n");
        return;
    }

    if (next == BGM_NONE) {
        printf("[Audio] stop music\n");
        Mix_HaltMusic();
        audio->currentBgmID = BGM_NONE;
        return;
    }

    if (audio->bgm[next] == NULL) {
        printf("[Audio] BGM[%d] is NULL : %s\n",
               next, Mix_GetError());
        return;
    }

    if (Mix_PlayMusic(audio->bgm[next], -1) < 0) {
        printf("[Audio] Mix_PlayMusic error: %s\n",
               Mix_GetError());
        return;
    }

    audio->currentBgmID = next;
    printf("[Audio] play BGM %d\n", next);
}

/**
 * @brief オーデイオの初期化
 */
int Audio_Init(void)
{
    if (!(Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3))) {
    printf("Mix_Init error: %s\n", Mix_GetError());
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
    {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        return -1;
    }

    Mix_AllocateChannels(16);



    /* ---- BGMロード ---- */
/*    
    myGameManager.audio.bgm[BGM_TITLE] =
        Mix_LoadMUS("assets/audio/bgm/title.mp3");

    if (!myGameManager.audio.bgm[BGM_TITLE]) {
    printf("Load TITLE BGM failed: %s\n", Mix_GetError());
    }
*/

//    myGameManager.audio.bgm[BGM_WAIT] =
//        Mix_LoadMUS("../assets/audio/bgm/wait.ogg");

    myGameManager.audio.bgm[BGM_MAIN] =
        Mix_LoadMUS("assets/audio/bgm/main.mp3");

    if (!myGameManager.audio.bgm[BGM_MAIN]) {
    printf("Load MAIN BGM failed: %s\n", Mix_GetError());
    }


//    myGameManager.audio.bgm[BGM_RESULT] =
//        Mix_LoadMUS("../assets/audio/bgm/result.ogg");

    /* ---- SEロード ---- */
for (int i = SE_GUNSHOT_PISTOL; i < SE_MAX; i++) {

    if (seFilePath[i] == NULL) {
        myGameManager.audio.se[i] = NULL;
        continue;
    }

    myGameManager.audio.se[i] = Mix_LoadWAV(seFilePath[i]);

    if (!myGameManager.audio.se[i]) {
        printf("Load SE failed (%d): %s\n", i, Mix_GetError());
    }
}

        /* currentBgmID 初期化 */
    myGameManager.audio.currentBgmID = BGM_NONE;

    return 0;
}

/**
 * @brief オーデイオの終了処理
 */
void Audio_Quit(void)
{
    for (int i = 0; i < BGM_MAX; i++)
    {
        if (myGameManager.audio.bgm[i])
        {
            Mix_FreeMusic(myGameManager.audio.bgm[i]);
            myGameManager.audio.bgm[i] = NULL;
        }
    }

    for (int i = 0; i < SE_MAX; i++)
    {
        if (myGameManager.audio.se[i])
        {
            Mix_FreeChunk(myGameManager.audio.se[i]);
            myGameManager.audio.se[i] = NULL;
        }
    }

    Mix_CloseAudio();
}

/* =========================
   BGM
   ========================= */
void Audio_PlayBGM(BgmID id)
{
    /* 同じBGMなら何もしない */
    if (myGameManager.audio.currentBgmID == id)
        return;

    Mix_HaltMusic();

    if (id != BGM_NONE && myGameManager.audio.bgm[id])
    {
        Mix_PlayMusic(myGameManager.audio.bgm[id], -1);
    }

    myGameManager.audio.currentBgmID = id;
}


void Audio_StopBGM(void)
{
    Mix_HaltMusic();
    myGameManager.audio.currentBgmID = BGM_NONE;
}

void Audio_FadeOutBGM(int ms)
{
    Mix_FadeOutMusic(ms);
    myGameManager.audio.currentBgmID = BGM_NONE;
}


/* =========================
   SE
   ========================= */
void Audio_PlaySE(int seID)
{
    if (seID < 0 || seID >= SE_MAX)
        return;

    if (!myGameManager.audio.se[seID])
        return;

    Mix_PlayChannel(-1, myGameManager.audio.se[seID], 0);
}

/* =========================
   3D SE（距離減衰）
   ========================= */

static float Vec3_Distance(Vec3f a, Vec3f b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

/* リスナー = 自プレイヤー */
static Vec3f Audio_GetListenerPos(void)
{
    if (myGameManager.scene) {
        MainScene *scene = (MainScene*)myGameManager.scene;
        if (scene->myCar) {
            return scene->myCar->center;
        }
    }
    return (Vec3f){0,0,0};
}

void Audio_PlaySE_3D(int seID, Vec3f soundPos)
{
    if (seID < 0 || seID >= SE_MAX) return;
    if (!myGameManager.audio.se[seID]) return;

    Vec3f listener = Audio_GetListenerPos();
    float dist = Vec3_Distance(soundPos, listener);

    const float MAX_DIST = 2000.0f;  // この距離で無音

    if (dist >= MAX_DIST) return;

    float volumeRate = 1.0f - (dist / MAX_DIST);
    int volume = (int)(volumeRate * MIX_MAX_VOLUME);

    int ch = Mix_PlayChannel(-1, myGameManager.audio.se[seID], 0);
    if (ch >= 0) {
        Mix_Volume(ch, volume);
    }
}


