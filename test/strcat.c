#include <func.h>


int main()
{
    char s[] = "abc";
    char filename[50] = "567";
    //sprintf(filename, "%s%s", "./file/", s);
    strcat(s, filename);
    printf("%s\n", s);

    return 0;
}

