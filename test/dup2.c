#include <func.h>

int main() {
    int fd = open("data.out", O_RDWR | O_CREAT,0666);
    printf("file fd: %d\n", fd);

    int replaced = dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    printf("duplicated file descriptor: %d\n", replaced);
    perror("duplicate status ");
    printf("haha\n");
    perror("restore status ");
    close(fd);
    perror("close status ");
    return 0;

}
