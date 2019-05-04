#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include <mysql/mysql.h>                                                                                                                   

typedef struct {
    int code;
    int precode;
    int belong;
}Workspace_t, *pWorkspace_t;

typedef struct{
    pid_t pid;
    int fd;//子进程的管道对端
    short busy; //子进程是否忙碌，0代表非忙碌，1代表忙碌
}Process_Data;

typedef struct{
    int dataLen;
    char buf[1000];
}Train_t;

#define FILENAME "file"

int makeChild(Process_Data*, int);
int tcpInit(int*, char*, char*);
int childHandle(int);
int sendFd(int, int, int);
int recvFd(int, int*, int*);
int tranFile(int);


//menu 
//void func_ls(pWorkspace_t);
//void func_cd(pWorkspace_t);
//int func_puts();
//int func_gets();
//int func_remove(Workspace_t);
//void func_pwd(Workspace_t);

//mysql
int connMySQL(int);
int pwd(int, MYSQL*, MYSQL_RES*, MYSQL_ROW);


#endif
