#include "ngx_func.h"

#include <stdio.h>
#include <string.h>

//字符串处理相关函数
void Rtrim(char* str)
{
    if(str == NULL)
    {
        return;
    }
    size_t len = strlen(str);
    while(len > 0 && str[len-1] == ' ')
    {
        str[--len] = 0;
    }
    return;
}
void Ltrim(char* str)
{
    if(*str != ' ')
    {
        return;
    }
    //找到第一个不为空格的指针位置
    char* p_tmp = str;
    while((*p_tmp) != '\0')
    {
        if(*p_tmp == ' ')
        {
            p_tmp++;
        }else{
            break;
        }
    }
    //走到最后，表示字符串全是空格
    if(*p_tmp == '\0')
    {
        *str = '/0';
        return;
    }
    char* start = str;
    while(*p_tmp != '\0')
    {
        *start = *p_tmp;
        start++;p_tmp++;
    }
    *start = '\0';
    return;
}