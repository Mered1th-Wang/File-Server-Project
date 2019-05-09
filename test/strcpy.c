#include <func.h>

int main()
{
    char a[] = "/home/wj/YunPan/src2/file";
    printf("%ld\n", strlen(a));
    char a_beg[6] = {0};
    strcpy(a_beg, a + 4);
    puts(a_beg);
    return 0;
}

