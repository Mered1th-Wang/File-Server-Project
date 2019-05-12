#include <func.h>

int main()
{
    struct itimerval it, oldit;
    it.it_value.tv_sec = 1;
    it.it_value.tv_usec = 0;

    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &it, &oldit);
    for(int i = 0;;i++)
        printf("%d\n", i);
    return 0;
}

