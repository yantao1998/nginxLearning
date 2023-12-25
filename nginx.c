#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void sig_usr(int signo)
{
    if(signo == SIGUSR1)
    {
        printf("收到了SIGUSR1信号！\n");
    }
    else if(signo == SIGUSR2)
    {
        printf("收到了SIGUSR2信号!\n");
    }
    else
    {
        printf("收到了未捕捉到的信号！\n");
    }
}
int main()
{
    //注册信号
    if(signal(SIGUSR1,sig_usr) == SIG_ERR)
    {
        printf("无法捕捉到SIGUSR1信号\n");
    }
    if(signal(SIGUSR2,sig_usr) == SIG_ERR)
    {
        printf("无法捕捉到SIGUSR2信号\n");
    }
    pid_t pid;
    printf("非常高兴，和大家一起学习本书！\n");
    for(;;)
    {
       sleep(1);
       printf("进程休息1秒！\n"); 
    }
    printf("进程退出！再见！\n");
    return 0;
}