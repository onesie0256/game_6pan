#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "server.h"
#include <sys/types.h>
#include <sys/select.h>

typedef unsigned short u_short;

//グローバル変数
static SERVER_CLIENT clients[MAX_Clients]; //接続してきたクライアント情報を格納
static int num_clients; //接続クライアント数
static int num_socks; //最大ソケット番号＋1
static fd_set mask; //ファイルディスクリプタ集合
//static NetworkContainer data; //通信用の汎用データ構造体
static NetworkContainer inputBuffer[MAX_Clients]; // 各クライアントの入力データ
static int inputReceived[MAX_Clients] = {0};            // 受信したかどうか

//関数プロトタイプ宣言
void setup_server(int, u_short);
int control_requests();
void terminate_server();


static void send_data(int id, void *data, int size);
static int receive_data(int, void *, int);
static void handle_error(char *message);

//サーバの初期化処理
void setup_server(int num_cl, u_short port) {
  int rsock, sock = 0;
  struct sockaddr_in sv_addr, cl_addr;//サーバ・クライアントアドレス構造体

  fprintf(stderr, "Server setup is started.\n");

  num_clients = num_cl; //接続を受け付けるクライアント数を保持する
//ソケット作成
  rsock = socket(AF_INET, SOCK_STREAM, 0); //TCPソケット作成
  if (rsock < 0) { //ソケット作成失敗
    handle_error("socket()");
  }
  fprintf(stderr, "sock() for request socket is done successfully.\n");
//サーバアドレス設定
  sv_addr.sin_family = AF_INET; 
  sv_addr.sin_port = htons(port); //ポート番号をネットワークバイトオーダーに変換
  sv_addr.sin_addr.s_addr = INADDR_ANY; //任意のアドレスからの接続を受け付ける
//ポート再利用設定
  int opt = 1;
  setsockopt(rsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//ソケットにアドレスを割り当てる
  if (bind(rsock, (struct sockaddr *)&sv_addr, sizeof(sv_addr)) != 0) {
    handle_error("bind()");
  }
  fprintf(stderr, "bind() is done successfully.\n");
//クライアントからの接続要求を待機状態にする
  if (listen(rsock, num_clients) != 0) {
    handle_error("listen()");
  }
  fprintf(stderr, "listen() is started.\n");
//クライアントからの接続要求を受け付ける
  int i, max_sock = 0;
  socklen_t len;
  char src[MAX_LEN_ADDR];//クライアントipアドレス文字列格納用
  for (i = 0; i < num_clients; i++) { //指定されたクライアント数だけ接続を受け付ける
    len = sizeof(cl_addr);
    sock = accept(rsock, (struct sockaddr *)&cl_addr, &len); //接続要求受け付け
    if (sock < 0) { //接続要求受け付け失敗
      handle_error("accept()");
    }
    if (max_sock < sock) { //最大ソケット番号を更新
      max_sock = sock;
    }

    //クライアント情報を構造体に保持
    clients[i].id = i;
    clients[i].sock = sock;
    clients[i].addr = cl_addr;
    memset(src, 0, sizeof(src));//ipアドレスを文字列に変換して表示
    inet_ntop(AF_INET, (struct sockaddr *)&cl_addr.sin_addr, src, sizeof(src));
    fprintf(stderr, "Client %d is accepted (address=%s, port=%d).\n", i, src, ntohs(cl_addr.sin_port));

  }
//接続要求受付用ソケットは不要になるのでクローズ
  close(rsock);
  int j;
  for (i = 0; i < num_clients; i++) {
    //全体のクライアント数を送信
    send_data(i, &num_clients, sizeof(int));
    //自分ののIDを送信
    send_data(i, &i, sizeof(int));
    for (j = 0; j < num_clients; j++) {
      send_data(i, &clients[j], sizeof(SERVER_CLIENT));//クライアント情報を送信
    }
  }

  num_socks = max_sock + 1;
  FD_ZERO(&mask); //マスクを初期化
  FD_SET(0, &mask); //標準入力を監視対象に設定 

  for (i = 0; i < num_clients; i++) {
    FD_SET(clients[i].sock, &mask); //クライアントソケットを監視対象に設定
  }
  fprintf(stderr, "Server setup is done.\n");
}

int control_requests() {
  NetworkContainer data;
  fd_set read_flag = mask; //コピー作成
  memset(&data, 0, sizeof(NetworkContainer)); //データ初期化

  fprintf(stderr, "select() is started.\n");
  //読み込み可能データが来たか監視
  if (select(num_socks, (fd_set *)&read_flag, NULL, NULL, NULL) == -1) {
    handle_error("select()");
  }
  

  int i, result = 1;
  //全クライアントソケットをチェック
  for (i = 0; i < num_clients; i++) {
    if (FD_ISSET(clients[i].sock, &read_flag)) {
      receive_data(i, &data, sizeof(data)); //データ受信
      printf("data is ready. from id:%d , order:%c\n" , data.id , data.order);
      //コマンドに応じた処理
      switch (data.order) {
      case 'I': //入力情報を受け取った場合
      {
      u_short id = data.id;
      printf("recieve input data from id:%d\n" , data.id);


       if (id < num_clients) {
           memcpy(&inputBuffer[id], &data, sizeof(NetworkContainer));
           inputReceived[id] = 1;    // 受信済フラグ
       }
    }
       // 全員から揃ったかチェック
       int all = 1;
       for (int j = 0; j < num_clients; j++) {
           if (!inputReceived[j]) {
               all = 0;
               
           }
       }

       if (all) {
           // まとめて送信
           for (int j = 0; j < num_clients; j++) {
               send_data(BROADCAST, &inputBuffer[j], sizeof(NetworkContainer));
               inputReceived[j] = 0; // 次フレームのためにリセット
               printf("send input data to id:%d\n" , j);
           }
       }

        //入力データを全クライアントへ送信
        //send_data(BROADCAST, &data, sizeof(data));
        result = 1;
        break;


      case 'Q': //Qの場合
        fprintf(stderr, "client[%d]: quit\n", clients[i].id);
        send_data(BROADCAST, &data, sizeof(data)); //全クライアントへ終了コマンド送信
        result = 0; //終了
        break;
      default: //無効なコマンドの場合
        fprintf(stderr, "control_requests(): %c is not a valid command.\n", data.order);
        exit(1); //無効であることを示す
    }
    }
  }

  return result;
}

//データ送信関数
static void send_data(int id, void *data, int size) {
  //不正なクライアントIDチェック
  
  if ((id != BROADCAST) && (0 > id || id >= num_clients)) {
    fprintf(stderr, "send_data(): client id is illeagal.\n");
    exit(1);
  }
  
  //不正なデータチェック
  if ((data == NULL) || (size <= 0)) {
    fprintf(stderr, "send_data(): data is illeagal.\n");
    exit(1);
  }
//ブロードキャスト送信
  if (id == BROADCAST) {
    int i;
    for (i = 0; i < num_clients; i++) {
      if (write(clients[i].sock, data, size) < 0) {
        handle_error("write()");
      }
    }
  } else {
    if (write(clients[id].sock, data, size) < 0) {
      handle_error("write()"); //特定クライアントへ送信
    }
  }
}

//データ受信関数
static int receive_data(int id, void *data, int size) {
  if ((id != BROADCAST) && (0 > id || id >= num_clients)) {
    fprintf(stderr, "receive_data(): client id is illeagal.\n");
    exit(1);
  }
  if ((data == NULL) || (size <= 0)) {
    fprintf(stderr, "receive_data(): data is illeagal.\n");
  	exit(1);
  }

  return read(clients[id].sock, data, size);
}

//エラーハンドリング関数
//エラーメッセージを表示し終了
static void handle_error(char *message) {
  perror(message);
  fprintf(stderr, "%d\n", errno);
  exit(1);
}

//サーバ終了処理
void terminate_server(void) {
  int i;
  //全クライアントソケットをクローズ
  for (i = 0; i < num_clients; i++) {
    close(clients[i].sock);
  }
  fprintf(stderr, "All connections are closed.\n");
  exit(0);
}
