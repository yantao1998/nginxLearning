//守护进程代码编写

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "ngx_func.h"
#include "ngx_macro.h"
#include "ngx_global.h"

int ngx_daemon()
{
    switch(fork())
    {
        case -1:
            ngx_log_error_core(NGX_LOG_EMERG,errno,"ngx_daemon()中fork()失败!");
            return -1;
        case 0:break;
        default:
            return 1;
    }
    //子进程流程
    ngx_parent = ngx_pid;
    ngx_pid = getpid();

    //设置新的会话组,不受启动终端影响
    if(setsid() == -1)
    {
        ngx_log_error_core(NGX_LOG_EMERG,errno,"ngx_daemon()中setsid()失败!");
        return -1;
    }
    //设置文件权限
    umask(0);
    //打开黑洞设备
    int fd = open("/dev/null",O_RDWR);
    if(fd == -1)
    {
        ngx_log_error_core(NGX_LOG_EMERG,errno,"ngx_daemon()中open(\"/dev/null\")失败！");
        return -1;
    }
    if(dup2(fd,STDIN_FILENO) == -1)
    {
        ngx_log_error_core(NGX_LOG_EMERG,errno,"ngx_daemon()中dup2(STDIN)失败!");
        return -1;
    }
    if(dup2(fd,STDOUT_FILENO) == -1)
    {
        ngx_log_error_core(NGX_LOG_EMERG,errno,"ngx_daemon()中dup2(STDOUT)失败!");
        return -1;
    }
    if(fd > STDERR_FILENO)
    {
        if(close(fd) == -1)
        {
            ngx_log_error_core(NGX_LOG_EMERG,errno, "ngx_daemon()中close(fd)失败!");
            return -1;
        }
    }
    return 0;
}