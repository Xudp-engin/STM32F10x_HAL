/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // ���ú궨��
    
  History:         
      // ��ʷ�޸ļ�¼
      <author>      <time>       <desc>
      ���Ʒ�        10/04/2014    v1.0
                                    1���޸�SetBit��ClrBit��
      ���Ʒ�        09/18/2014    v0.9
                                    1�����Ӱٷֱȼ��㺯��
      ���Ʒ�        09/09/2014    v0.8
                                    1������IntCeil��
      ���Ʒ�        09/09/2014    v0.7
                                    1�������Զ���printf����
      ���Ʒ�        09/05/2014    v0.6
                                    1�������Զ���ASSERT����
      ���Ʒ�        07/27/2014    v0.5
                                    1������ASSERT�д�ӡ__FILE__��__LINE__
                                    2������λ����
      ���Ʒ�        07/05/2014    v0.4
                                    1������UCOS III֧��
                                    2��printf�ض���
      ���Ʒ�        07/04/2014    v0.3
                                    1��TRACE��ASSERT��ֹ�ظ�����
      ���Ʒ�        06/26/2014    v0.2     
                                    1��������Ҫ��ͷ�ļ�
                                    2������ASSERT��TRACE
                                    3��ȥ�������õĺ�
      ��С��        06/25/2014    v0.1
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
                   ����ʧ�ܺ����˺���
  Input:           
                   file            ԭʼ�ļ�
                   line            ����λ�ã�����ڼ��У�
  Output:          
                   ��
  Return:          
                   ��
*****************************************************************/
void AssertFailed_CU(CHAR* file, U32 line)
{
    TRACE(("file: %s, line: %d \r\n", file, line));
    while(1);
}

/*****************************************************************
  Description:    
                   ��ʼ��printf
  Input:           
                   ��
  Output:          
                   ��
  Return:          
                   ��
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
                   �Զ����ʽ���ַ������
  Input:           
                   _Format          ��ʽ���ַ���
  Output:          
                   �Ӵ���1��ӡ��ʽ������ַ���
  Return:          
                   ��
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
                   ����ٷֱ�
  Input:           
                   n          ��ǰ��
                   total      ����  
  Output:          
                   ��
  Return:          
                   �ٷֱ�
*****************************************************************/
U32  CalcPercent(U32 n, U32 total)
{
    return (U32)((n * 1.0f / total) * 100);
}
