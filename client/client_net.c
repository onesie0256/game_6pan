#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include <errno.h>

#include "client.h"

typedef unsigned int u_int;

//GameManager myGameManager;   

#define DEF_PORT 50100
#define MAX_LEN_NAME 100
#define MAX_LEN_ADDR 32
#define MAX_LEN_MESSAGE 256
#define BROADCAST -1


static int n_clients; //クライアント数
static int my_id; //自分のID
static int sock; //ソケット
static int num_sock; // 
static fd_set mask; 
static struct sockaddr_in sv_addr; //サーバアドレス構造体
//static Client clients[MAX_Clients]; //クライアント情報

void setup_client(char *serverName, uint16_t port); 
int control_requests();
void terminate_client();
void send_input_data(void);
void receive_input_data(void);
void send_Quit(void);

static int in_command(void);
static int exe_command(void);
static void send_data(void *, int);
static int receive_data(void *, int);
static void handle_error(char *);

void unpackCar(CarInfo data , uint8_t id);

//クライアント起動後の動き
void setup_client(char *server_name, uint16_t port) {

  struct hostent *server; //サーバ情報

//起動直後　Trying to connect serverとポート番号を表示
  fprintf(stderr, "Trying to connect server %s (port = %d).\n", server_name, port);
  if ((server = gethostbyname(server_name)) == NULL) { 
    handle_error("gethostbyname()");
  }

  //ソケット作成
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {//ソケット作成失敗
    handle_error("socket()");
  }

  //サーバアドレス設定
  sv_addr.sin_family = AF_INET;
  sv_addr.sin_port = htons(port);//ポート番号をネットワークバイトオーダーに変換
  sv_addr.sin_addr.s_addr = *(u_int *) server ->h_addr_list[0];//IPアドレス設定

  NetworkContainer data;
  memset(&data, 0, sizeof(NetworkContainer)); //データ初期化
  data.order = COMMAND_SYN; //接続要求

  send_data(&data, sizeof(NetworkContainer)); //データをサーバへ送信



  fprintf(stderr, "Waiting for other clients...\n");
  //クライアント数を受信
  receive_data(&n_clients, sizeof(int));
  // GameManager にプレイヤー数を保存
  myGameManager.playerNum = (uint8_t)n_clients;
  //クライアント数を表示
  fprintf(stderr, "Number of clients = %d.\n", n_clients);
  //自分のIDを受信
  receive_data(&my_id, sizeof(int));
  fprintf(stderr, "Your ID = %d.\n", my_id);
  myGameManager.myID = my_id;
}

int control_requests () {
    int result = exe_command(); //コマンド実行
  return result;
}

void send_input_data(void) {
  NetworkContainer data;
  memset(&data, 0, sizeof(NetworkContainer)); //データ初期化

  data.id = my_id; //自分のIDを設定
  data.order = COMMAND_INPUT_DATA; //入力情報コマンド
  data.container.inputData.keyInputs = 0;
  for (int i = 0; i < KEY_MAX; i++) {
    if (myGameManager.keyNow[i]) {
      data.container.inputData.keyInputs |= 1;//ビット列に圧縮
    }
    data.container.inputData.keyInputs <<= 1;
  }
  
  data.container.inputData.joyKeyInputs = 0;
  #ifdef USE_JOY
  for (int i = 0; i < JOY_KEY_MAX; i++) {
    if (myGameManager.joyBotton[i]) {
      data.container.inputData.joyKeyInputs |= 1;//ビット列に圧縮
    }
    data.container.inputData.joyKeyInputs <<= 1;
  }
  data.stickX = myGameManager.StickX;
  data.stickY = myGameManager.StickY;
  #endif
  
  //データをサーバへ送信
  send_data(&data, sizeof(NetworkContainer));
  //printf("send input data\n");
}   

void receive_input_data(void) {
  NetworkContainer data;
  memset(&data, 0, sizeof(NetworkContainer));
  //ソケットからデータ受信
  receive_data(&data, sizeof(data));

  //受信したデータをクライアント情報に反映
  if (data.id < n_clients) {


    // 1. まず前フレームを保存
    for (int i = 0; i < KEY_MAX; i++) {
        myGameManager.clients[data.id].keyPrev[i] = myGameManager.clients[data.id].keyNow[i];
    }
    #ifdef USE_JOY
    for (int i = 0; i < JOY_KEY_MAX; i++) {
        myGameManager.clients[data.id].joyBottonPrev[i] = myGameManager.clients[data.id].joyBotton[i];
    }
    #endif

    // 2. 次に今フレームの状態を復元する
    for (int i = KEY_MAX -1 ; i >= 0 ; i--) {
        data.container.inputData.keyInputs >>= 1;
        myGameManager.clients[data.id].keyNow[i] = (data.container.inputData.keyInputs & 1);
    }
    #ifdef USE_JOY
    for (int i = JOY_KEY_MAX - 1; i >= 0; i--) {
        data.container.inputData.joyKeyInputs >>= 1;
        myGameManager.clients[data.id].joyBotton[i] = (data.container.inputData.joyKeyInputs & 1);
    }
    #endif
  }
}

static int in_command() { 
  NetworkContainer data;
  char com;
  memset(&data, 0, sizeof(NetworkContainer)); //データ初期化
  com = getchar(); //コマンド読み込み
  while(getchar()!='\n'); //改行を読み飛ばし

  switch (com) {
    //Qと入力されたらQUIT_COMMANDを送信
  case COMMAND_QUIT:
    data.order = COMMAND_QUIT;
    data.id = my_id;
    send_data(&data, sizeof(NetworkContainer)); //データをサーバへ送信
    break;
  default:
  //無効なコマンドとして表示
    fprintf(stderr, "%c is not a valid command.\n", com);
  }

  return 1;//続行
}

//サーバまたは他クライアントからのメッセージ受信処理
static int exe_command() {
  NetworkContainer data;
  int result = 1;
  memset(&data, 0, sizeof(NetworkContainer));
  //ソケットからデータ受信
  int recv_size = receive_data(&data, sizeof(data));
  if (recv_size <= 0) {
      // 接続が切れたか、エラーが発生した
      fprintf(stderr, "Server disconnected.\n");
      myGameManager.quitRequest = SDL_TRUE;
      return 0; // 終了
  }
//コマンドに応じた処理
  switch (data.order) {

  case COMMAND_INPUT_DATA: //入力情報の場合
    //受信した入力データを処理
    if (data.id < n_clients) {
      printf("recieve input data from id:%d\n" , data.id);
      //前フレームの状態を保存
      for (int i = 0; i < KEY_MAX; i++) {
        myGameManager.clients[data.id].keyPrev[i] = myGameManager.clients[data.id].keyNow[i];
      }
      #ifdef USE_JOY
      for (int i = 0; i < JOY_KEY_MAX; i++) {
        myGameManager.clients[data.id].joyBottonPrev[i] = myGameManager.clients[data.id].joyBotton[i];
      }
      #endif
      
      //現在のフレームの状態を復元
      for (int i = KEY_MAX - 1; i >= 0; i--) {
        data.container.inputData.keyInputs >>= 1;
        myGameManager.clients[data.id].keyNow[i] = (data.container.inputData.keyInputs & 1);
      }
      #ifdef USE_JOY
      for (int i = JOY_KEY_MAX - 1; i >= 0; i--) {
        data.container.inputData.joyKeyInputs >>= 1;
        myGameManager.clients[data.id].joyBotton[i] = (data.container.inputData.joyKeyInputs & 1);
      }
      #endif
      MainScene *scene = (MainScene *)myGameManager.scene;
      scene->sendInputDataPlayerNum++;
    }
    result = 1;
    break;


  case COMMAND_QUIT: //Qの場合
    fprintf(stderr, "client[%d] sent quit order.\n", data.id);
    myGameManager.quitRequest = SDL_TRUE;
    result = 0; //終了
    break;

  case COMMAND_CARINFO: //Cの場合
  {
    unpackCar(data.container.carInfo , data.id);
    MainScene *scene = (MainScene *)myGameManager.scene;
    if (scene == NULL){
      printf("exe_command: failed to get scene\n");
      break;
    }
  
    scene->sendCarInfoPlayerNum++;
  }
    break;

  case COMMAND_ACK:
    myGameManager.ackRequest++;
    break;
  
  case COMMAND_CLIENT_DATA:
    myGameManager.clients[data.id].gunId = data.container.clientData.gunId;
    //printf("id:%d gun id:%d\n" , data.id , myGameManager.clients[data.id].gunId);
    break;
  
  case COMMAND_COUNT:
  {
    MainScene *scene = (MainScene *)myGameManager.scene;
    scene->count = data.container.clientData.gunId;
  }
    break;


  default: //無効なコマンドの場合
    fprintf(stderr, "exe_command(): %c is not a valid command.\n", data.order);
    //exit(1); 
  }

  return result;
}

void send_Quit(void)
{
    NetworkContainer data;
    memset(&data, 0, sizeof(NetworkContainer)); //データ初期化

    data.id = my_id; //自分のIDを設定
    data.order = COMMAND_QUIT; //入力情報コマンド

    send_data(&data, sizeof(NetworkContainer));
}


//データ送信関数
static void send_data(void *data, int size) {
  if ((data == NULL) || (size <= 0)) {
    fprintf(stderr, "send_data(): data is illeagal.\n");
    exit(1);
  }

  int bytes_sent = 0;
  int result;
  while (bytes_sent < size) {
    result = sendto(sock, (char *)data + bytes_sent, size - bytes_sent , 0 , (struct sockaddr *)&sv_addr, sizeof(sv_addr));
    if (result < 0) {
      handle_error("write()");
      return; // エラーが発生したら中断
    }
    bytes_sent += result;
  }
}

//データ受信関数
static int receive_data(void *data, int size) {
  if ((data == NULL) || (size <= 0)) {
    fprintf(stderr, "receive_data(): data is illeagal.\n");
    exit(1);
  }

  int bytes_received = 0;
  int result;
  while (bytes_received < size) {
    result = recvfrom(sock, (char *)data + bytes_received, size - bytes_received , 0 , NULL, NULL);
    if (result < 0) {
      // EAGAIN/EWOULDBLOCKは、非ブロッキングソケットではエラーではない
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // データがまだ到着していない。少し待ってリトライ。
        SDL_Delay(1);
        continue;
      }
      // その他の致命的なエラー
      handle_error("read()");
      return result;
    }
    if (result == 0) {
      // 接続が閉じられた
      fprintf(stderr, "Connection closed by peer during receive.\n");
      return bytes_received; // 読めた分だけ返す
    }
    bytes_received += result;
  }
  return bytes_received;
}

//エラーハンドリング関数
//エラーメッセージを表示し終了
static void handle_error(char *message) {
  perror(message);
  fprintf(stderr, "%d\n", errno); //エラー番号表示
  exit(1);
}

//終了
void terminate_client() {
  fprintf(stderr, "Connection is closed.\n");
  close(sock); //ソケットクローズ
}

void unpackCar(CarInfo data , uint8_t id)
{
  MainScene *scene = myGameManager.scene;
  if (scene == NULL){
    printf("unpackCar: failed to get scene\n");
    return;
  }
  if (scene->cars == NULL){
    printf("unpackCar: failed to get cars\n");
    return;
  }

  Car *car = getCarFromId(scene->cars , id);

  if (car == NULL){
    printf("unpackCar: failed to get car\n");
    return;
  }

  car->shotFlag = data.param & 1;
  car->isOnGround = data.param & 2;

  car->center = (Vec3f){data.carX , data.carY , data.carZ};
  car->hp = data.HP;

  car->q_pre = car->q;
  car->q = data.q;
}

void waitUntilAck(void)
{
  while (myGameManager.ackRequest == 0)
  {
    ;
  }
  myGameManager.ackRequest = 0;
}

void send_gunId(void)
{
  NetworkContainer data;
  memset(&data, 0, sizeof(NetworkContainer)); //データ初期化
  data.order = COMMAND_GUN;
  data.id = my_id;
  data.container.clientData.gunId = myGameManager.gunId;
  send_data(&data, sizeof(data));
}