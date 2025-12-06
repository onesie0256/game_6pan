#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>



#include "client.h"

static int sockfd = -1;

/*
int connectServer(char *serverName)
{
    struct sockaddr_in server_addr;
    struct hostent *host;

    
    if (host = gethostbyname(serverName) == NULL) {
        herror("gethostbyname");
        return 0;
    }

    // ソケット作成
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 0;
    }

    // サーバー情報設定
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr, host->h_addr_list[0], host->h_length);
    server_addr.sin_port = htons(50000);  // 任意のポート（サーバーと合わせる）

    // 接続試行
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        sockfd = -1;
        return 0;
    }

    fprintf(stderr,"Connected\n");
    return 1;
}
*/


/**
 * @brief データ送信
 * @param container 送信する NetworkContainer 構造体
 * @return 成功: 1, 失敗: 0
 */
/*
int sendData(NetworkContainer *container)
{
    if (sockfd < 0) return 0;

    ssize_t n = send(sockfd, container, sizeof(NetworkContainer), 0);
    if (n < 0) {
        perror("send");
        return 0;
    }

    return 1;
}
*/

/**
 * @brief データ受信
 * @param container 受信先 NetworkContainer 構造体
 * @return 成功: 1, 失敗または切断: 0
 */
/*
int recvData(NetworkContainer *container)
{
    if (sockfd < 0) return 0;

    ssize_t n = recv(sockfd, container, sizeof(NetworkContainer), 0);
    if (n <= 0) {
        if (n < 0) perror("recv");
        else printf("Server disconnected.\n");
        close(sockfd);
        sockfd = -1;
        return 0;
    }

    return 1;
}
*/

/*
void CloseSoc(void)
{
NetworkContainer pack;
memset(&pack, 0, sizeof(pack));

pack.id = myGameManager.myID;
pack.order = 'Q';   // 終了命令をセット
send(sockfd, &pack, sizeof(pack), 0);

printf("...Connection closed\n");
close(sockfd);

}
*/
