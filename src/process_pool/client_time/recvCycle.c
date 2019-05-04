#include "function.h"

int recvCycle(int newFd, void* p, int len){
    int total = 0;
    int ret;
    char *pStart = (char*)p;   
    while(total < len){
        ret = recv(newFd, pStart + total, len - total, 0);
        total = total + ret;
    }
    
    return 0;
}

