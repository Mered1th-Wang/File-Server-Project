#include "function.h"

int exitFds[2]; //全局变量，用于信号处理的管道，同一管道的目的是在同一进程中既可读又可写

void sigFunc(int sigNum){
    write(exitFds[1], &sigNum, 1);//写一个字节
}

int main(int argc, char* argv[]){
    while(fork()) //爷进程进入循环
    {
        int status;
        wait(&status); //获取退出码
        if(WIFEXITED(status)) //如果父进程是正常退出
        {
            printf("child exit normal\n");
            exit(0);//爷进程退出

        }
        //父进程非正常退出，重新回while循环创建父进程
    }
    ARGS_CHECK(argc, 4);
    pipe(exitFds);
    signal(SIGUSR1, sigFunc);
    int ret;
    int childNum = atoi(argv[3]);
    Process_Data *pChild = (Process_Data*)calloc(childNum, sizeof(Process_Data));
    makeChild(pChild, childNum);//创建子进程
    int socketFd;
    tcpInit(&socketFd, argv[1], argv[2]);//建立TCP连接
    int epfd;
    epfd = epoll_create(1);//创建一个句柄，占用一个文件描述符，参数表示需要监控的数目
    struct epoll_event event, *evs;
    evs = (struct epoll_event*)calloc(childNum + 2, sizeof(struct epoll_event));
    event.events = EPOLLIN;
    event.data.fd = socketFd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, socketFd, &event);//监听socketFd
    ERROR_CHECK(ret, -1, "epoll_ctl");
    event.data.fd = exitFds[0];
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, exitFds[0], &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    int i, j;
    for(i = 0; i < childNum ; i++){
        event.data.fd = pChild[i].fd;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, pChild[i].fd, &event);//将要监控的子进程的fd加入
        ERROR_CHECK(ret, -1, "epoll_ctl");
    }
    int readyFdNum; 
    int newFd;
    //int count = 0;
    while(1){
        readyFdNum = epoll_wait(epfd, evs, childNum + 2, -1);
        //printf("count = %d, readyFdNum = %d\n",count++, readyFdNum);
        //epoll_wait等待事件的产生，参数evs用来从内核得到事件的集合
        //用childNum + 1告知内核这个events有多大
        for(i = 0; i < readyFdNum; i++){
            //有客户端连入
            if(evs[i].events == EPOLLIN && evs[i].data.fd == socketFd){
                //断开后不会进入该循环
                newFd = accept(socketFd, NULL, NULL);//不保存远程主机信息
                for(j = 0; j < childNum; j++){
                    if(!pChild[j].busy){ //找到非忙碌的子进程，发任务（文件描述符）
                        sendFd(pChild[j].fd, newFd, 1);
                        pChild[j].busy = 1;
                        printf("%d child is busy\n", pChild[j].pid);
                        break;
                    }
                }	
                close(newFd);   //限制客户端只下载一次后就关闭，否则newFd的引用计数为2，即父进程和子进程都可以读取数据
            }
            if(evs[i].events == EPOLLIN && evs[i].data.fd == exitFds[0]){
                printf("start exit\n");
                close(socketFd);
                //两种方式：1、暴力Kill 2、同步退出机制
                //暴力kill
                for(j = 0;j < childNum; j++){
                    //kill(pChild[j].pid, 9);
                    sendFd(pChild[j].fd, 0, 0);//给所有的子进程发送0号描述符(没啥用)，exitFlag为0
                }
                for(j = 0;j < childNum; j++){
                    wait(NULL);
                }
                return 0;
            }
            for(j = 0; j < childNum; j++){
                if(evs[i].data.fd == pChild[j].fd){  //遍历所有子进程的fd
                    //判断就绪描述符是哪个子进程的管道对端，说明子进程已完成任务，就将对应子进程标记为非忙碌，并读出管道内容。
                    //printf("%d %d\n",evs[i].data.fd, pChild[j].fd);
                    read(pChild[j].fd, &ret, 1);//对端写一个字节这边读一个字节标记已完成任务，如果数据不读出，则会一直可读状态
                    //       if(0 == ret2){
                    //           return -1;
                    //       }
                    pChild[j].busy = 0;
                    //printf("ret2 = %d\n", ret2);
                    printf("%d child is not busy\n\n", pChild[j].pid);
                }
            }
            // printf("%d\n", count++);
            // sleep(3);
        }
    }
    return 0;
}
