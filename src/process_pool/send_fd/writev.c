#include <func.h>

int main(){
	int fd = open("file", O_RDWR);
	struct iovec iov[2];
	char buf1[10] = "hello";
	char buf2[10] = "world";
	iov[0].iov_base = buf1;
	iov[0].iov_len = 5;
	iov[1].iov_base = buf2;
	iov[1].iov_len = 5;
	writev(fd, iov, 2);
	close(fd);
}
