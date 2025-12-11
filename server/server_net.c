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
static int rsock;
static int clientId = 0;
//static NetworkContainer data; //通信用の汎用データ構造体
static NetworkContainer inputBuffer[MAX_Clients]; // 各クライアントの入力データ
static int inputReceived[MAX_Clients] = {0};            // 受信したかどうか
//void packInputData(InputData *data , );
void unpackInputData(uint16_t key , uint16_t joy , uint8_t id);
void packCarInfo(Car *car , CarInfo *data);
void sendCarInfo(void);

//関数プロトタイプ宣言
void setup_server(int, u_short);
int control_requests();
void terminate_server();
void sendACK(void);


static void send_data(int id, void *data, int size);
static int receive_data(void *data, int size , struct sockaddr *src_addr , socklen_t *src_addr_len);
static void handle_error(char *message);

//サーバの初期化処理
void setup_server(int num_cl, u_short port) {
  struct sockaddr_in sv_addr;//サーバ・クライアントアドレス構造体

  fprintf(stderr, "Server setup is started.\n");

  num_clients = num_cl; //接続を受け付けるクライアント数を保持する
//ソケット作成
  rsock = socket(AF_INET, SOCK_DGRAM, 0); //TCPソケット作成
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

    myGameManager.playerNum = num_cl;

  fprintf(stderr, "Server setup is done.\n");
}

int control_requests() {
  NetworkContainer data;
  memset(&data, 0, sizeof(NetworkContainer)); //データ初期化
  
  struct sockaddr src_addr;
  socklen_t src_addr_len;

      // データ受信。戻り値が0以下ならクライアント切断かエラー
      if (receive_data(&data, sizeof(data), &src_addr, &src_addr_len) <= 0){
          fprintf(stderr, "Client disconnected. Sending quit command.\n");
          // 他のクライアントに終了を通知
          data.order = 'Q';
          //data.id = i;
          send_data(BROADCAST, &data, sizeof(data));
          return 0; // サーバー終了
      }

      //printf("data is ready. from id:%d , order:%c\n" , data.id , data.order);
      //コマンドに応じた処理
      switch (data.order) {

      case 'S': //接続要求を受け取った場合
      {
        //クライアント情報を構造体に保持
        clients[clientId].id = clientId;
        //clients[clientId].sock = rsock;
        clients[clientId].addr = *(struct sockaddr_in *)&src_addr;
        clients[clientId].addr_len = src_addr_len;
        
        printf("client %d connected\nip:%s\nport:%d\n" , clientId , inet_ntoa(clients[clientId].addr.sin_addr) , ntohs(clients[clientId].addr.sin_port));

        send_data(clientId, &num_clients, sizeof(int)); //クライアント数を送信
        send_data(clientId, &clientId, sizeof(int)); //自分のIDを送信


        clientId++;
        if (clientId == num_clients){
          printf("all client connected\n");
          SDL_Delay(300);
          setupPhysics();
          myGameManager.timer = SDL_AddTimer(1000/60.0f, update_physics , "update_physics");
        }
        }
      break;

      case 'I': //入力情報を受け取った場合
      {
        u_short id = data.id;
        printf("recieve input data from id:%d\n" , data.id);

        if (id < num_clients) {
            memcpy(&inputBuffer[id], &data, sizeof(NetworkContainer));
            
            inputReceived[id] = 1;    // 受信済フラグ
        }
        unpackInputData(data.container.inputData.keyInputs , data.container.inputData.joyKeyInputs , data.id);
        MainScene *scene = (MainScene *)myGameManager.scene;
        scene->sendInputDataPlayerNum++;

      
        if (scene->sendInputDataPlayerNum == myGameManager.playerNum){
          scene->sendInputDataPlayerNum = 0;
          }
      
        }
        break;

      case 'Q': //Qの場合
      {
        fprintf(stderr, "client[%d]: quit\n", data.id);
        send_data(BROADCAST, &data, sizeof(data)); //全クライアントへ終了コマンド送信
        myGameManager.quitRequest = SDL_TRUE;
        return 0; // サーバー終了
      }
      break;
        
      default: //無効なコマンドの場合
        fprintf(stderr, "control_requests(): %c is not a valid command.\n", data.order);
        break; 
      
    }

  return 1; // 継続
}

// 堅牢な送信処理
  void send_all(int id, void *buf, int len){
    int bytes_sent = 0;
    int result;
    while (bytes_sent < len) {
      result = sendto(rsock, buf + bytes_sent, len - bytes_sent , 0 , (struct sockaddr *)&clients[id].addr , clients[id].addr_len);
      if (result < 0) {
        handle_error("sendto() in send_all");
        return;
      }
      bytes_sent += result;
    }
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
  }

  //ブロードキャスト送信
  if (id == BROADCAST) {
    int i;
    for (i = 0; i < clientId; i++) {
      send_all(i, data, size);
    }
  } else {
    send_all(id, data, size);
  }

}

//データ受信関数
static int receive_data(void *data, int size , struct sockaddr *src_addr , socklen_t *src_addr_len) {
  
  int bytes_received = 0;
  int result;
  while (bytes_received < size) {
    result = recvfrom(rsock, data + bytes_received, size - bytes_received, 0, src_addr, src_addr_len);
    if (result < 0) {
      // read エラー
      perror("read in receive_data");
      return result;
    }
    if (result == 0) {
      // 接続が正常に閉じられた
      fprintf(stderr, "Connection closed by client.\n");
      return bytes_received; // 途中までしか読めなかったが、読めた分だけ返す
    }
    bytes_received += result;
  }
  return bytes_received;
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
  //全クライアントソケットの書き込みチャネルを閉じる
  /*
  for (i = 0; i < num_clients; i++) {
    shutdown(clients[i].sock, SHUT_WR);
  }
  */
  
  //念のため、データがクライアントに届くのを少し待つ
  sleep(1);

  //全クライアントソケットをクローズ
  /*
  for (i = 0; i < num_clients; i++) {
    close(clients[i].sock);
  }
  */
  fprintf(stderr, "All connections are closed.\n");
  exit(0);
}

void unpackInputData(uint16_t key , uint16_t joy , uint8_t id)
{
  //前フレームの状態を保存
      for (int i = 0; i < KEY_MAX; i++) {
        myGameManager.clients[id].keyPrev[i] = myGameManager.clients[id].keyNow[i];
      }
      #ifdef USE_JOY
      for (int i = 0; i < JOY_KEY_MAX; i++) {
        myGameManager.clients[id].joyBottonPrev[i] = myGameManager.clients[id].joyBotton[i];
      }
      #endif
      
      //現在のフレームの状態を復元
      for (int i = KEY_MAX - 1; i >= 0; i--) {
        key >>= 1;
        myGameManager.clients[id].keyNow[i] = (key & 1);
      }
      #ifdef USE_JOY
      for (int i = JOY_KEY_MAX - 1; i >= 0; i--) {
        data.joyKeyInputs >>= 1;
        myGameManager.clients[id].joyBotton[i] = (joy & 1);
      }
      #endif
}

void packCarInfo(Car *car , CarInfo *data)
{
  Rectangler *r = car->collisionBox->data.rectangler;

  data->carX = car->center.x;
  data->carY = car->center.y;
  data->carZ = car->center.z;

  data->q = car->q;

  data->HP = car->hp;

  if (myGameManager.clients[car->id].keyNow[K_ENTER]){
    data->isShotGun = 1;
  }

  #ifdef USE_JOY
  if (myGameManager.clients[car->id].joyBotton[JOY_B]){
    data->isShotGun = 1;
  }
  #endif
}

void sendCarInfo(void)
{
  MainScene *scene = (MainScene *)myGameManager.scene;
  ListNode *node;
  foreach(node , scene->cars){
    NetworkContainer con;
    memset(&con, 0, sizeof(NetworkContainer));

    Car *car = (Car *)node->data;
    packCarInfo(car , &con.container.carInfo);
    con.id = car->id;
    con.order = 'C';

    send_data(BROADCAST , &con , sizeof(con));
  }
}
