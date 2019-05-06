#include <func.h>

#define STR_LEN 10 //定义随机输出的字符串长度

int main(){
    char str[STR_LEN + 1] = {0};
    int i, flag;
    srand(time(NULL)); //设置随机数种子
    for(i = 0; i < STR_LEN; i++){
        flag = rand() % 3;
        switch(flag)
        {
            case 0:
                str[i] = rand() % 26 + 'a';
                break;
            case 1:
                str[i] = rand() % 26 + 'A';
                break;
            case 2:
                str[i] = rand() % 10 + '0';
                break;
        }
    }
    printf("%s\n", str); //输出生成的随机数
}


