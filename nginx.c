/*
 * @Author: yantao1998 1265640301@qq.com
 * @Date: 2023-12-25 15:52:16
 * @LastEditors: yantao1998 1265640301@qq.com
 * @LastEditTime: 2023-12-27 16:58:15
 * @FilePath: \nginxLearning\nginx.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void sig_usr(int signo)
{
    int status;
    switch(signo)
    {
        case SIGUSR1:
        printf("收到了SIGUSR1信号,进程ID=%d\n",getpid());
        break;
        case SIGCHLD:
        printf("收到了SIGCHLD信号,进程id=%d\n",getpid());
        while(1)
        {
            pid_t pid = waitpid(-1,&status,WNOHANG);
            if(pid <= 0)
            {
                break;
            }
        }
        break;
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
    pid_t pid;
    printf("进程开始执行!\n");
    if(signal(SIGUSR1,sig_usr) == SIG_ERR)
    {
        printf("无法捕捉到SIGUSR1信号！\n");
        exit(1);
    }
    if(signal(SIGCHLD,sig_usr) == SIG_ERR)
    {
        printf("无法捕捉到SIGUSR1信号！\n");
    }
    //创建一个子进程
    pid = fork();
    if(pid < 0)
    {
        printf("子进程创建失败,很遗憾!\n");
        exit(1);
    }
    //父子进程同时执行代码
    for(;;)
    {
        sleep(1);
        printf("休息1s，进程id=%d\n",getpid());
    }
    //父子进程分流执行   
    pid = fork();
    if(pid < 0)
    {
        printf("子进程创建失败,很遗憾!\n");
        exit(1);
    }
    if(pid == 0)
    {
        //子进程执行代码
        while(1)
        {
            g_mygbltest++;
            sleep(1);
            printf("我是子进程，进程id=%d,g_mygbltest=%d\n",getpid(),g_mygbltest);
        }
    }else{
        //父进程执行代码
        while(1)
        {
            g_mygbltest++;
            sleep(5);
            printf("我是父进程，进程id=%d,g_mygbltest=%d\n",getpid(),g_mygbltest);
        }
    }     
    return 0;
}