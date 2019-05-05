#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"

#define SIZE 1024*1024

typedef struct{//火车头协议
    int dataLen; 
    char buf[1000]; 
}Train_t;

typedef struct{
    pthread_t *pth_arr; //pthId
    int thread_num; //线程数
    pthread_cond_t cond;//条件变量
    que_t que; //队列
    short start_flag;//未启动为0，启动为1
}factory_t, *pFactory_t;

void* thread_func(void*);
void factory_init(pFactory_t, int, int);
void factory_start(pFactory_t);
int tcpInit(int*, char*, char*, int);
int tran_file(int, char*);
int recvCycle(int, void*, int);

#endif
