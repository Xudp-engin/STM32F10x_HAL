/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // 数据流解析，支持stx len ck和stx etx ck两种模式
    
  History:         
      // 历史修改记录
      <author>      <time>       <desc>
      宰云峰        09/27/14      v0.5
                                        1) 增加单字存入协议缓冲区函数
                                        2) 增加得到缓冲区剩余量函数
                                        3) 所有U16变量换为U32
      宰云峰        09/16/14      v0.4  
                                        1) 增加ETX模式（没有STX和CK）
      宰云峰        07/19/14      v0.3 
                                        1) 增加不带CK的STX，ETX模式
      宰云峰        07/19/14      v0.2 
                                        1) 修正stx etx模式中因为frameLen和缓冲区实际长度
                                           不匹配而导致解析bug
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

// 定义断言
#ifndef ASSERT
    #define ASSERT(n) assert(n)
#endif

// 函数指针定义
typedef void (*pCheckSum)(U8* buf, U32 bufLen, U8* ret, U8 retLen);
typedef void (*pOnGetFrame)(U8* buf, U32 bufLen, void* lParam);

// 帧基本元素类型
typedef struct  
{
    U8  data[MAX_ELEMENT_SIZE_CP];
    U8  dataLen;
    S32 offset;
    U8  enabled;
}ProtocolElement;

// 帧类型
typedef struct  
{
    ProtocolElement stx;                // 起始位
    ProtocolElement etx;                // 结束位
    ProtocolElement len;                // 数据长度
    ProtocolElement ck;                 // 校验
    U32             headLen;            // 帧头长度，只在len使能etx禁能时有效
    U8*             buf;                // 缓冲区
    U32             bufLen;             // 缓冲区长度
    U32             datCount;           // 缓冲区内有效字节长度
    pCheckSum       CheckSum;           // 校验函数
    pOnGetFrame     OnGetFrame;         // 获取帧成功后回调函数
    void*           OnGetFrameParam;
}ProtocolData;

// 初始化帧对象
void Init_CP(ProtocolData* p, 
                    U8* buf, 
                    U32 bufLen, 
                    pCheckSum CheckSum, 
                    pOnGetFrame OnGetFrame,
                    void* lParam);

// 设定起始位
void SetSTX_CP(ProtocolData* p, U8* data, U8 dataLen);

// 设定结束位
void SetETX_CP(ProtocolData* p, U8* data, U8 dataLen);

// 设定帧内有效数据长度元素
void SetLEN_CP(ProtocolData* p, U8 dataLen, S32 offset, U32 headLen);

// 设定校验
void SetCK_CP(ProtocolData* p, U8 dataLen, S32 offset);

// 往帧对象中存数据
U8   PutData_CP(ProtocolData* p, U8* dat, U32 datLen);

// 往帧对象中存数据
U8   PutDataSingleByte_CP(ProtocolData* p, U8 dat);

// 得到帧对象缓冲区剩余空间
U32  GetFreeBufferSpace_CP(ProtocolData* p);

// 帧解析
void AnalyzeData_CP(ProtocolData* p);

// 默认校验函数
void CheckSum_CP(U8* buf, U32 bufLen, U8* ret, U8 retLen);

#ifdef __cplusplus
};
#endif

