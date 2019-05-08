#include <func.h>

int main()
{
    char last[50] = "abc";
    int len = strlen(last), i;
    char c;
    puts(last);
    for(i = 0; i < len / 2; i++){
        c = last[i];
        last[i] = last[len - i -1];
        last[len - i - 1] = c;
    }
    puts(last);
    return 0;
}
