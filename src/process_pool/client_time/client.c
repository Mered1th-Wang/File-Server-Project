#include "function.h"

int main(int argc,char* argv[])
{
	ARGS_CHECK(argc,3);
	int socketFd;
	socketFd=socket(AF_INET,SOCK_STREAM,0);
	ERROR_CHECK(socketFd,-1,"socket");
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);//点分十进制转为32位的网络字节序
	int ret;
	ret=connect(socketFd,(struct sockaddr*)&ser,sizeof(ser));
	ERROR_CHECK(ret,-1,"connect");
	printf("connect success\n");
	int dataLen;
	char buf[1000]={0};
    //接收文件名
	recvCycle(socketFd,&dataLen,4);
	recvCycle(socketFd,buf,dataLen);
	int fd;
	fd=open(buf,O_CREAT|O_WRONLY,0666);
	ERROR_CHECK(fd,-1,"open");
    //接文件大小
    off_t fileSize = 0;//文件大小off_t 长整型
	recvCycle(socketFd,&dataLen,4);
	recvCycle(socketFd,&fileSize,dataLen);
    time_t start, now;
	//接受文件内容
    start = now = time(NULL);
    int downLoadSize = 0;
    while(1)
	{
		recvCycle(socketFd,&dataLen,4);
		if(dataLen>0)
		{
			recvCycle(socketFd,buf,dataLen);
			write(fd,buf,dataLen);
            downLoadSize+=dataLen;
            time(&now);
            if(now-start>=1){
                printf("\r%5.2f%%", (float)downLoadSize / fileSize * 100);
                fflush(stdout);
                start = now;
            }
		}else{
            printf("\r100%%         \n");
			break;
		}
	}
	close(fd);
	close(socketFd);
	return 0;
}
