

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_global.h"



typedef struct
{
    int signo;
    const char* signame;
    void (*handler)(int signo,siginfo_t* siginfo,void *ucontext);
} ngx_signal_t;

static void ngx_signal_handler(int signo,siginfo_t* siginfo,void* ucontext);
static void ngx_process_get_status(void);

ngx_signal_t signals[] = {
    // signo      signame             handler
    { SIGHUP,    "SIGHUP",           ngx_signal_handler },        //终端断开信号，对于守护进程常用于reload重载配置文件通知--标识1
    { SIGINT,    "SIGINT",           ngx_signal_handler },        //标识2   
	{ SIGTERM,   "SIGTERM",          ngx_signal_handler },        //标识15
    { SIGCHLD,   "SIGCHLD",          ngx_signal_handler },        //子进程退出时，父进程会收到这个信号--标识17
    { SIGQUIT,   "SIGQUIT",          ngx_signal_handler },        //标识3
    { SIGIO,     "SIGIO",            ngx_signal_handler },        //指示一个异步I/O事件【通用异步I/O信号】
    { SIGSYS,    "SIGSYS, SIG_IGN",  NULL               },        //我们想忽略这个信号，SIGSYS表示收到了一个无效系统调用，如果我们不忽略，进程会被操作系统杀死，--标识31
                                                                  //所以我们把handler设置为NULL，代表 我要求忽略这个信号，请求操作系统不要执行缺省的该信号处理动作（杀掉我）
    //...日后根据需要再继续增加
    { 0,         NULL,               NULL               } 
};
//初始化信号的函数，用于注册信号处理程序
//返回值：0成功  ，-1失败
int ngx_init_signals()
{
    struct sigaction sa;
    for(ngx_signal_t* sig = signals;sig->signo != 0;++sig)
    {
        memset(&sa,0,sizeof(struct sigaction));
        if(sig->handler)
        {
            sa.sa_sigaction = sig->handler;
            sa.sa_flags = SA_SIGINFO;
        }else{
            sa.sa_handler = SIG_IGN;
        }
        sigemptyset(&sa.sa_mask);
        if(sigaction(sig->signo,&sa,NULL) == -1)
        {
            ngx_log_error_core(NGX_LOG_EMERG,errno,"sigaction(%s) failed",sig->signame); //显示到日志文件中去的 
            return -1; //有失败就直接返回
        }else{
            ngx_log_stderr(0,"sigaction(%s) succed!",sig->signame); //直接往屏幕上打印看看 ，不需要时可以去掉
        }
    }
}
//信号处理函数
//siginfo：这个系统定义的结构中包含了信号产生原因的有关信息
void ngx_signal_handler(int signo,siginfo_t* siginfo,void* ucontext)
{
    ngx_signal_t *sig;
    char *action;
    for(sig = signals;sig->signo != 0;++sig)
    {
        if(sig->signo == signo)
        {
            break;
        }
    }
    action = (char*)"";
    if(ngx_process == NGX_PROCESS_MASTER)//master进程，管理进程，处理的信号一般会比较多 
    {
        switch(signo)
        {
            case SIGCHLD:
              //标记子进程状态变化，日后master主进程的for(;;)循环中可能会用到这个变量【比如重新产生一个子进程】
                ngx_reap = 1;
                break;
            default:
                break;
        }

    }
    else if(ngx_process == NGX_PROCESS_WORKER)
    {
        //worker进程，具体干活的进程，处理的信号相对比较少

    }else{
        //非master非worker进程，先啥也不干        
    }
    //记录信号的日志信息
    if(siginfo && siginfo->si_pid)
    {
        ngx_log_error_core(NGX_LOG_NOTICE,0,"signal %d (%s) received from %P%s", signo, sig->signame, siginfo->si_pid, action);
    }else{
        //没有发送该信号的进程id，所以不显示发送该信号的进程id
        ngx_log_error_core(NGX_LOG_NOTICE,0,"signal %d (%s) received %s",signo, sig->signame, action);
    }
    if(signo == SIGCHLD)
    {
        ngx_process_get_status();
    }
}
static void ngx_process_get_status(void)
{
    pid_t pid;
    int status;
    int err;
    int one = 0;//应该是标记信号正常处理过一次
    while(1)
    {
        pid = waitpid(-1,&status,WNOHANG);
        switch(pid)
        {
            case 0:return;
            case -1:
                err = errno;
                if(err == EINTR)
                {
                    continue;
                }
                if(err == ECHILD && one)
                {
                    return;
                }
                if(err == ECHILD)
                {
                    ngx_log_error_core(NGX_LOG_INFO,err,"waitpid() failed!");
                    return;
                }
                ngx_log_error_core(NGX_LOG_ALERT,err,"waitpid() failed!");
                return;
            default:break;
        }
        //正常回收了一个子进程
        one = 1;
        if(WTERMSIG(status))//获取使子进程终止的信号编号
        {
            ngx_log_error_core(NGX_LOG_ALERT,0,"pid = %P exited on signal %d!",pid,WTERMSIG(status));
        }
        else
        {
            ngx_log_error_core(NGX_LOG_NOTICE,0,"pid = %P exited with code %d!",pid,WEXITSTATUS(status)); //WEXITSTATUS()获取子进程传递给exit或者_exit参数的低八位
        }
    }
    return;
}