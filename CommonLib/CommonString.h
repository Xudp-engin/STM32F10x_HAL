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

// ����ָ���ַ������ָ��ַ���
U32   SplitString_CS(const CHAR* szOrg, const CHAR* szDelimit, U32* pNext, CHAR* szOutput, U32 outputBufferLen);

// ȥ���ַ�����ߵĿո�
U32   TrimLeft_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// ȥ���ַ����ұߵĿո�
U32   TrimRight_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// ȥ���ַ������ҵĿո�
U32   Trim_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// ���ַ�����Ϊ��д
U32   MakeUpper_CS(const CHAR* szOrg, CHAR* szOutput, U32 outputBufferLen);

// ʵ���޷������͵��ַ�����ת��
CHAR* itoa_CS(U32 nNum, CHAR* szStr, U32 nStrLen, U32 nRadix);

#ifdef __cplusplus
};
#endif
