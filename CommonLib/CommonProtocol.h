/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // ������������֧��stx len ck��stx etx ck����ģʽ
    
  History:         
      // ��ʷ�޸ļ�¼
      <author>      <time>       <desc>
      ���Ʒ�        09/27/14      v0.5
                                        1) ���ӵ��ִ���Э�黺��������
                                        2) ���ӵõ�������ʣ��������
                                        3) ����U16������ΪU32
      ���Ʒ�        09/16/14      v0.4  
                                        1) ����ETXģʽ��û��STX��CK��
      ���Ʒ�        07/19/14      v0.3 
                                        1) ���Ӳ���CK��STX��ETXģʽ
      ���Ʒ�        07/19/14      v0.2 
                                        1) ����stx etxģʽ����ΪframeLen�ͻ�����ʵ�ʳ���
                                           ��ƥ������½���bug
*****************************************************************/
#pragma once
#include "CommonType.h"
#include "CommonUtil.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_ELEMENT_SIZE_CP     4           
#define MAX_FRAME_SIZE_CP       256
#define CK_OFFSET_BEFORE_ETX    -1
#define CK_OFFSET_AT_END        -3
#define ETX_OFFSET              -2

// �������
#ifndef ASSERT
    #define ASSERT(n) assert(n)
#endif

// ����ָ�붨��
typedef void (*pCheckSum)(U8* buf, U32 bufLen, U8* ret, U8 retLen);
typedef void (*pOnGetFrame)(U8* buf, U32 bufLen, void* lParam);

// ֡����Ԫ������
typedef struct  
{
    U8  data[MAX_ELEMENT_SIZE_CP];
    U8  dataLen;
    S32 offset;
    U8  enabled;
}ProtocolElement;

// ֡����
typedef struct  
{
    ProtocolElement stx;                // ��ʼλ
    ProtocolElement etx;                // ����λ
    ProtocolElement len;                // ���ݳ���
    ProtocolElement ck;                 // У��
    U32             headLen;            // ֡ͷ���ȣ�ֻ��lenʹ��etx����ʱ��Ч
    U8*             buf;                // ������
    U32             bufLen;             // ����������
    U32             datCount;           // ����������Ч�ֽڳ���
    pCheckSum       CheckSum;           // У�麯��
    pOnGetFrame     OnGetFrame;         // ��ȡ֡�ɹ���ص�����
    void*           OnGetFrameParam;
}ProtocolData;

// ��ʼ��֡����
void Init_CP(ProtocolData* p, 
                    U8* buf, 
                    U32 bufLen, 
                    pCheckSum CheckSum, 
                    pOnGetFrame OnGetFrame,
                    void* lParam);

// �趨��ʼλ
void SetSTX_CP(ProtocolData* p, U8* data, U8 dataLen);

// �趨����λ
void SetETX_CP(ProtocolData* p, U8* data, U8 dataLen);

// �趨֡����Ч���ݳ���Ԫ��
void SetLEN_CP(ProtocolData* p, U8 dataLen, S32 offset, U32 headLen);

// �趨У��
void SetCK_CP(ProtocolData* p, U8 dataLen, S32 offset);

// ��֡�����д�����
U8   PutData_CP(ProtocolData* p, U8* dat, U32 datLen);

// ��֡�����д�����
U8   PutDataSingleByte_CP(ProtocolData* p, U8 dat);

// �õ�֡���󻺳���ʣ��ռ�
U32  GetFreeBufferSpace_CP(ProtocolData* p);

// ֡����
void AnalyzeData_CP(ProtocolData* p);

// Ĭ��У�麯��
void CheckSum_CP(U8* buf, U32 bufLen, U8* ret, U8 retLen);

#ifdef __cplusplus
};
#endif

