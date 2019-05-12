#ifndef __FUNCTION_H__
#define __FUNCTION_H__
#include <linux/tcp.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/msg.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/sendfile.h>
#include <crypt.h>
#include <shadow.h>
#include <errno.h>
#define SIZE 1024*1024*100

#define ARGS_CHECK(argc,val) {if(argc!=val) \
	{printf("error args\n");return -1;}}
#define ERROR_CHECK(ret,retval,funcName) {if(ret==retval) \
	{printf("LINE:%d ", __LINE__);fflush(stdout);perror(funcName);return -1;}}
#define THREAD_ERROR_CHECK(ret,funcName) {if(ret!=0) \
	{printf("%s:%s\n",funcName,strerror(ret));return -1;}}

typedef struct train{
    int dataLen;
    char buf[1000];
}Train_t;

typedef struct node{
    int new_fd;
    struct node *pNext;
    int option;
    char buf[1000];
}Node_t, *pNode_t;

typedef struct{
    pNode_t que_head, que_tail;
    int que_capacity;
    int que_size;
    pthread_mutex_t mutex;
}que_t, *pque_t;

typedef struct {
    pthread_t pid;
    pthread_cond_t cond;
    que_t que;
}factory_t, *pFactory_t;

typedef struct{
    int lvl;
    char pathNow[100];
}Dir, *pDir;


int recvCycle(int, void*, int);
int downloadFile(int, char*);
int uploadFile(int, char*);
int tcpInit_client(int*, char*, char*);
int getdir();
int login_client(int, char*);
int getcd(int);
int getpwd(int);
int gets(int, char*);
int putsfile(int, char*);
int removefile(int);
int getls(int);

void que_init(pque_t, int);
int que_get(pque_t, pNode_t*);
void que_insert(pque_t, pNode_t);
void* thread_func(void*);
void factory_init(pFactory_t, int);
void factory_start(pFactory_t);
int gettime(char, int);
int query_token(char*);
int login_childclient(int, char *);

#endif
