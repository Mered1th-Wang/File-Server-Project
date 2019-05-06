#include "function.h"

//IP PORT 
int main(int argc,char* argv[])
{
	ARGS_CHECK(argc,3);
	int socketFd;
    int ret;
    char commond[20];
    char buf[20];
    ret = tcpInit_client(&socketFd, argv[1], argv[2]);
    ERROR_CHECK(ret, -1, "tcpInit_client");
    Train_t train;
    while(1){
        memset(commond, 0, sizeof(commond));
        scanf("%s", commond);
        if(!strcmp(commond, "quit")){
            close(socketFd);
            return 0;
        }
        else if(!strcmp(commond, "gets")){
            memset(buf, 0, sizeof(buf));
            scanf("%s", buf);
            train.dataLen = 4;
            ret = send(socketFd, &train, 4, 0);//发命令
            ERROR_CHECK(ret, -1, "send");
            train.dataLen = strlen(buf);
            strcpy(train.buf, buf);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);//文件名
            ERROR_CHECK(ret, -1, "send");
            ret = downloadFile(socketFd, buf);
            printf("\n");
            if(!ret) printf("gets %s OK!\n", buf);
            else printf("gets %s FAILED!\n", buf);                
            printf("---------------------------------\n");
        }
        else if(!strcmp(commond, "puts")){
            memset(buf, 0, sizeof(buf));
            scanf("%s", buf);
            train.dataLen = 3;
            ret = send(socketFd, &train, 4, 0);
            ERROR_CHECK(ret, -1, "send");
            train.dataLen = strlen(buf);
            strcpy(train.buf, buf);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);//文件名
            ERROR_CHECK(ret, -1, "send");
            ret = uploadFile(socketFd, buf);
            printf("\n");
            if(!ret) printf("puts %s OK!\n", buf);
            else printf("puts %s FAILED!\n", buf);                
            printf("---------------------------------\n");
        }
        else if(!strcmp(commond, "pwd")){
            memset(buf, 0, sizeof(buf));
            train.dataLen = 6;
            ret = send(socketFd, &train, 4, 0);
            ERROR_CHECK(ret, -1, "send");
            recvCycle(socketFd, &train.dataLen, 4);
            recvCycle(socketFd, buf, train.dataLen);
            puts(buf);
        }
        else if(!strcmp(commond, "remove")){
            memset(buf, 0, sizeof(buf));
            scanf("%s", buf);
            train.dataLen = 5;
            ret = send(socketFd, &train, 4, 0);
            ERROR_CHECK(ret, -1, "send");
            train.dataLen = strlen(buf);
            strcpy(train.buf, buf);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);//文件名
            ERROR_CHECK(ret, -1, "send");
            recvCycle(socketFd ,&train, 4);
            if(0 == train.dataLen) printf("remove %s OK!\n", buf);
            else printf("remove %s OK!\n", buf);
            printf("---------------------------------\n");
        }
        else if(!strcmp(commond, "ls")){
            memset(buf, 0, sizeof(buf));
            train.dataLen = 2;
            ret = send(socketFd, &train, 4, 0);
            ERROR_CHECK(ret, -1, "send");
            recvCycle(socketFd, &train.dataLen, 4);
            recvCycle(socketFd, &train.buf, train.dataLen);
            puts(train.buf);
        }
    }
    close(socketFd);
}
