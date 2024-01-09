#include "ngx_func.h"
#include <string.h>
#include <unistd.h>
#include "ngx_global.h"

//设置可执行程序标题相关函数
//将环境变量移出,分配内存，并且把环境变量拷贝到新内存中来
void ngx_init_setproctitle()
{
    //统计环境变量所占的内存。注意判断方法是environ[i]是否为空作为环境变量结束标记
    for(int i = 0;environ[i];++i)
    {
        g_environlen += strlen(environ[i]) + 1;
    }
    gp_envmem = new char[g_environlen];
    memset(gp_envmem,0,g_environlen);

    char* ptmp = gp_envmem;
    //把原来的内存内容搬到新地方来
    for(int i = 0;environ[i];++i)
    {
        size_t len = strlen(environ[i]) + 1;//strlen是不包括字符串末尾的\0的
        strcpy(ptmp,environ[i]);//把原环境变量内容拷贝到新地方【新内存】
        environ[i] = ptmp;//然后还要让环境变量指针指向这段新内存
        ptmp += len;
    }
    return;
}
//修改进程名称
void ngx_setproctitle(const char *title)
{
    //我们假设，所有的命令 行参数我们都不需要用到了，可以被随意覆盖了；
    //注意：我们的标题长度，不会长到原始标题和原始环境变量都装不下，否则怕出问题，不处理
    
    //(1)计算新标题长度
    size_t titlelen = strlen(title);
    //(2)计算总的原始的argv那块内存的总长度【包括各种参数】
    int g_os_argv_len = 0;
    for(int i = 0;g_os_argv[i];++i)
    {
        g_os_argv_len += strlen(g_os_argv[i])+1;
    }
    int esy = g_os_argv_len + g_environlen; //argv和environ内存总和
    if(esy <= titlelen)
    {
        //注意字符串末尾多了个 \0，所以这块判断是 <=【也就是=都算存不下】
        return;
    }
    //(3)设置后续的命令行参数为空，表示只有argv[]中只有一个元素了，这是好习惯；防止后续argv被滥用，因为很多判断是用argv[] == NULL来做结束标记判断的;
    g_os_argv[1] = NULL;
    //(4)把标题弄进来，注意原来的命令行参数都会被覆盖掉，不要再使用这些命令行参数,而且g_os_argv[1]已经被设置为NULL了
    char* ptmp = g_os_argv[0];
    strcpy(ptmp,title);
    ptmp += titlelen;
    memset(ptmp,0,esy - titlelen);
    return;
}