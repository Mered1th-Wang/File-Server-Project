#include "function.h"

int login_client(int socketFd, char *token)
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

        memset(&train, 0 ,sizeof(train));
        //接收token
        recvCycle(socketFd, &train.dataLen, 4);
        recvCycle(socketFd, train.buf, train.dataLen);
        strcpy(token, train.buf);
        printf("recv token = %s\n", token);
    }
    else{
        printf("login failed.\n");
        return -1;
    }
    return 0;
}


int login_childclient(int socketFd, char *token)
{
    int ret;
    Train_t train;
    //发送token值
    train.dataLen = strlen(token);
    strcpy(train.buf, token);
    send(socketFd, &train, 4 + train.dataLen, 0);
    ret = query_token(token);
    
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

int gets(int socketFd, char *buf){
    int flag, ret;
    Train_t train;
    flag = 4;
    train.dataLen = sizeof(flag);
    memcpy(train.buf, &flag, train.dataLen);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);//发命令
    ERROR_CHECK(ret, -1, "send");

    train.dataLen = strlen(buf);
    strcpy(train.buf, buf);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);//文件名
    ERROR_CHECK(ret, -1, "send");

    ret = downloadFile(socketFd, buf);
    if(!ret) printf("gets %s OK!\n", buf);
    else printf("gets %s FAILED!\n", buf);                
    printf("---------------------------------\n");
    return 0;
}

int putsfile(int socketFd, char* buf){
    int flag, ret;
    Train_t train;
    flag = 3;
    train.dataLen = sizeof(flag);
    memcpy(train.buf, &flag, train.dataLen);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    //memset(md5sum, 0, sizeof(md5sum));
    //Compute_file_md5(buf, md5sum);
    //printf("%s\n", md5sum);
    train.dataLen = strlen(buf);
    strcpy(train.buf, buf);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);//文件名
    ERROR_CHECK(ret, -1, "send");
    ret = uploadFile(socketFd, buf);
    if(!ret) printf("puts %s OK!\n", buf);
    else printf("puts %s FAILED!\n", buf);                
    printf("---------------------------------\n");
    return 0;
}
int removefile(int socketFd){
    char buf[1000];
    int flag, ret;
    Train_t train;
    memset(buf, 0, sizeof(buf));
    scanf("%s", buf);
    flag = 5;
    train.dataLen = sizeof(flag);
    memcpy(train.buf, &flag, train.dataLen);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");//发命令

    train.dataLen = strlen(buf);
    strcpy(train.buf, buf);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");//发文件名

    recvCycle(socketFd ,&ret, 4);
    recvCycle(socketFd, &flag, ret);
    if(0 == flag) printf("remove %s OK!\n", buf);
    else printf("remove %s failed!\n", buf);
    printf("---------------------------------\n");
    return 0;
}

int getls(int socketFd){
    char buf[1000];
    int flag, ret;
    Train_t train;
    memset(buf, 0, sizeof(buf));
    flag = 2;
    train.dataLen = sizeof(flag);
    memcpy(train.buf, &flag, train.dataLen);
    ret = send(socketFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    memset(&train, 0 ,sizeof(train));
    recvCycle(socketFd, &train.dataLen, 4);
    recvCycle(socketFd, train.buf, train.dataLen);
    puts(train.buf);
    printf("---------------------------------\n");
    return 0;
}

void que_init(pque_t pq, int capacity){
    memset(pq, 0, sizeof(que_t));
    pq->que_capacity = capacity;
    pthread_mutex_init(&pq->mutex, NULL);
}

int que_get(pque_t pq, pNode_t *ppCur){
    if(0 == pq->que_size){
        return -1;
    }
    else{//头部删除
        *ppCur = pq->que_head;
        pq->que_head = pq->que_head->pNext;
        if(NULL == pq->que_head){
            pq->que_tail = NULL;
        }
        pq->que_size--;
        return 0;
    }
}

void que_insert(pque_t pq, pNode_t pNew){
    if(NULL == pq->que_tail){
        pq->que_head = pq->que_tail = pNew;
    }
    else{//尾插
        pq->que_tail->pNext = pNew;
        pq->que_tail = pNew;
    }
    pq->que_size++;
}

