#include "head.h"

int tcpInit(int *pSocketFd, char *ip, char *port, int thread_num){
	int socketFd;
	socketFd = socket(AF_INET, SOCK_STREAM, 0);
	ERROR_CHECK(socketFd, -1, "socket");
	int ret, reuse = 1;
    ret = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    ERROR_CHECK(ret, -1, "setsockopt");
    struct sockaddr_in ser;
	bzero(&ser, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(atoi(port));
	ser.sin_addr.s_addr = inet_addr(ip);//点分十进制转为32位的网络字节序
	//printf("bind!\n");
    ret = bind(socketFd, (struct sockaddr*)&ser, sizeof(ser));
	ERROR_CHECK(ret, -1, "bind");
	listen(socketFd, 2 * thread_num);//缓冲区的大小，一瞬间能够放入的客户端连接信息
	*pSocketFd = socketFd;
	return 0;
}
