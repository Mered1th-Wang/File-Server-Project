#include "function.h"
#define CHECK_recvCycle(ret, retval) {if(ret == retval) {printf("\n LINE = %d, Server is update!\n", __LINE__); break;}} 

int downloadFile(int socketFd, char* name){
    int dataLen, ret;
    char buf[1000]={0};
    Train_t train;
    struct stat filebuf;
    off_t offset;

    //接收flag返回值,检查服务端是否存在该文件
    int flag;
    recvCycle(socketFd, &dataLen, 4);
    recvCycle(socketFd, &flag, dataLen);//接收服务端打开文件的返回值
    if(-1 == flag){
        printf("No such file!\n");
        return -1;
    }
    else if(-2 == flag){
        printf("Directory download is not supported.\n");
        return -1;
    }

    //查看本地文件大小（断点续传）发送偏移量
    ret = stat(name, &filebuf);
    //printf("open name = %s, Size = %ld\n", name, filebuf.st_size);
    if(-1 == ret) {
        offset = 0;
        train.dataLen = sizeof(offset);
        memcpy(train.buf, &offset, train.dataLen);
    }
    else {
        offset = filebuf.st_size;
        train.dataLen = sizeof(offset);
        memcpy(train.buf, &offset, train.dataLen);
    }
    //printf("offset = %d\n", train.dataLen); //上次缓存的大小
    //printf("%s\n", name);

    ret = send(socketFd, &train, 4 + train.dataLen, 0);// 发偏移量
    //printf("send offset = %ld\n", offset);
    int fd = open(name, O_CREAT|O_RDWR, 0666);
    ERROR_CHECK(fd, -1, "open");
    lseek(fd, offset, SEEK_SET);
    
    //接文件大小
    off_t fileSize = 0;
    recvCycle(socketFd, &dataLen, 4);
    recvCycle(socketFd, &fileSize, dataLen);
    //printf("downloadFile filesize = %ld\n", fileSize);
    //printf("dataLen = %d, fileSize = %ld\n", dataLen, fileSize);   
    if(fileSize < 0) return -1;
    else if(fileSize == 0) {
        printf("File already exists.\n");
        return -1;
    }

    //printf("recv fileSize = %ld\n", fileSize);
    //接受文件内容
    if(fileSize >= SIZE){
        //printf("ftruncate\n");
        ftruncate(fd, fileSize);
        char *pMap = (char*)mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        ERROR_CHECK(pMap, (char*)-1, "mmap");
        ret = recvCycle(socketFd, pMap, fileSize);
        ERROR_CHECK(ret, -1, "recvCycle");

        //接受结束标志
        ret = recvCycle(socketFd, &train.dataLen, 4);
        
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
            else break;
        }
        close(fd);
    }
    return 0;
}
