#include "factory.h"

int makeChild(Process_Data *pChild, int childNum)
{
	int i;
	pid_t pid;
	int fds[2];
	int ret;
	for(i=0;i < childNum;i++)
	{
		//初始化socketpair类型描述符，与pipe不同，每一端既可读又可写
		ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
		ERROR_CHECK(ret, -1, "socketpair");
		pid = fork();
		if(0 == pid)  //子进程
		{
			close(fds[1]); 
			ret = childHandle(fds[0]);
            if(-1 == ret){
                return -1;
            }
		}
		close(fds[0]); //父进程
		pChild[i].pid = pid;
		pChild[i].fd = fds[1];
		pChild[i].busy = 0;
	}
	return 0;
}

int childHandle(int fd)
{
    int ret;
	int newFd;
    int exitFlag;
	while(1){
        //开5个子进程，newFd为10，因内核控制信息，父子进程共享同一块文件描述符
		recvFd(fd, &newFd, &exitFlag);//接收到任务
#ifdef DEBUG
        printf("newFd = %d\n", newFd);
#endif
        if(exitFlag){
            ret = tranFile(newFd);//给客户端发送文件
            printf("I get task %d\n", newFd);
            if(-1 == ret){
                printf("tranFile not finish!\n\n");
                close(newFd);//断点续传在这搞
                continue;
            }
        	//newFd的值为10，socketFd为3，有五个子进程管道，为4-8，epfd也占1
        	printf("finish send file\n");
            close(newFd);
        }
    }
}



//typedef struct {
//    int code;
//    int precode;
//    int belong;
//}Workspace_t, *pWorkspace_t;

//void func_ls(pWorkspace_t);
//void func_cd(pWorkspace_t);
//int func_puts();
//int func_gets();
//int func_remove(Workspace_t);
//void func_pwd(Workspace_t workspace){
//    char pwd[512];
//    memset(pwd, 0, sizeof(pwd));
//    Workspace_t now = workspace;
//    if(0 == now.precode){
//        printf("/%s\n", );
//    }
//}


int tcpInit(int *pSocketFd, char *ip, char *port){
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
	ret = bind(socketFd, (struct sockaddr*)&ser, sizeof(ser));
	ERROR_CHECK(ret, -1, "bind");
	listen(socketFd, 10);//缓冲区的大小，一瞬间能够放入的客户端连接信息
	*pSocketFd = socketFd;
	return 0;
}


int tranFile(int newFd){
    Train_t train;
    int ret;
    //发送文件名
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

int sendFd(int sfd,int fd, int exitFlag)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct iovec iov[2];
	char buf2[10]="world";
	iov[0].iov_base= &exitFlag;
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

int recvFd(int sfd,int *fd, int *exitFlag)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct iovec iov[2];
	char buf2[10];
	iov[0].iov_base=exitFlag;
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

int connMySQL(int userID){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	const char* server="localhost";
	const char* user="root";
	const char* password="123";
	const char* database="NetDisk";//要访问的数据库名称
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		printf("Connected...\n");
	}
    char buf[128];
    while(1){
        memset(buf, 0, sizeof(buf));
        if(!strcmp(buf, "pwd")){
            pwd(userID, conn, res, row);
        }
        else if(!strcmp(buf, "exit")){
		    mysql_free_result(res);
            mysql_close(conn);
        }
    }
}

int pwd(int userID, MYSQL *conn, MYSQL_RES *res, MYSQL_ROW row){
	char query[300]="select * from FILE where ID='";
	int t;
	char stringID[512] = {0};
    sprintf(stringID, "%d", userID); //整数转字符串
    sprintf(query, "%s%s%s", query, stringID, "'"); //字符串合并
	//strcpy(query,"select * from FILE");
	//puts(query);
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
    else{
	//	printf("Query made...\n");
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)	{	
				//printf("num=%d\n",mysql_num_fields(res));//列数
				for(t=0;t<mysql_num_fields(res);t++){
						printf("%8s ",row[t]);
				}
				printf("\n");
			}
		}
        else{
			printf("Don't find data\n");
        }   
    }
    return 0;
}
