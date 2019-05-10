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
    int flag, ret;
    recvCycle(socketFd, &ret, 4);
    recvCycle(socketFd, &flag, ret);
    if(flag == 1){
        system("clear");
        printf("login succeeded.\n");
    }
    else{
        printf("login failed.\n");
        return -1;
    }
    return 0;
}


int getcd(int socketFd){
    Train_t train;
    char buf[1000];
    int ret;
    int flag;
    int option;
    memset(buf, 0, sizeof(buf));
    memset(&train, 0, sizeof(train));
    scanf("%s", buf);
    char tempName[100] = {0};
    strcpy(tempName, buf);
    option = 1;
    train.dataLen = sizeof(int);
    memcpy(train.buf, &option, train.dataLen);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");

    train.dataLen = strlen(buf);
    strcpy(train.buf, buf);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);//目录名
    ERROR_CHECK(ret, -1, "send");
    
    //printf("name : %s send\n", train.buf);
    memset(&train, 0, sizeof(train));
    recvCycle(socketFd, &train.dataLen, 4);
    recvCycle(socketFd, &flag, train.dataLen);
    //printf("train.dataLen = %d\n", train.dataLen);
    //recvCycle(socketFd, &flag, train.dataLen);
    //printf("recv flag = %d\n", flag);
    if(flag == -1){
        printf("Failed! Already root directory.\n");
        printf("---------------------------------\n");
    }
    else if(flag == -2){
        printf("Failed! %s is not directory.\n", tempName);
        printf("---------------------------------\n");
    }
    else{
     //   memset(&train, 0, sizeof(train));
     //   recvCycle(socketFd, &train.dataLen, 4);
     //   recvCycle(socketFd, buf, train.dataLen);
     //   setbuf(stdin, NULL);
     //   puts(buf);
        printf("cd finish.\n");
        printf("---------------------------------\n");
    }
    return 0;
}

int getpwd(int socketFd){
    char buf[1000];
    Train_t train;
    int ret;
    memset(buf, 0, sizeof(buf));
    memset(&train, 0, sizeof(train));
    int option;
    option = 6;
    train.dataLen = sizeof(int);
    memcpy(train.buf, &option, train.dataLen);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    recvCycle(socketFd, &train.dataLen, 4);
    recvCycle(socketFd, buf, train.dataLen);
    puts(buf);
    printf("---------------------------------\n");
    return 0;
}
