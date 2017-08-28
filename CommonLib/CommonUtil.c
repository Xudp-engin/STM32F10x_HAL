/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // 常用宏定义
    
  History:         
      // 历史修改记录
      <author>      <time>       <desc>
      宰云峰        10/04/2014    v1.0
                                    1）修改SetBit和ClrBit宏
      宰云峰        09/18/2014    v0.9
                                    1）增加百分比计算函数
      宰云峰        09/09/2014    v0.8
                                    1）增加IntCeil宏
      宰云峰        09/09/2014    v0.7
                                    1）增加自定义printf函数
      宰云峰        09/05/2014    v0.6
                                    1）增加自定义ASSERT函数
      宰云峰        07/27/2014    v0.5
                                    1）增加ASSERT中打印__FILE__和__LINE__
                                    2）增加位操作
      宰云峰        07/05/2014    v0.4
                                    1）增加UCOS III支持
                                    2）printf重定向
      宰云峰        07/04/2014    v0.3
                                    1）TRACE和ASSERT防止重复定义
      宰云峰        06/26/2014    v0.2     
                                    1）包含必要的头文件
                                    2）定义ASSERT，TRACE
                                    3）去掉不常用的宏
      季小虎        06/25/2014    v0.1
*****************************************************************/
#ifdef WIN32
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include "CommonUtil.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef USE_UCOS_III
    #include "os.h"
    
    static OS_SEM g_pfLock;
#endif
/*****************************************************************
  Description:    
                   断言失败后进入此函数
  Input:           
                   file            原始文件
                   line            出错位置（具体第几行）
  Output:          
                   无
  Return:          
                   无
*****************************************************************/
void AssertFailed_CU(CHAR* file, U32 line)
{
    TRACE(("file: %s, line: %d \r\n", file, line));
    while(1);
}

/*****************************************************************
  Description:    
                   初始化printf
  Input:           
                   无
  Output:          
                   无
  Return:          
                   无
*****************************************************************/
void InitPrintf_CU(void)
{
#ifdef USE_UCOS_III
    OS_ERR err;
    
    OSSemCreate(&g_pfLock, "", 1, &err);
#endif
}

/*****************************************************************
  Description:    
                   自定义格式化字符串输出
  Input:           
                   _Format          格式化字符串
  Output:          
                   从串口1打印格式化后的字符串
  Return:          
                   无
*****************************************************************/
void Printf_CU(const CHAR * _Format, ...)
{
    static CHAR string[512];
    va_list ap;
    
#ifdef USE_UCOS_III
    OS_ERR err;
    
    OSSemPend(&g_pfLock, 0, OS_OPT_PEND_BLOCKING, NULL, &err);
#endif
    
	va_start(ap, _Format);
	vsnprintf(string, sizeof(string), _Format, ap);
	puts(string);
    va_end(ap);
    
#ifdef USE_UCOS_III
    OSSemPost(&g_pfLock, OS_OPT_POST_1, &err);
#endif
}

/*****************************************************************
  Description:    
                   计算百分比
  Input:           
                   n          当前数
                   total      总数  
  Output:          
                   无
  Return:          
                   百分比
*****************************************************************/
U32  CalcPercent(U32 n, U32 total)
{
    return (U32)((n * 1.0f / total) * 100);
}
