#include <func.h>

typedef struct{
    int dataLen;
    char buf[100];
}Train_t;

int main()
{
    Train_t train;
    int x = 5;
    train.dataLen = sizeof(x);
    memcpy(train.buf, &x, train.dataLen);
    printf("%s\n", train.buf);
    return 0;
}

