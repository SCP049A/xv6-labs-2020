#include "kernel/types.h"
#include "user/user.h"

int main(int argc,char* argv[])
{
    if(argc!=2)//第一个参数argv[0]代表程序本身的名称
    {
        fprintf(2, "must 1 argument for sleep\n");
        exit(1);
    }

    int num = atoi(argv[1]);
    printf("(nothing happens for a little while)\n");
    sleep(num);
    exit(0);
}