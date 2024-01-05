#include "ngx_c_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "ngx_func.h"
#include <cstring>
#include <algorithm>

//静态成员赋值
CConfig* CConfig::m_instance = nullptr;

//构造函数
CConfig::CConfig()
{

}
//析构函数
CConfig::~CConfig()
{
    for(auto pos = m_ConfigItemList.begin();pos != m_ConfigItemList.end();++pos)
    {
        delete (*pos);
    }
    m_ConfigItemList.clear();
}
//加载配置文件函数
bool CConfig::Load(const char* pconfName)
{
    FILE* fp = fopen(pconfName,"r");
    if(fp == NULL) return false;
    //按行读取文件
    char linebuf[501];//每行读取的字符
    while(!feof(fp))//是否到文件结束
    {
        if(fgets(linebuf,500,fp) == NULL)
        {
            continue;
        }
        //空行跳过
        if(linebuf[0] == 0) continue;
        if(*linebuf == ';' || *linebuf == ' ' || *linebuf == '#' || *linebuf == '\t' || *linebuf == '\n')
        {
            continue;
        }
lblprocstring:
        int len = -1;
        if((len = strlen(linebuf)) > 0)
        {
            if(linebuf[len - 1] == 10 || linebuf[len-1] == 13 || linebuf[len-1] == 32)
            {
                linebuf[len-1] = 0;
                goto lblprocstring;
            }
        }
        if(linebuf[0] == 0)
        {
            continue;
        }
        if(*linebuf == '[')
        {
            continue;
        }
        //处理赋值字符串
        char* ptmp = strchr(linebuf,'=');
        if(ptmp != NULL)
        {
            LPCConfItem p_confitem = new CConfItem;
            memset(p_confitem,0,sizeof(p_confitem));
            strncpy(p_confitem->ItemName,linebuf,(int)(ptmp-linebuf));
            strcpy(p_confitem->ItemContent,ptmp+1);
            Rtrim(p_confitem->ItemName);
            Ltrim(p_confitem->ItemName);
            Rtrim(p_confitem->ItemContent);
            Ltrim(p_confitem->ItemContent);
            m_ConfigItemList.push_back(p_confitem);
        }
    }
    fclose(fp);
    return true;
}
const char* CConfig::GetString(const char* p_itemname)
{
    for(auto p = m_ConfigItemList.begin();p != m_ConfigItemList.end();++p)
    {
        std::string s1(p_itemname);
        std::string s2((*p)->ItemName);
        std::transform(s1.begin(),s1.end(),s1.begin(),::toupper);
        std::transform(s2.begin(),s2.end(),s2.begin(),::toupper);
        if(s1 == s2)
        {
            return (*p)->ItemContent;
        }
    }
    return NULL;
}
int CConfig::GetIntDefault(const char* p_itemname,const int def)
{
    for(auto p = m_ConfigItemList.begin();p != m_ConfigItemList.end();++p)
    {
        std::string s1(p_itemname);
        std::string s2((*p)->ItemName);
        std::transform(s1.begin(),s1.end(),s1.begin(),::toupper);
        std::transform(s2.begin(),s2.end(),s2.begin(),::toupper);
        if(s1 == s2)
        {
            return atoi((*p)->ItemContent);
        }
    }
    return def;
}