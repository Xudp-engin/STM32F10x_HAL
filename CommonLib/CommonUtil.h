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
#ifndef    __COMMONUTIL_H
#define    __COMMONUTIL_H

#include "CommonType.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <memory.h>
#endif

#ifndef NDEBUG
    #ifdef USE_UCOS_III
        #include "os.h"
    #endif
            
    #ifndef ASSERT
        #define ASSERT(n)       { if (!(n)) { AssertFailed_CU(__FILE__, __LINE__); };  }
    #endif
    
    #ifndef TRACE
        #define TRACE(n)        { Printf_CU##n; }
    #endif
#else
    #ifndef ASSERT
        #define ASSERT(n)
    #endif
    
    #ifndef TRACE
        #define TRACE(n)
    #endif
#endif

// �õ�ָ����ַ�ϵ�һ���ֽڻ���
#define  Mem_B( x )             ( *( (U8 *) (x) ) )  
#define  Mem_W( x )             ( *( (U16 *) (x) ) ) 

// �����ֵ����Сֵ
#define  Max( x, y )            ( ((x) > (y)) ? (x) : (y) )  
#define  Min( x, y )            ( ((x) < (y)) ? (x) : (y) )

// x����y����ȡ��
#define  IntCeil( x, y )        ( (x) % (y) == 0 ? ((x) / (y)) : ((x) / (y) + 1) ) 

// �õ�һ��field�ڽṹ��(struct)�е�ƫ����
#define  FOffset( type, field ) ( (U32) &(( type *) 0)->field )

// �õ�һ���ṹ����field��ռ�õ��ֽ���
#define  FSize( type, field )   sizeof( ((type *) 0)->field )  

// �õ�һ�������ĵ�ַ��word��ȣ�
#define  B_Ptr(var)             ((U8 *)  (void *) &(var))  
#define  W_Ptr(var)             ((U16 *) (void *) &(var))  

// �õ�һ���ֵĸ�λ�͵�λ�ֽ�
#define  Word_L(n)              ((U8) (((U16)(n)) & 255))  
#define  Word_H(n)              ((U8) (((U16)(n)) >> 8))
#define  MakeWord(L, H)         ((U16)(((U8)(((U16)(L)) & 0xff)) | ((U16)((U8)(((U16)(H)) & 0xff))) << 8))

// ��������Ԫ�صĸ���
#define  ArraySize(a)           (sizeof (a) / sizeof (a[0]))
    
// λ���� 
#define  IsValidBit(dat, n)	    ((dat & (1 << (n))) ? 1 : 0)
#define  SetBit(dat, n)         (dat |= (1 << (n)))
#define  ClrBit(dat, n)         (dat &= ~(1 << (n)))

// ����ͬ���������
#ifndef XOR
    #define XOR(a, b)           ((!(a) && (b)) || ((a) && (!b))) 
#endif

#ifndef XNOR
    #define XNOR(a, b)          ((!(a) && !(b)) || ((a) && (b))) 
#endif

// ��������
#ifdef __cplusplus
extern "C"
{
#endif

// ����ʧ�ܺ����˺���
void AssertFailed_CU(CHAR* file, U32 line);

// ��ʼ��printf
void InitPrintf_CU(void);
    
// �Զ����ʽ���ַ������
void Printf_CU(const CHAR * _Format, ...);

// ����ٷֱ�
U32  CalcPercent(U32 n, U32 total);

#ifdef __cplusplus
};
#endif

#endif  
