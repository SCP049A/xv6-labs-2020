//exec.c:replace a process with an executable file
#include"kernel/types.h"
#include"user/user.h"

int main()
{
    char *argv[]={"echo","this","is","echo",0};//这里的0指的是void*(0),也就是NULL，用于让exec知道参数停在哪个位置
    exec("echo",argv);
    printf("exec failed!\n");
    exit(0);
}