/*****************************************************************
 * ファイル名 : server_net.c
 * 機能       : クライアントとのTCP通信確立・送受信
 *****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>

#include "common.h"

int gClientNum;

#define SERVER_PORT 50000
#define BACKLOG 2  // 同時接続待ち上限
#define MAX_Clients 2

static int listen_sock = -1;
static int client_sock = -1;

static GameManager	gClients[MAX_Clients];	/* ���饤����� */

static fd_set	gMask;					/* select()�ѤΥޥ��� */
static int	gWidth;						/* gMask��Υ����å����٤��ӥåȿ� */


/**
 * @brief クライアント接続を待ち、1人受け付ける
 * @return 成功: 1, 失敗: 0
 */
int connectClient(void)
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // ソケット作成
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        return 0;
    }

    // ソケット再利用設定
    int yes = 1;
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt");
        close(listen_sock);
        return 0;
    }

    // サーバーアドレス設定
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  
    server_addr.sin_port = htons(SERVER_PORT);

    // バインド
    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(listen_sock);
        return 0;
    }

    // 接続待ち
    if (listen(listen_sock, BACKLOG) < 0) {
        perror("listen");
        close(listen_sock);
        return 0;
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // クライアント受け入れ
    client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
    if (client_sock < 0) {
        perror("accept");
        close(listen_sock);
        return 0;
    }

    printf("Client connected \n");
    return 1;
}

/**
 * @brief クライアントにデータを送信する
 */
int sendData(NetworkContainer *container)
{
    if (client_sock < 0) return 0;

    ssize_t n = send(client_sock, container, sizeof(NetworkContainer), 0);
    if (n < 0) {
        perror("send");
        return 0;
    }

    return 1;
}

/**
 * @brief クライアントからデータを受信する
 */
int recvData(NetworkContainer *container)
{
    if (client_sock < 0) return 0;

    ssize_t n = recv(client_sock, container, sizeof(NetworkContainer), 0);
    if (n <= 0) {
        if (n < 0) perror("recv");
        else printf("Client disconnected.\n");
        close(client_sock);
        client_sock = -1;
        return 0;
    }

    return 1;
}

int SendRecvManager(void)
{
    fd_set readOK = gMask;
    int i, endFlag = 1;

    if (select(gWidth, &readOK, NULL, NULL, NULL) < 0)
        return endFlag;

    for (i = 0; i < gClientNum; i++) {
        if (FD_ISSET(gClients[i].fd, &readOK)) {
            NetworkContainer pack;
            int n = read(gClients[i].fd, &pack, sizeof(pack));
            if (n <= 0) {
                // クライアントが強制切断した場合も同じ処理
                printf("Client %d disconnected unexpectedly.\n", i);
                CloseClient(i);
                continue;
            }

            if (pack.order == 'Q') {
                printf("Client %d requested quit.\n", i);
                CloseClient(i);
            } else {
                // 他のコマンド処理
                ExecuteCommand(pack.order, i);
            }
        }
    }
    return endFlag;
}


void CloseClient(int i)
{
    close(gClients[i].fd);        // クライアントソケットを閉じる
    FD_CLR(gClients[i].fd, &gMask); // select マスクから外す

    // クライアントリストから削除
    gClients[i].fd = -1;
    gClients[i].name[0] = '\0';

    // 必要なら他のクライアントに通知
    SendAllName(); // 名前リスト更新など
}