#include "function.h"
#define CHECK_recvCycle(ret, retval) {if(ret == retval) {printf("\n LINE = %d, Server is update!\n", __LINE__); break;}} 

int downloadFile(int socketFd, char *name){
	int dataLen, ret;
    char buf[1000]={0};
#ifdef DEBUG
    printf("go in downloadFile\n");
#endif
    //接收文件名
    recvCycle(socketFd, &dataLen, 4);
   //printf("downloadFile dataLen = %d\n",dataLen);//4 file
    recvCycle(socketFd, buf, dataLen);
   //printf("downloadFile buf = %s\n", buf);
    
    char fileName[256] = {0};
    strcpy(fileName, buf);
#ifdef DEBUG
    printf("downloadFile fileName = %s\n", fileName);
#endif
    int fd = open(name, O_CREAT|O_RDWR, 0666);
	ERROR_CHECK(fd, -1, "open");
    
    //接文件大小
    off_t fileSize = 0;
    recvCycle(socketFd, &dataLen, 4);
    //printf("downloadFile filesize dataLen = %d\n", dataLen);
	recvCycle(socketFd, &fileSize, dataLen);
    
    //接受文件内容
    if(fileSize >= SIZE){
       ftruncate(fd, fileSize);
       char *pMap = (char*)mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
       ERROR_CHECK(pMap, (char*)-1, "mmap");
       ret = recvCycle(socketFd, pMap, fileSize);
       ERROR_CHECK(ret, -1, "recvCycle");
       
       //接受结束标志
       ret = recvCycle(socketFd, buf, 4);
       ERROR_CHECK(ret, -1, "recvCycle");
       munmap(pMap, fileSize);
       close(fd);
    }
    else {
        while(1){
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
                break;
            }
        }
        close(fd);
    }
	return 0;
}
