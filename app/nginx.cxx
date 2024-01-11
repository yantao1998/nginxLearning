

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "ngx_func.h"  //头文件路径，已经使用gcc -I参数指定了
#include "ngx_signal.h"
#include "ngx_global.h"
#include "ngx_c_conf.h"

//本文件用的函数声明
static void freeresource();

size_t g_argvneedmem = 0;
int g_os_argc;
char **g_os_argv;
char* gp_envmem = NULL;
int g_environlen = 0;

//和进程本身有关的全局量
pid_t ngx_pid;               //当前进程的pid
pid_t ngx_parent;               //当前进程的pid

int main(int argc, char *const *argv)
{      
    int exitcode = 0;           //退出代码，先给0表示正常退出
    //(1)无伤大雅也不需要释放的放最上边    
    ngx_pid = getpid();         //取得进程pid
    ngx_parent = getppid();
    g_argvneedmem = 0;
    for(int i = 0;i < argc;++i)
    {
        g_argvneedmem += strlen(argv[i]) + 1;
    }
    g_os_argv = (char **)argv;  
    g_os_argc = argc;      
    printf("非常高兴，我们大家一起学习《linux C++通信架构实战》\n");    


    //(2)初始化失败，就要直接退出的
    //配置文件必须最先要，后边初始化啥的都用，所以先把配置读出来，供后续使用 
    CConfig *p_config = CConfig::GetInstance(); //单例类
    if(p_config->Load("nginx.conf") == false) //把配置文件内容载入到内存        
    {        
        ngx_log_stderr(0,"配置文件[%s]载入失败，退出!","nginx.conf");
        //exit(1);终止进程，在main中出现和return效果一样 ,exit(0)表示程序正常, exit(1)/exit(-1)表示程序异常退出，exit(2)表示表示系统找不到指定的文件
        exitcode = 2; //标记找不到文件
        goto lblexit;
    }
    //(3)一些初始化函数，准备放这里
    ngx_log_init();             //日志初始化(创建/打开日志文件)
    if(ngx_init_signals() != 0) //信号初始化
    {
        exitcode = 1;
        goto lblexit;
    }
    //(4)一些不好归类的其他类别的代码，准备放这里
    ngx_init_setproctitle();    //把环境变量搬家
    printf("argc=%d,argv[0]=%s\n",argc,argv[0]);
    //要保证所有命令行参数从下面这行代码开始都不再使用，才能调用ngx_setproctitle函数，因为调用后，命令行参数的内容可能会被覆盖掉

    //(5)开始正式的主工作流程，主流程一致在下边这个函数里循环，暂时不会走下来，资源释放啥的日后再慢慢完善和考虑
    ngx_master_process_cycle(); //不管父进程还是子进程，正常工作期间都在这个函数里循环；
    // ngx_log_stderr(0, "invalid option: \"%s\"", argv[0]);  //nginx: invalid option: "./nginx"
    // ngx_log_stderr(0, "invalid option: %10d", 21);         //nginx: invalid option:         21  ---21前面有8个空格
    // ngx_log_stderr(0, "invalid option: %.6f", 21.378);     //nginx: invalid option: 21.378000   ---%.这种只跟f配合有效，往末尾填充0
    // ngx_log_stderr(0, "invalid option: %.6f", 12.999);     //nginx: invalid option: 12.999000
    // ngx_log_stderr(15, "invalid option: %s , %d", "testInfo",326); 
    //     //测试ngx_log_error_core函数的调用
    // ngx_log_error_core(5,8,"这个XXX工作的有问题，显示的结果=%s","YYYY");
lblexit:
    //(5)该释放的资源要释放掉
    freeresource();  //一系列的main返回前的释放动作函数
    printf("程序退出，再见!\n");
    return exitcode;
}

//专门在程序执行末尾释放资源的函数【一系列的main返回前的释放动作函数】
void freeresource()
{
    //(1)对于因为设置可执行程序标题导致的环境变量分配的内存，我们应该释放
    if(gp_envmem)
    {
        delete []gp_envmem;
        gp_envmem = NULL;
    }

    //(2)关闭日志文件
    if(ngx_log.fd != STDERR_FILENO && ngx_log.fd != -1)  
    {        
        close(ngx_log.fd); //不用判断结果了
        ngx_log.fd = -1; //标记下，防止被再次close吧        
    }
}


