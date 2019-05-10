#include "factory.h"

int tran_file(int newFd, char* FILENAME, Dir current){
    Train_t train;
    int ret;
    int offset, dataLen;
    char pathName[100] = {0};
    sprintf(pathName, "%s%s", current.pathNow, FILENAME);
    //printf("open %s\n", pathName);
    
    int fd = open(pathName, O_RDONLY);
    struct stat buf;
    int flag;
    if(-1 == fd){
        perror("open");
        flag = -1;
        train.dataLen = sizeof(flag);
        memcpy(train.buf, &flag, train.dataLen);
        ret = send(newFd, &train, 4 + train.dataLen, 0);
        ERROR_CHECK(ret, -1, "send");
        return -1;
    }
    fstat(fd, &buf);
    if(S_ISDIR(buf.st_mode)){
        flag = -2;
        train.dataLen = sizeof(flag);
        memcpy(train.buf, &flag, train.dataLen);
        ret = send(newFd, &train, 4 + train.dataLen, 0);
        ERROR_CHECK(ret, -1, "send");
        return -1;
    }
    flag = 0;
    train.dataLen = sizeof(flag);
    memcpy(train.buf, &flag, train.dataLen);
    ret = send(newFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    
    //接收偏移量
    recvCycle(newFd, &dataLen, 4);
    recvCycle(newFd, &offset, dataLen);

    //printf("recv offset = %d\n", offset);
    lseek(fd, offset, SEEK_SET);

    //printf("buf.st_size = %ld\n", buf.st_size);
    
    off_t fileSize;
    fileSize = buf.st_size - offset;
    train.dataLen = sizeof(fileSize);
    memcpy(train.buf, &fileSize, train.dataLen);
    //发文件剩余大小
    ret = send(newFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    //printf("send filesize = %ld\n", fileSize);
    
    if(fileSize <= 0){
        return -1;
    }
    
    //发文件内容 
    if(buf.st_size >= SIZE){
        ret = sendfile(newFd, fd, NULL, buf.st_size);
        ERROR_CHECK(ret, -1, "sendfile");
    }
    else{
        while((train.dataLen = read(fd, train.buf, sizeof(train.buf)))){
            ret = send(newFd, &train, 4 + train.dataLen, 0);
            ERROR_CHECK(ret, -1, "send");
        }
    }

    //发送结束标志
    ret = send(newFd, &train, 4, 0);
    ERROR_CHECK(ret, -1, "send");
    return 0;
}

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
