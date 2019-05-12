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

    //printf("name = %s\n", name);
    if(strlen(name) >= 21){
        childlogin(name, current);
        return 0;
    }

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

        char token[100] = {0};
        strcpy(token, name);
        create_token(token, timeNow);

        update_dir(name, current->pathNow);


        //printf("send token = %s\n", token);
        //发送token
        memset(&train, 0, sizeof(train));
        train.dataLen = strlen(token);
        strcpy(train.buf, token);
        send(pDelete->new_fd, &train, 4 + train.dataLen, 0);
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

int create_token(char *token, char *timeNow){
    char name[50] = {0};
    strcpy(name, token);
    sprintf(token, "%s%s%s", token, "abc", timeNow);
    update_mysql(token, name);
    return 0;
}


//子线程token验证登录
int childlogin(char *name, pDir current)
{
    //char timeNow[50];
    //验证登录
    //接收token
    //  recvCycle(pDelete->new_fd, &train.dataLen, 4);
    //  recvCycle(pDelete->new_fd, train.buf, train.dataLen);
    //  strcpy(name, train.buf);
    //gettime(timeNow, 50);
    //printf("client id = %lu, connection succeeded.\n", pthread_self());
    //printf("%s login succeeded.\n", name);
    //getcwd(current->pathNow, sizeof(current->pathNow));
    //printf("getcwd ok!\n");

    //sprintf(current->pathNow, "%s%s%s%s%s", current->pathNow, "/", "file/", "wj", "/");
    //printf("current->pathNow = %s\n", current->pathNow);
    return 0;
}




