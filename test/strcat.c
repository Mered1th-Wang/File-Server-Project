#include <func.h>


int main()
{
    char s[] = "abc";
    char filename[50] = {0};
    sprintf(filename, "%s%s", "./file/", s);
    printf("%s\n", filename);

    return 0;
}

