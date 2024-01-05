#ifndef __NGX_GLOBAL_H__
#define __NGX_GLOBAL_H__
// 通用的宏定义
//配置项的结构体单元
typedef struct
{
    char ItemName[50];//配置项名称
    char ItemContent[500];//配置项内容
}CConfItem,*LPCConfItem;

//外部全局量声明

#endif