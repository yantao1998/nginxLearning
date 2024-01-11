#include <errno.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <string.h>

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_c_conf.h"


//变量声明
static u_char  master_process[] = "master process";


//函数声明
static void ngx_start_worker_processes(int threadnums);
static int ngx_spawn_process(int inum,const char *pprocname);
static void ngx_worker_process_cycle(int inum,const char *pprocname);
static void ngx_worker_process_init(int inum);

//描述：创建worker子进程
void   ngx_master_process_cycle()
{
    sigset_t set;//信号集
    sigemptyset(&set);

    //下列这些信号在执行本函数期间不希望收到【考虑到官方nginx中有这些信号，老师就都搬过来了】（保护不希望由信号中断的代码临界区）
    //建议fork()子进程时学习这种写法，防止信号的干扰；
    sigaddset(&set, SIGCHLD);     //子进程状态改变
    sigaddset(&set, SIGALRM);     //定时器超时
    sigaddset(&set, SIGIO);       //异步I/O
    sigaddset(&set, SIGINT);      //终端中断符
    sigaddset(&set, SIGHUP);      //连接断开
    sigaddset(&set, SIGUSR1);     //用户定义信号
    sigaddset(&set, SIGUSR2);     //用户定义信号
    sigaddset(&set, SIGWINCH);    //终端窗口大小改变
    sigaddset(&set, SIGTERM);     //终止
    sigaddset(&set, SIGQUIT);     //终端退出符
    //.........可以根据开发的实际需要往其中添加其他要屏蔽的信号......
    if(sigprocmask(SIG_BLOCK,&set,NULL) == -1)
    {
        ngx_log_error_core(NGX_LOG_ALERT,errno,"ngx_master_process_cycle()中sigprocmask()失败!");
    }

    //首先我设置主进程标题---------begin
    size_t size = sizeof(master_process);
    size += g_argvneedmem;
    if(size < 1000)
    {
        char title[1000];
        strcpy(title,(const char*)master_process);
        strcat(title," ");
        for(int i = 0;i < g_os_argc;++i)
        {
            strcat(title,g_os_argv[i]);
        }
        ngx_setproctitle(title);
    }
    //首先我设置主进程标题---------end

    //从配置文件中读取要创建的worker进程数量
    CConfig *p_config = CConfig::GetInstance();
    int workprocess = p_config->GetIntDefault("WorkerProcesses",1);
    //创建子进程
    ngx_start_worker_processes(workprocess);

    sigemptyset(&set);
    if(sigprocmask(SIG_SETMASK,&set,NULL) == -1)
    {
        ngx_log_error_core(NGX_LOG_ALERT,errno,"ngx_master_process_init()中sigprocmask()失败!");
    }
    ngx_log_error_core(0,0,"haha--这是父进程，pid为%P",ngx_pid);
    for ( ;; ) 
    {
        sleep(1);
    //    usleep(100000);
        //ngx_log_error_core(0,0,"haha--这是父进程，pid为%P",ngx_pid);

        //a)根据给定的参数设置新的mask 并 阻塞当前进程【因为是个空集，所以不阻塞任何信号】
        //b)此时，一旦收到信号，便恢复原先的信号屏蔽【我们原来的mask在上边设置的，阻塞了多达10个信号，从而保证我下边的执行流程不会再次被其他信号截断】
        //c)调用该信号对应的信号处理函数
        //d)信号处理函数返回后，sigsuspend返回，使程序流程继续往下走
        //printf("for进来了！\n"); //发现，如果print不加\n，无法及时显示到屏幕上，是行缓存问题，以往没注意；可参考https://blog.csdn.net/qq_26093511/article/details/53255970

    //    sigsuspend(&set); //阻塞在这里，等待一个信号，此时进程是挂起的，不占用cpu时间，只有收到信号才会被唤醒（返回）；
                         //此时master进程完全靠信号驱动干活    

    //    printf("执行到sigsuspend()下边来了\n");        
        
///        printf("master进程休息1秒\n");      
        //sleep(1); //休息1秒        
        //以后扩充.......

    }// end for(;;)
    return;
}
static void ngx_start_worker_processes(int threadnums)
{
    for(int i = 0;i < threadnums;++i)
    {
        ngx_spawn_process(i,"worker process");
    }
    return;
}
static int ngx_spawn_process(int inum,const char *pprocname)
{
    pid_t pid = fork();
    switch(pid)
    {
        case -1:ngx_log_error_core(NGX_LOG_ALERT,errno,"ngx_spawn_process()fork()产生子进程num=%d,procname=\"%s\"失败!",inum,pprocname);return -1;
        case 0:
            ngx_parent = ngx_pid;
            ngx_pid = getpid();
            ngx_worker_process_cycle(inum,pprocname);
        break;
        default:break;
    }
    return pid;
}
static void ngx_worker_process_cycle(int inum,const char *pprocname)
{
    ngx_worker_process_init(inum);
    ngx_setproctitle(pprocname);

    //暂时先放个死循环，我们在这个循环里一直不出来
    //setvbuf(stdout,NULL,_IONBF,0); //这个函数. 直接将printf缓冲区禁止， printf就直接输出了。
    ngx_log_error_core(0,0,"good--这是子进程，编号为%d,pid为%P！",inum,ngx_pid);
    for(;;)
    {

        //先sleep一下 以后扩充.......
        //printf("worker进程休息1秒");       
        //fflush(stdout); //刷新标准输出缓冲区，把输出缓冲区里的东西打印到标准输出设备上，则printf里的东西会立即输出；
        sleep(1); //休息1秒       
        //usleep(100000);
        //ngx_log_error_core(0,0,"good--这是子进程，编号为%d,pid为%P！",inum,ngx_pid);
        //printf("1212");
        //if(inum == 1)
        //{
            //ngx_log_stderr(0,"good--这是子进程，编号为%d,pid为%P",inum,ngx_pid); 
            //printf("good--这是子进程，编号为%d,pid为%d\r\n",inum,ngx_pid);
            //ngx_log_error_core(0,0,"good--这是子进程，编号为%d",inum,ngx_pid);
            //printf("我的测试哈inum=%d",inum++);
            //fflush(stdout);
        //}
            
        //ngx_log_stderr(0,"good--这是子进程，pid为%P",ngx_pid); 
        //ngx_log_error_core(0,0,"good--这是子进程，编号为%d,pid为%P",inum,ngx_pid);

    } //end for(;;)
    return;

}
static void ngx_worker_process_init(int inum)
{
    sigset_t set;
    sigemptyset(&set);
    if(sigprocmask(SIG_SETMASK,&set,NULL) == -1)
    {
        ngx_log_error_core(NGX_LOG_ALERT,errno,"ngx_worker_process_init()中sigprocmask()失败!");
    }
    return;
}