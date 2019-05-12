#include <func.h>

void* pthread_Func(void* p){
    printf("child: %d\n", getpid());
    pthread_exit(NULL);
}

int main()
{
    printf("parent: %d\n", getpid());    
    pthread_t tid;
    pthread_create(&tid, NULL, pthread_Func, NULL);
    pthread_join(tid, NULL);
    return 0;
}

