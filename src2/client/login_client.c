#include "function.h"

int login_client(int socketFd)
{
    Train_t train;
    char username[20] = {0};
    char passwd[20] = {0};
    printf("please input your username:\n");
    scanf("%s", username);
    printf("please input your passwd:\n");
    scanf("%s", passwd);

    //发用户名
    train.dataLen = strlen(username);
    strcpy(train.buf, username);
    send(socketFd, &train, 4 + train.dataLen, 0);
    
    //接收salt，并计算crpty_code
    char crypt_code[512] = {0};
    recvCycle(socketFd, &train.dataLen, 4);
    recvCycle(socketFd, train.buf, train.dataLen);
    strcpy(crypt_code, crypt(passwd, train.buf));

    //发送crypt
    train.dataLen = strlen(crypt_code);
    strcpy(train.buf, crypt_code);
    send(socketFd, &train, 4 + train.dataLen, 0);
    
    //接收登录标志
    recvCycle(socketFd, &train, 4);
    if(train.dataLen == 1){
        printf("验证成功\n");
    }
    else{
        printf("验证失败\n");
        return -1;
    }
    return 0;
}

