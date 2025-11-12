/**
 * @file list.c
 * @brief リスト構造体の実装
 */

#include "3DEngine.h"


/**
 * @brief 双方向リストを生成する
 * 
 * @return リスト構造体へのポインタ
 */
List* createList(void)
{
    List* rtn = (List*)malloc(sizeof(List));

    rtn->count = 0;
    rtn->head = NULL;

    return rtn;
}

/**
 * @brief リストにノードを追加する
 * 
 * @param list リストの構造体のポインタ
 * @param data 任意の型のデータへのポインタ
 * @param key 検索キー(検索キーが要らなかったらNULLを入力)
 */
void addListNode(List *list, void *data , char *key)
{
    //ノードを初期化
    ListNode *node = (ListNode*)malloc(sizeof(ListNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;

    //リストが空の場合
    if (list->head == NULL){
        list->head = node;
        list->count++;
        return;
    }

    //キーを設定
    if (key != NULL)
    strcpy(node->key , key);

    //リストの最後尾まで
    ListNode *temp = list->head;
    while (temp->next != NULL){
        temp = temp->next;
    }
    //リストの最後尾に追加
    temp->next = node;
    node->prev = temp;
    list->count++;
}

/**
 * @brief keyでリストのノードを検索する
 * 
 * @param list 検索対象のリスト
 * @param key 検索キー
 * 
 * @return もし対象のリストが発見出来たらそのリストへのポインタを返す.見つからなかったらNULLを返す.
 */
void *serchListNode(List *list , char *key)
{
    void *rtn = NULL;
    if (list->head == NULL || list == NULL) return rtn;
    ListNode *node = list->head;

    while (node != NULL)
    {
        if (!strcmp(node->key , key)){
            return node->data;
        }

        node = node->next;
    }

    return rtn;
}

/**
 * @brief リストからノードを1つ削除する
 * 
 * @param list リストの構造体のポインタ
 * @param node 対象のノードのポインタ
 */
void deleteListNode(List *list, ListNode *node)
{
    //ヘッダの指すノードが対象だった場合
    if (node->prev == NULL){
        list->head = node->next;
    }else{
        //前ノードのnext付け替え
        node->prev->next = node->next;
    }
    //次ノードのprev付け替え
    if (node->next != NULL){
        node->next->prev = node->prev;
    }

    free(node);
    list->count--;
}

/**
 * @brief リストから全てのノードを削除する
 * 
 * @param list リストの構造体のポインタ
 */
void deleteListNodeAll(List *list)
{
    ListNode *temp = list->head;
    while (temp != NULL){
        ListNode *next = temp->next;
        free(temp);
        temp = next;
    }
    list->head = NULL;
    list->count = 0;
}


/**
 * @brief リストを解放する
 * 
 * @param list リストの構造体のポインタ
 */
void destroyList(List *list)
{
    deleteListNodeAll(list);
    free(list);
}