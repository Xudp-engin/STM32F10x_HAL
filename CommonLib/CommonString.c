/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:
      // �����ַ�������

  History:
      // ��ʷ�޸ļ�¼
      <author>      <time>       <desc>
      ���Ʒ�        08/27/2014    v0.1
      ���Ʒ�        09/18/2014    v0.2
                                    1����������ת��Ϊ�ַ�������
                                    2��ȥ��itoa_CS�еı��뾯��	            
*****************************************************************/
#ifdef WIN32
//ȥ��vc����
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "CommonString.h"
#include <string.h>
#include <ctype.h>

/*****************************************************************
  Description:
                   ����ָ���ַ������ָ��ַ���
  Input:
                   szOrg            ԭʼ�ַ���
                   szDelimit        �ָ������ַ���
                   pNext            λ�ü���������ʼֵ����Ϊ0
                   szOutput         �ָ����ַ�������׵�ַ
                   outputBufferLen  �������������
  Output:
                   szOutput         �ָ����ַ������
  Return:
                   COMMON_STRING_OK     �ɹ�
                   COMMON_STRING_OVER   ����
                   COMMON_STRING_ERROR  ʧ�ܣ�������̫С��
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

    // �ȵõ��ַ������ȣ�ע��˴�ÿ�α���Ч�ʽϵͣ�����ҪЧ����Ҫ�ı��㷨
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
            // �������������򱨴�
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
                   ȥ���ַ�����ߵĿո�
  Input:
                   szOrg            ԭʼ�ַ���
                   szOutput         �ַ�������׵�ַ
                   outputBufferLen  �������������
  Output:
                   szOutput         �ַ������
  Return:
                   COMMON_STRING_OK     �ɹ�
                   COMMON_STRING_ERROR  ʧ�ܣ�������̫С��
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
                   ȥ���ַ����ұߵĿո�
  Input:
                   szOrg            ԭʼ�ַ���
                   szOutput         �ַ�������׵�ַ
                   outputBufferLen  �������������
  Output:
                   szOutput         �ַ������
  Return:
                   COMMON_STRING_OK     �ɹ�
                   COMMON_STRING_ERROR  ʧ�ܣ�������̫С��
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
                   ȥ���ַ������ҵĿո�
  Input:
                   szOrg            ԭʼ�ַ���
                   szOutput         �ַ�������׵�ַ
                   outputBufferLen  �������������
  Output:
                   szOutput         �ַ������
  Return:
                   COMMON_STRING_OK     �ɹ�
                   COMMON_STRING_ERROR  ʧ�ܣ�������̫С��
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
                   ���ַ�����Ϊ��д
  Input:
                   szOrg            ԭʼ�ַ���
                   szOutput         �ַ�������׵�ַ
                   outputBufferLen  �������������
  Output:
                   szOutput         �ַ������
  Return:
                   COMMON_STRING_OK     �ɹ�
                   COMMON_STRING_ERROR  ʧ�ܣ�������̫С��
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
                   ʵ���޷������͵��ַ�����ת��
  Input:
                   nNum             ����ת��������
                   szStr            ���������
                   nStrLen          ����������
                   nRadix           ���ֽ���
  Output:

  Return:
                   ת������ַ�����ָ��
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
