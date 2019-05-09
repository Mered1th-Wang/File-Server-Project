#include "factory.h"

int tran_file(int newFd, char* FILENAME, Dir current){
    Train_t train;
    int ret;
    int offset, dataLen;
    char pathName[100] = {0};
    sprintf(pathName, "%s%s", current.pathNow, FILENAME);
    printf("open %s\n", pathName);
    
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

    printf("offset = %d\n", offset);
    lseek(fd, offset, SEEK_SET);

    printf("buf.st_size = %ld\n", buf.st_size);
    
    off_t fileSize;
    if(offset == buf.st_size){
        fileSize = 0;
    }
    else{
        fileSize = sizeof(buf.st_size - offset);
    }
    train.dataLen = sizeof(fileSize);
    memcpy(train.buf, &fileSize, train.dataLen);
    //发文件大小
    ret = send(newFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    //printf("send filesize = %d\n", train.dataLen);

    if(0 == train.dataLen){
        return 0;
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
