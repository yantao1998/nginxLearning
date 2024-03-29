﻿#ifndef __NGX_FUNC_H__
#define __NGX_FUNC_H__

#include <stdarg.h>
typedef unsigned char u_char;
//函数声明
void myconf();    
#define  MYVER  "1.2"
// 字符串相关函数
void Rtrim(char* str);//去除字符串左边空格
void Ltrim(char* str);//去除字符串右边空格

//设置可执行程序标题相关函数
void ngx_init_setproctitle();
void ngx_setproctitle(const char *title);

//和日志，打印输出有关
void   ngx_log_init();
void   ngx_log_stderr(int err, const char *fmt, ...);
void   ngx_log_error_core(int level,  int err, const char *fmt, ...);

u_char *ngx_log_errno(u_char *buf, u_char *last, int err);
u_char *ngx_slprintf(u_char *buf, u_char *last, const char *fmt, ...);
u_char *ngx_vslprintf(u_char *buf, u_char *last,const char *fmt,va_list args);

//和信号/主流程相关相关
int    ngx_init_signals();
void   ngx_master_process_cycle();
int    ngx_daemon();
#endif  