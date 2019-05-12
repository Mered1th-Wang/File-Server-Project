#include "function.h"

int childsfd;
char token[100] = {0};

void* thread_func(void *p){
    FILE *config;
    char ip[50] = {0};
    char port[10] = {0};
    int ret;
    config = fopen("../conf/client.conf", "r");
    fscanf(config, "%s%s", ip, port);
    ret = tcpInit_client(&childsfd, ip, port);
    //printf("子线程登录前收到的token值:%s\n\n", token);
    printf("Please enter the commond or input \"quit\" to exit\n");
    ret = login_childclient(childsfd, token);
    //printf("子线程登录成功\n");
    pNode_t pDelete;
    pFactory_t pf = (pFactory_t)p;
    pque_t pq = &pf->que;
    while(1){
        pthread_mutex_lock(&pq->mutex);
        if(!pq->que_size){
            pthread_cond_wait(&pf->cond, &pq->mutex);
        }
        if(-1 == pq->que_tail->new_fd){
            pthread_mutex_unlock(&pq->mutex);
            pthread_exit(NULL);
        }
        que_get(pq, &pDelete);
        pthread_mutex_unlock(&pq->mutex);
        //printf("取得任务：%d\n", pDelete->option);
        if(pDelete->option == 1){
            gets(childsfd, pDelete->buf);
        }
        else{
            putsfile(childsfd, pDelete->buf);
        }
    }
    return NULL;
}

int main()
{
    int socketFd;
    int ret;
    char commond[20];
    Train_t train;
    char ip[50] = {0};
    char port[10] = {0};
    FILE *config;
    int flag;
    config = fopen("../conf/client.conf", "r");
    ERROR_CHECK(fopen, NULL, "fopen");
    fscanf(config, "%s%s", ip, port);
    ret = tcpInit_client(&socketFd, ip, port);//主线程创建连接
    ERROR_CHECK(ret, -1, "tcpInit_client");

    ret = login_client(socketFd, token); //主线程登录
    ERROR_CHECK(ret, -1, "login_client");
    //printf("主线程登录成功\n");

    int capacity = 100;
    factory_t f;
    memset(&f, 0, sizeof(factory_t));
    pthread_cond_init(&f.cond, NULL);
    que_init(&f.que, capacity);
    pthread_create(&f.pid, NULL, thread_func, &f);
    
    pque_t pq = &f.que;
    pNode_t pnew = (pNode_t)calloc(1, sizeof(Node_t));
    pnew->new_fd = childsfd;

    //sleep(2);

    //    struct tcp_info info;
    //    int len = sizeof(info);

    while(1){
        //    getsockopt(socketFd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
        //    if(!info.tcpi_state && scanf("%s", commond) != EOF){
        //        return -1;
        //    }
        memset(commond, 0, sizeof(commond));
        scanf("%s", commond);
        if(!strcmp(commond, "quit")){
            close(socketFd);
            return 0;
        }
        else if(!strcmp(commond, "cd")){
            getcd(socketFd);
        }
        else if(!strcmp(commond, "gets")){
            memset(pnew->buf, 0, sizeof(pnew->buf));
            scanf("%s", pnew->buf);
            
            pnew->option = 1;
            pthread_mutex_lock(&pq->mutex);
            que_insert(pq, pnew);
            pthread_mutex_unlock(&pq->mutex);
            pthread_cond_signal(&f.cond);
        }
        else if(!strcmp(commond, "puts")){
            memset(pnew->buf, 0, sizeof(pnew->buf));
            scanf("%s", pnew->buf);
            
            pnew->option = 2;
            pthread_mutex_lock(&pq->mutex);
            que_insert(pq, pnew);
            pthread_mutex_unlock(&pq->mutex);
            pthread_cond_signal(&f.cond);
        }
        else if(!strcmp(commond, "pwd")){
            getpwd(socketFd);
        }
        else if(!strcmp(commond, "remove")){
            removefile(socketFd);
        }
        else if(!strcmp(commond, "ls")){
            getls(socketFd);
        }
        else {
            flag = -1;
            train.dataLen = sizeof(flag);
            memcpy(train.buf, &flag, train.dataLen);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);
            ERROR_CHECK(ret, -1, "send");
            printf("input wrong!\n");
            setbuf(stdin, NULL);
            printf("---------------------------------\n");
        }
    }
    close(socketFd);
}
