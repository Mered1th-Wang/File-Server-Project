#include "factory.h"

const char *opt[] = {"", "cd", "ls", "puts", "gets", "remove", "pwd", "wrong input"};
char timeNow[50] = {0};
char name[50] = {0};

void* thread_func(void *p)
{
    int ret;
    pFactory_t pf = (pFactory_t)p;
    pque_t pq = &pf->que;
    pNode_t pDelete;
    Train_t train;
    Dir current;
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
        login(pDelete, name, &current);
        printf("%s\n", current.pathNow);
        while(1){
            int dataLen, option;
            char buf[512] = {0};
            ret = recvCycle(pDelete->new_fd, &train, 4);//接收命令
            if(-1 == ret) break;
            option = train.dataLen;
            gettime(timeNow, 50);
            printf("%s -> excuted the commond, \"", name);
            if(1 == option){
                
            }
            else if(2 == option){
                printf("%s\"\n", opt[option]);
                getls(pDelete->new_fd, name, current);
            }
            else if(3 == option){
                //接收文件名
                printf("%s ", opt[option]);
                ret = recvCycle(pDelete->new_fd, &dataLen, 4);
                if(-1 == ret) break;
                recvCycle(pDelete->new_fd, buf, dataLen);
                printf("%s\"\n", buf);
                tran_file2(pDelete->new_fd, buf, current);//接文件
            }
            else if(4 == option){
                //接收文件名
                printf("%s ", opt[option]);
                recvCycle(pDelete->new_fd, &dataLen, 4);
                recvCycle(pDelete->new_fd, buf, dataLen);
                printf("%s\"\n", buf);

                //接收偏移量
                recvCycle(pDelete->new_fd, &dataLen, 4);
                tran_file(pDelete->new_fd, buf, dataLen, current);//发文件
            }
            else if(5 == option){
                //接收文件名
                printf("%s ", opt[option]);
                ret = recvCycle(pDelete->new_fd, &dataLen, 4);
                if(-1 == ret) break;
                recvCycle(pDelete->new_fd, buf, dataLen);
                printf("%s\"\n", buf);
                removeFile(pDelete->new_fd, buf, current);//删除文件
            }
            else if(6 == option){
                printf("%s\" \n", opt[option]);
                memset(buf, 0, sizeof(buf));
                //getcwd(buf, sizeof(buf));
                //sprintf(buf, "%s%s%s%s", buf, "/", name, "/");
                train.dataLen = strlen(current.pathNow);
                strcpy(train.buf, current.pathNow);
                buf[train.dataLen] = '\0';
                train.dataLen++;
                send(pDelete->new_fd, &train, 4 + train.dataLen, 0);
            }
            else printf("wrong input!\n");
        }
        free(pDelete);
        pDelete = NULL;
        gettime(timeNow, 50);
        printf("%s connection closed.\n", name);
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

int main()
{
    pipe(exit_fds);
    while(fork()){
        //close(exit_fds[0]);
        signal(SIGUSR1, sigfunc_exit);//设定退出机制
        wait(NULL);
    }
    printf("\n");
    int logfd;
    logfd = open("log", O_CREAT|O_RDWR|O_APPEND, 0666);
    //dup2(logfd, STDOUT_FILENO);
    //dup2(logfd, STDERR_FILENO);
    close(exit_fds[1]);
    gettime(timeNow, 50);
    printf("Server startup.\n");
    char ip[100] = {0};
    char port[10] = {0};
    int thread_num;
    int capacity;
    FILE *config;
    config = fopen("./conf/server.conf", "r");
    ERROR_CHECK(config, NULL, "fopen");
    fscanf(config, "%s%s%d%d", ip, port, &thread_num, &capacity);
    factory_t f;
	factory_init(&f, thread_num, capacity);
	factory_start(&f);//创建线程
    int sfd;
    pque_t pq = &f.que;//锁在f里，为方便操作，定义 一个pque_t pq指向f.que
    tcpInit(&sfd, ip, port, thread_num); //tcp listen;
    
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
