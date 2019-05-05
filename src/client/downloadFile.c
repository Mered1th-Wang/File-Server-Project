#include "function.h"
#define CHECK_recvCycle(ret, retval) {if(ret == retval) {printf("\n LINE = %d, Server is update!\n", __LINE__); break;}} 

int downloadFile(int socketFd, char *name){
	int dataLen, ret;
    char buf[1000]={0};
#ifdef DEBUG
    printf("go in downloadFile\n");
#endif
    //接收文件名
    recvCycle(socketFd, &dataLen, 4); //接收命令
    printf("dataLen = %d\n",dataLen);//4
    recvCycle(socketFd, &dataLen, 4);
    printf("dataLen = %d\n",dataLen);//4 file
    recvCycle(socketFd, buf, dataLen);
   // printf("buf = %s\n", buf);
   // printf("strlen(buf)= %d\n", strlen(buf));
#ifdef DEBUG
    printf("finish recv filename\n");
#endif
    char fileName[256] = {0};
    strcpy(fileName, buf);
#ifdef DEBUG
    printf("buf = %s\n", buf);
    printf("name = %s\n", name);
    printf("fileName = %s\n", fileName);
#endif
    int fd = open(name, O_CREAT|O_RDWR, 0666);
	ERROR_CHECK(fd, -1, "open");
    
    //接文件大小
    off_t fileSize = 0;
    recvCycle(socketFd, &dataLen, 4); 
    recvCycle(socketFd, &dataLen, 4);
	recvCycle(socketFd, &fileSize, dataLen);
    
    //接受文件内容
    if(fileSize >= SIZE){
       ftruncate(fd, fileSize);
       char *pMap = (char*)mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
       ERROR_CHECK(pMap, (char*)-1, "mmap");
       ret = recvCycle(socketFd, pMap, fileSize);
       ERROR_CHECK(ret, -1, "recvCycle");
       
       //接受结束标志
       ret = recvCycle(socketFd, buf, 8);
       ERROR_CHECK(ret, -1, "recvCycle");
       munmap(pMap, fileSize);
       close(fd);
    }
    else {
        printf("into while\n");
        while(1){
            ret = recvCycle(socketFd, &dataLen, 4);
            CHECK_recvCycle(ret, -1);
            //printf("count1 = %d\n", count);
            //printf("dataLen = %d\n", dataLen);
            ret = recvCycle(socketFd, &dataLen, 4);
            CHECK_recvCycle(ret, -1);
            //printf("count2 = %d\n", count);
            if(dataLen > 0){
                ret = recvCycle(socketFd, buf, dataLen);
                CHECK_recvCycle(ret, -1);
                write(fd, buf, dataLen);
            }
            else {
                printf("100%%             \n");
                break;
            }
        }
        close(fd);
    }
	return 0;
}
