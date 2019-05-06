#include <func.h>

int main()
{
    char path[512] = {0};
    getcwd(path, sizeof(path));
    printf("%s\n", path);
    printf("%s\n", getcwd(NULL, 0));
    return 0;
}

