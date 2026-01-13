/**
 * @name audio.c
 * @brief オーデイオの中身
 */
#include "client.h"
#include <stdio.h>


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

    /* ---- SEロード例 ---- */
    myGameManager.audio.se[0] =
        Mix_LoadWAV("assets/audio/se/gunshot.wav");

    if (!myGameManager.audio.se[0]) {
    printf("Load SE failed: %s\n", Mix_GetError());
}

//    myGameManager.audio.se[1] =
//        Mix_LoadWAV("../assets/audio/se/death.wav");

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


