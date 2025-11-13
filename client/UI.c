#include "client.h"
#include <SDL2/SDL_image.h>

void createUIwithText(SceneKinds group , char *text , SDL_Rect dst , int fontNum , SDL_Color color)
{
    UI *rtn = (UI *)malloc(sizeof(UI));
    rtn->dst = dst;
    rtn->isDisplay = SDL_FALSE;
    rtn->group = group;
    rtn->surface = TTF_RenderUTF8_Blended(myGameManager.fonts[fontNum] , text , color);
    addListNode(myGameManager.UIList , rtn , NULL);
}

void createUIwithIMG(SceneKinds group , char *filename , SDL_Rect dst)
{
    UI *rtn = (UI *)malloc(sizeof(UI));
    rtn->dst = dst;
    rtn->isDisplay = SDL_FALSE;
    rtn->group = group;
    rtn->surface = IMG_Load(filename);
    if (!rtn->surface){
        printf("IMG_Load error: %s\n", IMG_GetError());
    }
    else
    addListNode(myGameManager.UIList , rtn , NULL);
    
}

void deleteUI(SceneKinds group)
{
    ListNode *temp = myGameManager.UIList->head;
    while (temp != NULL){
        ListNode *next = temp->next;
        UI *data = (UI*)temp->data;
        if (data->group & group){
            if(data->surface) SDL_FreeSurface(data->surface);
            free(data);
            deleteListNode(myGameManager.UIList , temp);
        }
        temp = next;
    }
}