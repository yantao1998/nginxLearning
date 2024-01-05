#ifndef __NGX_CONF_H__
#define __NGX_CONF_H__

#include <vector>
#include "ngx_global.h"
//单例模式，用于读取配置文件
class CConfig
{
private:
    CConfig();
public:
    bool Load(const char* pconfName);
    const char* GetString(const char* p_itemname);
    int GetIntDefault(const char* p_itemname,const int def);
    ~CConfig();
    //数据成员
private:
    static CConfig *m_instance;
public:
    std::vector<LPCConfItem> m_ConfigItemList;//存储配置信息列表
    static CConfig* GetInstance()
    {
        if(m_instance == nullptr)
        {
            m_instance = new CConfig();
            static CGarCycle c1;
        }
        return m_instance;
    }
    class CGarCycle
    {
        public:
            ~CGarCycle()
            {
                if(CConfig::m_instance)
                {
                    delete CConfig::m_instance;
                    CConfig::m_instance = nullptr;
                }
            }
    };
};

#endif