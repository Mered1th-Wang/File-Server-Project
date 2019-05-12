#include "function.h"

int main()
{
    int socketFd;
    int ret;
    char commond[20];
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
        //    if(!info.tcpi_state && scanf("%s", commond) != EOF){
        //        return -1;
        //    }
        memset(commond, 0, sizeof(commond));
        scanf("%s", commond);
        if(!strcmp(commond, "quit")){
            close(socketFd);
            return 0;
        }
        else if(!strcmp(commond, "cd")){
            getcd(socketFd);
        }
        else if(!strcmp(commond, "gets")){
            gets(socketFd);
        }
        else if(!strcmp(commond, "puts")){
            putsfile(socketFd);
        }
        else if(!strcmp(commond, "pwd")){
            getpwd(socketFd);
        }
        else if(!strcmp(commond, "remove")){
            removefile(socketFd);
        }
        else if(!strcmp(commond, "ls")){
            getls(socketFd);
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
