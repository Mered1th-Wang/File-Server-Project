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
