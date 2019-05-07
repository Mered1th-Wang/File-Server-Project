#include "function.h"

int main()
{
	int socketFd;
    int ret;
    char commond[20];
    char buf[20];
    //char md5sum[33];
    Train_t train;
    char ip[50] = {0};
    char port[10] = {0};
    FILE *config;
    config = fopen("../conf/client.conf", "r");
    ERROR_CHECK(fopen, NULL, "fopen");
    fscanf(config, "%s%s", ip, port);
    ret = tcpInit_client(&socketFd, ip, port);
    ERROR_CHECK(ret, -1, "tcpInit_client");
    
    ret = login_client(socketFd);
    ERROR_CHECK(ret, -1, "login_client");

    printf("Please enter the commond or input \"quit\" to exit\n");
    
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
            
            struct stat filebuf;
            ret = stat(buf, &filebuf);
            if(-1 == ret) train.dataLen = 0;
            else train.dataLen = filebuf.st_size;
            ret = send(socketFd, &train, 4, 0);// 发偏移量
            ERROR_CHECK(ret, -1, "send");

            ret = downloadFile(socketFd, buf, train.dataLen);
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
            //memset(md5sum, 0, sizeof(md5sum));
            //Compute_file_md5(buf, md5sum);
            //printf("%s\n", md5sum);
            train.dataLen = strlen(buf);
            strcpy(train.buf, buf);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);//文件名
            ERROR_CHECK(ret, -1, "send");
            ret = uploadFile(socketFd, buf);
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
            printf("---------------------------------\n");
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
            printf("---------------------------------\n");
        }
        else{
            train.dataLen = -1;
            ret = send(socketFd, &train, 4, 0);
            ERROR_CHECK(ret, -1, "send");
            printf("input wrong!\n");
            printf("---------------------------------\n");
        }
    }
    close(socketFd);
}
