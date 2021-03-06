#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"

#define SIZE 1024*1024*100

typedef struct{//火车头协议
    int dataLen; 
    char buf[1000]; 
}Train_t;

typedef struct{
    pthread_t *pth_arr; //pthId
    int thread_num; //线程数
    pthread_cond_t cond;//条件变量
    que_t que; //队列
    short start_flag;//未启动为0，启动为1
}factory_t, *pFactory_t;

typedef struct{
    char salt[20];
    char crypt_code[512];
    char token[100];
}UserInfo_t, *pUserInfo_t;

typedef struct{
    int lvl;
    char pathNow[100];
}Dir, *pDir;

void* thread_func(void*);
void factory_init(pFactory_t, int, int);
void factory_start(pFactory_t);
int tcpInit(int*, char*, char*, int);
int tran_file(int, char*, char*);//download
int tran_file2(int, char*, char*);//upload
int recvCycle(int, void*, int);
void removeFile(int, char*, Dir);
int getls(int, Dir);
int login_query(char*, pUserInfo_t);
int login(pNode_t, char*, pDir);
int gettime(char*, int);
int Compute_file_md5(const char *, char *);
int getcd(int, char*, char*, pDir);
int judgeDir(char*);
int getpwd(int, Dir);
int create_token(char *, char *);
int update_mysql(char *, char *);
int childlogin(char*, pDir);
int update_dir(char *, char *);
int query_current(char*, pDir);

#endif
