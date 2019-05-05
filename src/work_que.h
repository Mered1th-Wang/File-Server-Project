#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"

typedef struct node{
    int new_fd;
    struct node *pNext;
}Node_t, *pNode_t; 

typedef struct{
    pNode_t que_head, que_tail;
    int que_capacity;
    int que_size;
    pthread_mutex_t mutex;
}que_t, *pque_t; //描述队列的结构体

void que_init(pque_t, int);
void que_insert(pque_t, pNode_t);
int que_get(pque_t, pNode_t*);

#endif
