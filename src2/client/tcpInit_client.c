#include "function.h"

int tcpInit_client(int *sfd, char *ip, char *port){
    int socketFd;
	socketFd = socket(AF_INET,SOCK_STREAM,0);
	ERROR_CHECK(socketFd, -1, "socket");
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_addr.s_addr = inet_addr(ip);
	ser.sin_port = htons(atoi(port));
	int ret;
	ret = connect(socketFd, (struct sockaddr*)&ser, sizeof(ser));
	ERROR_CHECK(ret, -1, "connect");
	system("clear");
    *sfd = socketFd;
    return 0;
}
