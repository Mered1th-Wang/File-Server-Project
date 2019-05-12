#include "work_que.h"

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
