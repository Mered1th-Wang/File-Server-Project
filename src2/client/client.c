#include "function.h"

int main()
{
	int socketFd;
    int ret;
    char commond[20];
    char buf[20];
    Train_t train;
    char ip[50] = {0};
    char port[10] = {0};
    FILE *config;
    int flag;
    config = fopen("../conf/client.conf", "r");
    ERROR_CHECK(fopen, NULL, "fopen");
    fscanf(config, "%s%s", ip, port);
    ret = tcpInit_client(&socketFd, ip, port);
    ERROR_CHECK(ret, -1, "tcpInit_client");
    
    ret = login_client(socketFd);
    ERROR_CHECK(ret, -1, "login_client");

    printf("Please enter the commond or input \"quit\" to exit\n");
    
//    struct tcp_info info;
//    int len = sizeof(info);

    while(1){
    //    getsockopt(socketFd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
        memset(commond, 0, sizeof(commond));
    //    if(!info.tcpi_state && scanf("%s", commond) != EOF){
    //        return -1;
    //    }
        scanf("%s", commond);
        if(!strcmp(commond, "quit")){
            close(socketFd);
            return 0;
        }
        else if(!strcmp(commond, "cd")){
         //   memset(buf, 0, sizeof(buf));
         //   scanf("%s", buf);
         //   char tempName[100] = {0};
         //   strcpy(tempName, buf);
         //   train.dataLen = 1;
         //   ret = send(socketFd, &train, 4, 0);
         //   ERROR_CHECK(ret, -1, "send");

         //   train.dataLen = strlen(buf);
         //   strcpy(train.buf, buf);
         //   ret = send(socketFd, &train, 4 + train.dataLen, 0);//目录名
         //   ERROR_CHECK(ret, -1, "send");
         //   
         //   recvCycle(socketFd, &train, 4);
         //   if(train.dataLen == -1){
         //       printf("Failed! Already root directory.\n");
         //       printf("---------------------------------\n");
         //   }
         //   else if(train.dataLen == -2){
         //       printf("Failed! %s is not directory.\n", tempName);
         //       printf("---------------------------------\n");
         //   }
         //   else{
         //       recvCycle(socketFd, &train.dataLen, 4);
         //       recvCycle(socketFd, buf, train.dataLen);
         //       puts(buf);
         //       printf("---------------------------------\n");
         //   }
                getcd(socketFd);
        }
        else if(!strcmp(commond, "gets")){
            memset(buf, 0, sizeof(buf));
            scanf("%s", buf);
            flag = 4;
            train.dataLen = sizeof(flag);
            memcpy(train.buf, &flag, train.dataLen);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);//发命令
            ERROR_CHECK(ret, -1, "send");
            
            train.dataLen = strlen(buf);
            strcpy(train.buf, buf);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);//文件名
            ERROR_CHECK(ret, -1, "send");

            ret = downloadFile(socketFd, buf);
            if(!ret) printf("gets %s OK!\n", buf);
            else printf("gets %s FAILED!\n", buf);                
            printf("---------------------------------\n");
        }
        else if(!strcmp(commond, "puts")){
            memset(buf, 0, sizeof(buf));
            scanf("%s", buf);
            flag = 3;
            train.dataLen = sizeof(flag);
            memcpy(train.buf, &flag, train.dataLen);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);
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
          //  memset(buf, 0, sizeof(buf));
          //  train.dataLen = 6;
          //  ret = send(socketFd, &train, 4, 0);
          //  ERROR_CHECK(ret, -1, "send");
          //  recvCycle(socketFd, &train.dataLen, 4);
          //  recvCycle(socketFd, buf, train.dataLen);
          //  puts(buf);
          //  printf("---------------------------------\n");
            getpwd(socketFd);
        }
        else if(!strcmp(commond, "remove")){
            memset(buf, 0, sizeof(buf));
            scanf("%s", buf);
            flag = 5;
            train.dataLen = sizeof(flag);
            memcpy(train.buf, &flag, train.dataLen);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);
            ERROR_CHECK(ret, -1, "send");//发命令

            train.dataLen = strlen(buf);
            strcpy(train.buf, buf);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);
            ERROR_CHECK(ret, -1, "send");//发文件名
            
            recvCycle(socketFd ,&ret, 4);
            recvCycle(socketFd, &flag, ret);
            if(0 == flag) printf("remove %s OK!\n", buf);
            else printf("remove %s failed!\n", buf);
            printf("---------------------------------\n");
        }
        else if(!strcmp(commond, "ls")){
            memset(buf, 0, sizeof(buf));
            flag = 2;
            train.dataLen = sizeof(flag);
            memcpy(train.buf, &flag, train.dataLen);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);
            ERROR_CHECK(ret, -1, "send");
            recvCycle(socketFd, &train.dataLen, 4);
            recvCycle(socketFd, &train.buf, train.dataLen);
            puts(train.buf);
            printf("---------------------------------\n");
        }
        else {
            flag = -1;
            train.dataLen = sizeof(flag);
            memcpy(train.buf, &flag, train.dataLen);
            ret = send(socketFd, &train, 4 + train.dataLen, 0);
            ERROR_CHECK(ret, -1, "send");
            printf("input wrong!\n");
            setbuf(stdin, NULL);
            printf("---------------------------------\n");
        }
    }
    close(socketFd);
}
