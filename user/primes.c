#include "kernel/types.h"
#include "user/user.h"

void solve(int* p_left)
{
    close(p_left[1]);//关掉不用的文件描述符
    //读到的第一个数打印出来，并在之后进行模运算
    int first;
    read(p_left[0],&first,sizeof(first));
    if(first==-1) return;
    printf("prime %d\n",first);

    int p_right[2];
    pipe(p_right);
    if(fork()!=0)
    {
        //当前父进程
        //与first取模为0则舍去
        //读到-1就结束
        close(p_right[0]);
        int i;
        read(p_left[0],&i,sizeof i);
        while(i!=-1)
        {
            if(i%first!=0)
            {
                write(p_right[1],&i,sizeof i);
            }
            read(p_left[0],&i,sizeof i);
        }
        write(p_right[1],&i,sizeof i);
        close(p_right[1]);
        close(p_left[0]);
        wait(0);
    }
    else
    {
        close(p_left[0]);
        solve(p_right);
        close(p_right[0]);
    }
    exit(0);
}

int main(int argc,char* argv[])
{
    int to = atoi(argv[1]);
    if(argc>2||to<=1)
    {
        printf("illegal value!\n");
        exit(1);
    }

    int p[2];
    pipe(p);
    if(fork()!=0)
    {
        //第一层父进程
        close(p[0]);
        for(int i=2;i<=to;++i)
            write(p[1],&i,sizeof i);
        int i=-1;
        write(p[1],&i,sizeof i);//写一个-1充当哨兵，代表结束
        close(p[1]);
        wait(0);
    }
    else
        solve(p);
    exit(0);
}