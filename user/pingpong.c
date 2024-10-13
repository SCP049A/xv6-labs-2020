#include "kernel/types.h"
#include "user/user.h"

int main()
{
    int p_child[2];//孩子写入，父进程读
    int p_parent[2];//父进程写入，孩子读
    char buffer[]={'a'};//来回传输的字符数组，占一字节
    int len = sizeof(buffer);
    pipe(p_child);
    pipe(p_parent);

    if(fork()==0)
    {
        //把没用到的文件描述符关闭
        //注：调用fork后，子进程会复制父进程的整个地址空间，并从fork返回的位置继续执行
        //他们的文件描述符是共享的
        close(p_child[0]);
        close(p_parent[1]);
        if(read(p_parent[0],buffer,len)!=len)
        {
            printf("child-->parent read error!\n");
            exit(1);
        }
        printf("%d: received ping\n",getpid());

        if(write(p_child[1],buffer,len)!=len)
        {
            printf("child-->parent write error!\n");
            exit(1);
        }
        exit(0);
    }
    //父进程
    close(p_child[1]);
    close(p_parent[0]);
    if(write(p_parent[1],buffer,len)!=len)
    {
        printf("parent-->child write error!\n");
        exit(1);
    }
    if(read(p_child[0],buffer,len)!=len)
    {
        printf("parent-->child read error!\n");
        exit(1);
    }
    printf("%d: received pong\n",getpid());
    wait(0);
    exit(0);
}