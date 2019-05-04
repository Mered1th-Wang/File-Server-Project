#include "function.h"
//服务端
int makeChild(Process_Data *pChild, int childNum)
{
	int i;
	pid_t pid;
	int fds[2];
	int ret;
	for(i=0;i < childNum;i++)
	{
		//初始化socketpair类型描述符，与pipe不同，每一端既可读又可写
		ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
		ERROR_CHECK(ret, -1, "socketpair");
		pid = fork();
		if(0 == pid)  //子进程
		{
			close(fds[1]); 
			ret = childHandle(fds[0]);
            if(-1 == ret){
                return -1;
            }
		}
		close(fds[0]); //父进程
		pChild[i].pid = pid;
		pChild[i].fd = fds[1];
		pChild[i].busy = 0;
	}
	return 0;
}

int childHandle(int fd)
{
    int ret;
	int newFd;
    int exitFlag;
	while(1){
        //开5个子进程，newFd为10，因内核控制信息，父子进程共享同一块文件描述符
		recvFd(fd, &newFd, &exitFlag);//接收到任务
#ifdef DEBUG
        printf("newFd = %d\n", newFd);
#endif
        if(exitFlag){
            ret = tranFile(newFd);//给客户端发送文件
            printf("I get task %d\n", newFd);
            if(-1 == ret){
                printf("tranFile not finish!\n\n");
                close(newFd);//断点续传在这搞
                continue;
            }
        	//newFd的值为10，socketFd为3，有五个子进程管道，为4-8，epfd也占1
        	printf("finish send file\n");
            close(newFd);
        }
        else{
            exit(0);//不能用break
        }
 //       if(0 == newFd){
 //           printf("conncect failed!\n");
 //           return -1;
 //       }
		write(fd, &newFd, 1); //通知父进程非忙碌,写一个字节即可
	}
}
