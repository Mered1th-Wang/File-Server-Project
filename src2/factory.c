#include "factory.h"

void factory_init(pFactory_t pf, int thread_num, int capacity){
    memset(pf, 0, sizeof(factory_t));
    pf->pth_arr = (pthread_t*)calloc(thread_num, sizeof(pthread_t));
    pf->thread_num = thread_num;
    pthread_cond_init(&pf->cond, NULL);
    que_init(&pf->que, capacity);
}

void factory_start(pFactory_t pf){
    int i;
    //  pf->start_flag = 1;
    for(i = 0;i < pf->thread_num; i++){
        pthread_create(pf->pth_arr + i, NULL, thread_func, pf);
    }
}

int judgeDir(char* name)
{
    struct stat buf;
    stat(name, &buf);
    if(S_ISDIR(buf.st_mode)){
        //printf("dir\n");
        return 1;
    }
    else{
        //printf("file\n");
        return 0;
    }
}

void removeFile(int newFd, char* FILENAME, Dir current)
{
    Train_t train;
    char pathname[100] = {0};
    sprintf(pathname, "%s%s%s", current.pathNow, "/", FILENAME);
    train.dataLen = unlink(pathname);
    send(newFd, &train, 4, 0);
    return ;
}

int getls(int newFd, Dir current){
    Train_t train;
    memset(&train, 0, sizeof(Train_t));
    DIR *dir;
    char pathname[500] = {0};
    //sprintf(pathname, "%s%s", "../src2/file/", name);
    int len = strlen(current.pathNow), i, charcount = 0;
    char last[100] = {0};
    int j = 0;
    puts(current.pathNow);
    for(i = len - 1; i >= 0; i--){
        if(current.pathNow[i] == '/'){
            charcount++;
            if(2 == charcount) break;
        }
        else{
            last[j++] = current.pathNow[i];
        }
    }
    printf("last = %s\n", last);
    len = strlen(last);
    printf("len = %d\n", len);
    char c;
    for(i = 0; i < len / 2; i++){
        c = last[i];
        last[i] = last[len - i - 1];
        last[len - i - 1] = c;
    }
    printf("last = %s\n", last);
    sprintf(pathname, "%s%s%s", current.pathNow, "../", last);
    puts(pathname);
    dir = opendir(pathname);
    ERROR_CHECK(dir, NULL, "opendir");
    struct dirent *p;
    int count = 0;
    while((p = readdir(dir))){
        if(strcmp(p->d_name, ".") != 0 && strcmp(p->d_name, "..") != 0){
            if(count == 0){
                sprintf(train.buf, "%s%-15s", train.buf, p->d_name);
                count++;
                continue;
            }
            if(count == 5){
                sprintf(train.buf, "%s%s", train.buf, "\n");
                count = 0;
            }
            sprintf(train.buf, "%s%s%-15s", train.buf, " ", p->d_name);
            count++;
        }
    }
    closedir(dir);
    train.dataLen = strlen(train.buf);
    train.buf[train.dataLen] = '\0';
    train.dataLen += 1;
    int ret = send(newFd, &train, 4 + train.dataLen, 0);
    ERROR_CHECK(ret, -1, "send");
    return 0;
}

int gettime(char timeNow[], int size){
    memset(timeNow, 0, size);
    time_t now;
    now = time(NULL);
    ctime_r(&now, timeNow);
    printf("\n%s", timeNow);
    return 0;
}

int getcd(int newFd, char* buf, pDir current){
    Train_t train;
    int ret;
    memset(&train, 0, sizeof(train));
    //printf("%s\n", current->pathNow);
    int len = strlen(current->pathNow);
    int i, count = 0;
    if(!strcmp(buf, "..")){
        if(0 == current->lvl) {
            printf("lvl == 0\n");
            train.dataLen = -1;
        }
        else{
            for(i = len - 1; i >= 0 ; i--){
                if(current->pathNow[i] == '/'){
                    count++;
                    if(2 == count) break;
                }
            }
            i++;
            current->pathNow[i] = '\0';
            current->lvl--;
            train.dataLen = 0;
        }
    }
    else {
        char tempPath[500] = {0};
        strcpy(tempPath, current->pathNow);
        sprintf(tempPath, "%s%s%s", tempPath, buf, "/");
        printf("tempPath = %s\n", tempPath);
        if(judgeDir(tempPath)) {
            printf("dir!\n");
            sprintf(current->pathNow, "%s%s%s", current->pathNow, buf, "/");
            current->lvl++;
            train.dataLen = 0;
        }
        else{
            train.dataLen = -2;
        }
    }
    ret = send(newFd, &train, 4, 0);
    ERROR_CHECK(ret, -1, "send");
    printf("%s\n", current->pathNow);
}
