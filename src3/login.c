#include "factory.h"

int login(pNode_t pDelete, char *name, pDir current)
{
    Train_t train;
    UserInfo_t info;
    int ret;
    char timeNow[50];
    //验证登录
    //接收用户名
    recvCycle(pDelete->new_fd, &train.dataLen, 4);
    recvCycle(pDelete->new_fd, train.buf, train.dataLen);
    strcpy(name, train.buf);
    //查数据库对应用户名的salt, 并发给客户端
    memset(&info, 0, sizeof(info));
    ret = login_query(train.buf, &info);
    train.dataLen = strlen(info.salt);
    strcpy(train.buf, info.salt);
    ret = send(pDelete->new_fd, &train, 4 + train.dataLen, 0);

    //接收crypt
    recvCycle(pDelete->new_fd, &train.dataLen, 4);
    ret = recvCycle(pDelete->new_fd, train.buf, train.dataLen);
    //printf("recv crypt_code = %s\n", train.buf);
    if(-1 == ret) return -1;
    int flag;
    if(!strcmp(train.buf, info.crypt_code)){
        gettime(timeNow, 50);
        //printf("client id = %lu, connection succeeded.\n", pthread_self());
        printf("%s login succeeded.\n", name);
        flag = 1;
        train.dataLen = sizeof(flag);
        memcpy(train.buf, &flag, train.dataLen);
        send(pDelete->new_fd, &train, 4 + train.dataLen, 0);
        memset(current, 0, sizeof(Dir));
        getcwd(current->pathNow, sizeof(current->pathNow));
        //printf("getcwd ok!\n");
        sprintf(current->pathNow, "%s%s%s%s%s", current->pathNow, "/", "file/", name, "/");
        //printf("current->pathNow = %s\n", current->pathNow);
    }
    else{
        gettime(timeNow, 50);
        //printf("client id = %lu, connection failed.\n", pthread_self());
        printf("%s login failed.\n", name);
        flag = 0;
        train.dataLen = sizeof(flag);
        memcpy(train.buf, &flag, train.dataLen);
        send(pDelete->new_fd, &train, 4 + train.dataLen, 0);
        return -1;
    }
    return 0;
}

