#include "factory.h"

void* thread_func(void *p)
{
    pFactory_t pf = (pFactory_t)p;
    pque_t pq = &pf->que;
    pNode_t pDelete;
    while(1){
        pthread_mutex_lock(&pq->mutex);
        if(!pq->que_size){
            pthread_cond_wait(&pf->cond, &pq->mutex);
        }
        if(-1 == pq->que_tail->new_fd){
            pthread_mutex_unlock(&pq->mutex);
            pthread_exit(NULL);
        }
        que_get(pq, &pDelete);//从队列里获取发任务的客户端的socketFd
        pthread_mutex_unlock(&pq->mutex);
        while(1){
            int dataLen, option;
            char name[1000] = {0};
            recvCycle(pDelete->new_fd, &option, 4);//接收命令
            printf("option = %d\n",option);
            if(4 == option){
                //接受文件名
                recvCycle(pDelete->new_fd, &dataLen, 4);
                recvCycle(pDelete->new_fd, &name, dataLen);
                printf("name = %s\n", name);
                tran_file(pDelete->new_fd, name);//发文件
            }
        }
        free(pDelete);
        pDelete = NULL;
        printf("任务完成\n");
    }
}

int exit_fds[2];

void sigfunc_exit(int sigNum){
    write(exit_fds[1], &sigNum, 1);
    int status;
    wait(&status);
    if(WIFEXITED(status)){
        printf("exit value = %d\n", WEXITSTATUS(status));
    }
    exit(0);
}

int main(int argc, char** argv)
{
	ARGS_CHECK(argc, 5);
    pipe(exit_fds);
    while(fork()){
        //close(exit_fds[0]);
        signal(SIGUSR1, sigfunc_exit);//设定退出机制
        wait(NULL);
    }
    close(exit_fds[1]);
    //IP PORT THREAD_NUM CAPACITY
	factory_t f;
	int thread_num = atoi(argv[3]);
	int capacity = atoi(argv[4]);
	factory_init(&f, thread_num, capacity);
	factory_start(&f);//创建线程
    int sfd;
    pque_t pq = &f.que;//锁在f里，为方便操作，定义 一个pque_t pq指向f.que
    tcpInit(&sfd, argv[1], argv[2], thread_num); //tcp listen;
    
    int epfd = epoll_create(1);
    struct epoll_event event, evs[2];
    event.events = EPOLLIN;
    event.data.fd = sfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event);
    event.data.fd = exit_fds[0];
    epoll_ctl(epfd, EPOLL_CTL_ADD, exit_fds[0], &event);
    int new_fd;
    int fdReadyNum;
    int i;
    while(1){
        memset(evs, 0, sizeof(evs));
        fdReadyNum = epoll_wait(epfd, evs, 2, -1);
        for(i = 0; i < fdReadyNum; i++){
            if(evs[i].data.fd == sfd){
                new_fd = accept(sfd, NULL, NULL);
                pNode_t pnew = (pNode_t)calloc(1, sizeof(Node_t));
                pnew->new_fd = new_fd;
                pthread_mutex_lock(&pq->mutex);
                que_insert(pq, pnew);//任务放入队列
                pthread_mutex_unlock(&pq->mutex); 
                pthread_cond_signal(&f.cond); //唤醒一个子进程
            }
            if(evs[i].data.fd == exit_fds[0]){
                f.start_flag = 0;
                for(i = 0; i < f.thread_num; i++){
                    pthread_cond_signal(&f.cond);
                }
                for(i = 0; i < f.thread_num; i++){
                    pthread_join(f.pth_arr[i], NULL);
                }
                exit(0);
            }
        }
    }
    return 0;
}
