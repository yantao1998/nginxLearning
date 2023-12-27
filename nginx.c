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
void sig_quit(int signo)
{
    printf("收到了SIGQUIT信号！\n");
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
    sigset_t newmask,oldmask,pendmask;
    if(signal(SIGQUIT,sig_quit) == SIG_ERR)
    {
        printf("无法捕捉SIGQUIT信号!\n");
        exit(1);
    }
    sigemptyset(&newmask);//将信号集置空
    sigaddset(&newmask,SIGQUIT);//设置屏蔽SIGQUIT信号
    if(sigprocmask(SIG_BLOCK,&newmask,&oldmask) < 0)
    {
        printf("sigprocmask(SIG_BLOCK)失败!\n");
        exit(0);
    }
    printf("我要开始休息10s了 ----begin----,此时我无法接受SIG_QUIT信号!\n");
    sleep(10);
    printf("休息结束！\n");
    if(sigismember(&newmask,SIGQUIT))
    {
        printf("SIGQUIT信号被屏蔽了!\n");
    }
    else
    {
        printf("SIGQUIT信号没有被屏蔽!\n");
    }
    if(sigismember(&newmask,SIGHUP))
    {
        printf("SIGHUP信号被屏蔽了!\n");
    }
    else
    {
        printf("SIGHUP信号没有被屏蔽!\n");
    }
    if(sigprocmask(SIG_SETMASK,&oldmask,NULL) < 0)
    {
        printf("sigprocmask(SIG_SETMASK)失败!\n");
        exit(1);
    }
    printf("sigprocmask(SIG_SETMASK)成功!\n");
    if(sigismember(&oldmask,SIGQUIT))
    {
        printf("SIGQUIT信号被屏蔽了!\n");
    }
    else
    {
        printf("SIGQUIT信号没有被屏蔽，您可以发送了，我要休息十秒！!\n");
        int mysl = sleep(10);
        if(mysl > 0)
        {
            printf("sleep还剩%ds\n",mysl);
        }
    }
    return 0;
}