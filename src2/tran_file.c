#include "factory.h"

int tran_file(int newFd, char* FILENAME){
    Train_t train;
    int ret;
    
    //发文件名
    train.dataLen = strlen(FILENAME);
    strcpy(train.buf, FILENAME);
    int fd = open(FILENAME, O_RDONLY);
    ret = send(newFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");

    //发文件大小
    struct stat buf;
    fstat(fd, &buf);
    train.dataLen = sizeof(buf.st_size);
    memcpy(train.buf, &buf.st_size, train.dataLen);
    ret = send(newFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    
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
