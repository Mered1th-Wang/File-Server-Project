#include "factory.h"

int login(pNode_t pDelete, char *name)
{
    Train_t train;
    UserInfo_t info;
    int ret;
    char timeNow[50];
    //验证登录
    //接收用户名
    recvCycle(pDelete->new_fd, &train.dataLen, 4);
    recvCycle(pDelete->new_fd, &train.buf, train.dataLen);
    strcpy(name, train.buf);
    //查数据库对应用户名的salt, 并发给客户端
    memset(&info, 0, sizeof(info));
    ret = login_query(train.buf, &info);
    train.dataLen = strlen(info.salt);
    strcpy(train.buf, info.salt);
    ret = send(pDelete->new_fd, &train, 4 + train.dataLen, 0);

    //接收crypt
    recvCycle(pDelete->new_fd, &train.dataLen, 4);
    recvCycle(pDelete->new_fd, &train.buf, train.dataLen);

    if(!strcmp(train.buf, info.crypt_code)){
        gettime(timeNow, 50);
        //printf("client id = %lu, connection succeeded.\n", pthread_self());
        printf("%s login succeeded.\n", name);
        train.dataLen = 1;
        send(pDelete->new_fd, &train, 4, 0);
    }
    else{
        gettime(timeNow, 50);
        //printf("client id = %lu, connection failed.\n", pthread_self());
        printf("%s login failed.\n", name);
        train.dataLen = 0;
        send(pDelete->new_fd, &train, 4, 0);
    }
    return 0;
}

