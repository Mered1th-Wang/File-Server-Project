#include "factory.h"
#define CHECK_recvCycle(ret, retval) {if(ret == retval) {printf("\n LINE = %d, Server is update!\n", __LINE__); break;}} 

int tran_file2(int newFd, char *FILENAME, Dir current){
	int dataLen, ret;
    char buf[1000] = {0};
    
    //接收文件名
    recvCycle(newFd, &dataLen, 4);
    recvCycle(newFd, buf, dataLen);
    
    char pathName[100] = {0};
    sprintf(pathName, "%s%s", current.pathNow, FILENAME);
    int fd = open(pathName, O_CREAT|O_RDWR, 0666);
	ERROR_CHECK(fd, -1, "open");
    
    //接文件大小
    off_t fileSize = 0;
    recvCycle(newFd, &dataLen, 4);
	recvCycle(newFd, &fileSize, dataLen);
    
    //接受文件内容
    if(fileSize >= SIZE){
       ftruncate(fd, fileSize);
       char *pMap = (char*)mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
       ERROR_CHECK(pMap, (char*)-1, "mmap");
       ret = recvCycle(newFd, pMap, fileSize);
       ERROR_CHECK(ret, -1, "recvCycle");
       
       //接受结束标志
       ret = recvCycle(newFd, buf, 4);
       ERROR_CHECK(ret, -1, "recvCycle");
       munmap(pMap, fileSize);
       close(fd);
    }
    else {
        while(1){
            ret = recvCycle(newFd, &dataLen, 4);
            CHECK_recvCycle(ret, -1);
            if(dataLen > 0){
                ret = recvCycle(newFd, buf, dataLen);
                CHECK_recvCycle(ret, -1);
                write(fd, buf, dataLen);
            }
            else break;
        }
        close(fd);
    }
	return 0;
}
