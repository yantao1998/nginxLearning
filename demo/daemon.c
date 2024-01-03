#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

int ngx_daemon()
{
    //创建守护进程
    //1：父进程退出，子进程执行
    switch(fork())
    {
        case -1:
        //创建子进程失败，可以写日志
        return -1;
        case 0:
        //子进程直接跳出
        break;
        default:
        //父进程直接退出
        exit(0);
    }
    //2：脱离终端，终端关闭，不会收到终端来的信号（终端退出）
    if(setsid() == -1)
    {
        //记录错误日志
        return -1;
    }
    //3:设置文件权限
    umask(0);//设置为0，不要让它来限制文件权限，以免引起混乱
    //4:重定向输入输出
    int fd = open("/dev/null",O_RDWR);//打开黑洞设备
    if(fd == -1)
    {
        //记录错误日志
        return -1;
    }
    //将输入重定位到黑洞设备，会先让STDIN打开的设备关闭，然后让STDIN指向黑洞设备
    if(dup2(fd,STDIN_FILENO) == -1)
    {
        //记录错误日志
        return -1;  
    }
    //将输出重定位到黑洞设备，会先让STDIN打开的设备关闭，然后让STDIN指向黑洞设备
    if(dup2(fd,STDOUT_FILENO) == -1)
    {
        //记录错误日志
        return -1;  
    }
    if(fd > STDERR_FILENO)
    {
        if(close(fd) == -1)
        {
            //记录错误日志
            return -1;
        }
    }
    return 1;
}   

int main()
{
    if(ngx_daemon() != 1)
    {
        //创建守护进程失败，做失败后的处理
        return 1;
    }
    else
    {
        //创建守护进程成功，执行守护进程要做的工作
        for(;;)
        {
            sleep(1);
            printf("休息1秒，进程id=%d!\n",getpid());
        }
    }
    return 0;
}