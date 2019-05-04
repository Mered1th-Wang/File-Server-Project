#include <func.h>
int sendFd(int sfd,int fd)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct iovec iov[2];
	char buf1[10]="hello";
	char buf2[10]="world";
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=sendmsg(sfd,&msg,0);
	ERROR_CHECK(ret,-1,"sendmsg");
	return 0;
}
int recvFd(int sfd,int *fd)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct iovec iov[2];
	char buf1[10];
	char buf2[10];
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=recvmsg(sfd,&msg,0);
	ERROR_CHECK(ret,-1,"sendmsg");
	*fd=*(int*)CMSG_DATA(cmsg);
	return 0;
}
int main()
{
	int fds[2];
	socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
	if(!fork())
	{
		close(fds[1]);
		int fd;
		recvFd(fds[0],&fd);
		printf("child fd=%d,fds[0]=%d\n",fd,fds[0]);
		char buf[128]={0};
		read(fd,buf,sizeof(buf));
		printf("buf=%s\n",buf);
		close(fds[0]);
		return 0;
	}else{
		close(fds[0]);
		int fd;
		fd=open("file",O_RDWR);
		printf("parent fd=%d\n",fd);
		sendFd(fds[1],fd);
		close(fds[1]);
		wait(NULL);
		return 0;
	}
}
