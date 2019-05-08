#include <func.h>

int main()
{
    struct stat buf;
    int fd = open("log", O_CREAT|O_RDWR, 0666);
    dup2(fd, 1);
    for(int i = 0; i < 1000; i++){
        stat("/home/wj/YunPan/src2/file/wj/hello/", &buf);
        if(S_ISDIR(buf.st_mode))
            printf("this is a dir\n");
        else printf("this is a file\n");
    }
    return 0;
}
