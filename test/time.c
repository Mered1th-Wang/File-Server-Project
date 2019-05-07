#include <func.h>

int main()
{
    char timebuf[50] = {0};
    time_t now;
    now = time(NULL);
    ctime_r(&now, timebuf);
    printf("%s\n", timebuf);
    return 0;
}

