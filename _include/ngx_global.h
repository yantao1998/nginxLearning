#ifndef __NGX_GLOBAL_H__
#define __NGX_GLOBAL_H__
#include <sys/types.h>
// 通用的宏定义
//配置项的结构体单元
typedef struct
{
    char ItemName[50];//配置项名称
    char ItemContent[500];//配置项内容
}CConfItem,*LPCConfItem;

//和运行日志相关 
typedef struct
{
	int    log_level;   //日志级别 或者日志类型，ngx_macro.h里分0-8共9个级别
	int    fd;          //日志文件描述符

}ngx_log_t;
//外部全局量声明
extern char ** g_os_argv;
extern char * gp_envmem;
extern int g_environlen;
extern size_t g_argvneedmem;
extern int g_os_argc;

extern pid_t       ngx_pid;
extern pid_t       ngx_parent;
extern ngx_log_t   ngx_log;
#endif