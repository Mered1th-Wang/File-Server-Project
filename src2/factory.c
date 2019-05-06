#include "factory.h"

void factory_init(pFactory_t pf, int thread_num, int capacity){
    memset(pf, 0, sizeof(factory_t));
    pf->pth_arr = (pthread_t*)calloc(thread_num, sizeof(pthread_t));
    pf->thread_num = thread_num;
    pthread_cond_init(&pf->cond, NULL);
    que_init(&pf->que, capacity);
}

void factory_start(pFactory_t pf){
    int i;
    pf->start_flag = 1;
    for(i = 0;i < pf->thread_num; i++){
        pthread_create(pf->pth_arr + i, NULL, thread_func, pf);
    }
}


void removeFile(int newFd, char* FILENAME)
{
    Train_t train;
    train.dataLen = unlink(FILENAME);
    send(newFd, &train, 4, 0);
    return ;
}

int getls(int newFd){
    Train_t train;
    memset(&train, 0, sizeof(Train_t));
    DIR *dir;
    dir = opendir("../src2");
    ERROR_CHECK(dir, NULL, "opendir");
    struct dirent *p;
    int count = 0;
    while((p = readdir(dir))){
        if(strcmp(p->d_name, ".") != 0 && strcmp(p->d_name, "..") != 0){
            if(strlen(train.buf) > 0){
                sprintf(train.buf, "%s%s%s", train.buf, " ", p->d_name);
                count++;
            }
            else{ 
                sprintf(train.buf, "%s%s", train.buf, p->d_name);
                count++;
            }
        }
    }
    printf("%s\n", train.buf);
    closedir(dir);
    train.dataLen = strlen(train.buf);
    int ret = send(newFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    return 0;
}