#include <func.h>

int main(int argc,char* argv[])
{
    ARGS_CHECK(argc,2);
    DIR *dir;
    dir=opendir(argv[1]);
    ERROR_CHECK(dir,NULL,"opendir");
    struct dirent *p;
    while((p=readdir(dir)))
    {
        printf("%s ", p->d_name);
    }
    printf("\n");
    closedir(dir);
    return 0;
}
