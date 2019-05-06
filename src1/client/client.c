#include "function.h"

//IP PORT 
int main(int argc,char* argv[])
{
	ARGS_CHECK(argc,3);
	int socketFd;
    int ret;
    char commond[20];
    char name[20];
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
            memset(name, 0, sizeof(name));
            scanf("%s", name);
            train.option = 4;
            train.dataLen = strlen(name);
            strcpy(train.buf, name);
            ret = send(socketFd, &train, 8 + train.dataLen, 0);//发命令+文件名
            ERROR_CHECK(ret, -1, "send");
            ret = downloadFile(socketFd, name);
            printf("---------------------------------\n");
            if(!ret) printf("gets %s OK!\n", name);
            else printf("gets %s FAILED!\n", name);                
        }
        else if(!strcmp(commond, "puts")){
            //uploadFile(socketFd);
        }
    }
    close(socketFd);
}
