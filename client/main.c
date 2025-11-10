#include "client.h"

GameManager myGameManager;

int main(int argc , char* argv[])
{

  //引数チェック
    if(argc == 1){
        char* str_localhost = "localhost";
    	myGameManager.serverName = str_localhost;
    }
    else if(argc == 2){
    	myGameManager.serverName = argv[1];
    }
    else{
		fprintf(stderr, "Usage: %s, Cannot find a Server Name.\n", argv[0]);
		return -1;
    }

  //サーバーへ接続
  if(connectServer(myGameManager.serverName) == 0){
    printf("cannot connect server\n");
  }

  //ゲームループ
    int endFlag = 1;
    while(endFlag){
      endFlag = gameLoop();
    }

    return 0;
}
