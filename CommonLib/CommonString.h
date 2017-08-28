/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // 常用字符串处理
    
  History:         
      // 历史修改记录
      <author>      <time>       <desc>
      宰云峰        08/27/2014    v0.1
      宰云峰        09/18/2014    v0.2
                                    1）增加整数转换为字符串函数
*****************************************************************/
#pragma once
#include "ASCII.h"
#include "CommonType.h"
#include "CommonUtil.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define COMMON_STRING_OK      0
#define COMMON_STRING_OVER    1
#define COMMON_STRING_ERROR   2

// 根据指定字符串来分隔字符串
U32   SplitString_CS(const CHAR* szOrg, const CHAR* szDelimit, U32* pNext, CHAR* szOutput, U32 outputBufferLen);

// 去除字符串左边的空格
U32   TrimLeft_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// 去除字符串右边的空格
U32   TrimRight_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// 去除字符串左右的空格
U32   Trim_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// 将字符串变为大写
U32   MakeUpper_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// 实现无符号整型到字符串的转换
CHAR* itoa_CS(U32 nNum, CHAR* szStr, U32 nStrLen, U32 nRadix);

#ifdef __cplusplus
};
#endif
