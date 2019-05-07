#include "factory.h"

int tran_file(int newFd, char* FILENAME, int offset, Dir current){
    Train_t train;
    int ret;
    char pathName[100] = {0};
    sprintf(pathName, "%s%s", current.pathNow, FILENAME);
    printf("open %s\n", pathName);
    int fd = open(pathName, O_RDONLY);
    ERROR_CHECK(fd, -1, "open");
    printf("offset = %d\n", offset);
    lseek(fd, offset, SEEK_SET);

    struct stat buf;
    fstat(fd, &buf);
    printf("buf.st_size = %ld\n", buf.st_size);
    if(offset == buf.st_size){
        train.dataLen = 0;
    }
    else{
        train.dataLen = sizeof(buf.st_size - offset);
    }
    memcpy(train.buf, &buf.st_size, train.dataLen);
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
