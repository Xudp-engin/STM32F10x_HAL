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
                                    2）去除itoa_CS中的编译警告	            
*****************************************************************/
#ifdef WIN32
//去除vc警告
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "CommonString.h"
#include <string.h>
#include <ctype.h>

/*****************************************************************
  Description:
                   根据指定字符串来分隔字符串
  Input:
                   szOrg            原始字符串
                   szDelimit        分隔依据字符串
                   pNext            位置计数器，初始值必须为0
                   szOutput         分隔后字符串输出首地址
                   outputBufferLen  输出缓冲区长度
  Output:
                   szOutput         分隔后字符串输出
  Return:
                   COMMON_STRING_OK     成功
                   COMMON_STRING_OVER   结束
                   COMMON_STRING_ERROR  失败（缓冲区太小）
*****************************************************************/
U32 SplitString_CS(const CHAR *szOrg, const CHAR *szDelimit, U32 *pNext, CHAR *szOutput, U32 outputBufferLen)
{
    U32 orgLen = 0;
    U32 strokeLen = 0;
    U32 substrLen = 0;
    U32 errCode = COMMON_STRING_OK;

    ASSERT(szOrg != NULL);
    ASSERT(szDelimit != NULL);
    ASSERT(pNext != NULL);
    ASSERT(szOutput != NULL);
    ASSERT(outputBufferLen > 1);

    // 先得到字符串长度，注意此处每次遍历效率较低，若需要效率则要改变算法
    orgLen = strlen(szOrg);
    strokeLen = strlen(szDelimit);

    if (*pNext + strokeLen <= orgLen)
    {
        const CHAR *szFrom = strstr(&szOrg[*pNext], szDelimit);

        if (szFrom == NULL)
        {
            substrLen = strlen(&szOrg[*pNext]);
        }
        else
        {
            substrLen = (U32)(szFrom - &szOrg[*pNext]);
        }
    }
    else if (*pNext < orgLen)
    {
        substrLen = strlen(&szOrg[*pNext]);
    }
    else
    {
        errCode = COMMON_STRING_OVER;
    }

    if (errCode == COMMON_STRING_OK)
    {
        if (substrLen + 1 > outputBufferLen)
        {
            // 缓冲区不够大，则报错
            errCode = COMMON_STRING_ERROR;
            ASSERT(FALSE);
        }
        else
        {
            strncpy(szOutput, &szOrg[*pNext], substrLen);
            szOutput[substrLen] = '\0';
            *pNext += substrLen + strokeLen;
        }
    }

    return errCode;
}

/*****************************************************************
  Description:
                   去除字符串左边的空格
  Input:
                   szOrg            原始字符串
                   szOutput         字符串输出首地址
                   outputBufferLen  输出缓冲区长度
  Output:
                   szOutput         字符串输出
  Return:
                   COMMON_STRING_OK     成功
                   COMMON_STRING_ERROR  失败（缓冲区太小）
*****************************************************************/
U32 TrimLeft_CS(const CHAR *szOrg, CHAR *szOutput, U32 outputBufferLen)
{
    U32 orgLen = 0;
    U32 start = 0;
    U32 subStrLen = 0;

    ASSERT(szOrg != NULL);
    ASSERT(szOutput != NULL);
    ASSERT(outputBufferLen > 1);

    orgLen = strlen(szOrg);
    start = strspn(szOrg, " ");
    subStrLen = orgLen - start + 1;
    if (subStrLen > outputBufferLen)
    {
        return COMMON_STRING_ERROR;
    }
    else
    {
        strcpy(szOutput, &szOrg[start]);

        return COMMON_STRING_OK;
    }
}

/*****************************************************************
  Description:
                   去除字符串右边的空格
  Input:
                   szOrg            原始字符串
                   szOutput         字符串输出首地址
                   outputBufferLen  输出缓冲区长度
  Output:
                   szOutput         字符串输出
  Return:
                   COMMON_STRING_OK     成功
                   COMMON_STRING_ERROR  失败（缓冲区太小）
*****************************************************************/
U32 TrimRight_CS(const CHAR *szOrg, CHAR *szOutput, U32 outputBufferLen)
{
    U32 orgLen = 0;
    U32 subStrLen = 0;
    U32 i = 0;
    U32 errCode = COMMON_STRING_OK;

    ASSERT(szOrg != NULL);
    ASSERT(szOutput != NULL);
    ASSERT(outputBufferLen > 1);

    orgLen = strlen(szOrg);
    if (orgLen > 0)
    {
        for (i = 0; i < orgLen; i++)
        {
            if (szOrg[orgLen - i - 1] != ' ')
            {
                break;
            }
        }

        subStrLen = orgLen - i;
        if (subStrLen + 1 > outputBufferLen)
        {
            errCode = COMMON_STRING_ERROR;
        }
        else
        {
            strncpy(szOutput, szOrg, subStrLen);
            szOutput[subStrLen] = '\0';
        }
    }
    else
    {
        szOutput[0] = '\0';
    }

    return errCode;
}

/*****************************************************************
  Description:
                   去除字符串左右的空格
  Input:
                   szOrg            原始字符串
                   szOutput         字符串输出首地址
                   outputBufferLen  输出缓冲区长度
  Output:
                   szOutput         字符串输出
  Return:
                   COMMON_STRING_OK     成功
                   COMMON_STRING_ERROR  失败（缓冲区太小）
*****************************************************************/
U32 Trim_CS(const CHAR *szOrg, CHAR *szOutput, U32 outputBufferLen)
{
    U32 errCode = COMMON_STRING_OK;

    if (TrimLeft_CS(szOrg, szOutput, outputBufferLen) == COMMON_STRING_OK)
    {
        U32 orgLen = strlen(szOutput);
        U32 i = 0;

        if (orgLen > 0)
        {
            for (i = 0; i < orgLen; i++)
            {
                if (szOutput[orgLen - i - 1] != ' ')
                {
                    break;
                }
            }

            szOutput[orgLen - i] = '\0';
        }
    }
    else
    {
        errCode = COMMON_STRING_ERROR;
    }

    return errCode;
}

/*****************************************************************
  Description:
                   将字符串变为大写
  Input:
                   szOrg            原始字符串
                   szOutput         字符串输出首地址
                   outputBufferLen  输出缓冲区长度
  Output:
                   szOutput         字符串输出
  Return:
                   COMMON_STRING_OK     成功
                   COMMON_STRING_ERROR  失败（缓冲区太小）
*****************************************************************/
U32 MakeUpper_CS(const CHAR *szOrg, CHAR *szOutput, U32 outputBufferLen)
{
    U32 orgLen = 0;
    U32 i = 0;

    ASSERT(szOrg != NULL);
    ASSERT(szOutput != NULL);
    ASSERT(outputBufferLen > 1);

    orgLen = strlen(szOrg);

    if (orgLen + 1 > outputBufferLen)
    {
        return COMMON_STRING_ERROR;
    }
    else
    {
        for (i = 0; i < orgLen + 1; i++)
        {
            szOutput[i] = (CHAR)toupper(szOrg[i]);
        }

        return COMMON_STRING_OK;
    }
}

/*****************************************************************
  Description:
                   实现无符号整型到字符串的转换
  Input:
                   nNum             将被转换的整数
                   szStr            输出缓冲区
                   nStrLen          缓冲区长度
                   nRadix           数字进制
  Output:

  Return:
                   转换后的字符串首指针
*****************************************************************/
CHAR *itoa_CS(U32 nNum, CHAR *szStr, U32 nStrLen, U32 nRadix)
{

    CHAR *ptr = szStr;
    S32 i;
    U32 j, digval;

    ASSERT(szStr != NULL);
    ASSERT(nStrLen >= 2);

    do
    {
        digval = nNum % nRadix;
        nNum /= nRadix;

        if (digval > 9)
        {
            *ptr++ = (CHAR) (digval - 10 + 'a');
        }
        else
        {
            *ptr++ = (CHAR) (digval + '0');
        }
        if(ptr == szStr + nStrLen)
        {
            return NULL;
        }
    }
    while (nNum);

    *ptr    = '\0';
    j = ptr - szStr - 1;

    for (i = 0; i < (ptr - szStr) / 2; i++)
    {
        CHAR temp = szStr[i];
        szStr[i]  = szStr[j];
        szStr[j--] = temp;
    }

    return szStr;
}
