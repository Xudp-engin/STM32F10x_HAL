/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // 数据流解析，支持stx len ck和stx etx ck两种模式
    
  History:         
      // 历史修改记录
      <author>      <time>       <desc>
      宰云峰        12/25/14      v0.8  
                                        1) 修正etx模式的bug
      宰云峰        12/09/14      v0.7
                                        1) 去除AnalyzeData_ModeETX_CP函数中ASSERT警告
      宰云峰        10/27/14      v0.6  
                                        1) 修正单etx模式协议解析
                                        2）去除S32与U32直接比较警告
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
#include "CommonProtocol.h"

#ifdef WIN32
    #include <memory.h>
    #include <string.h>
    #include <assert.h>
#else
    #include <assert.h>
    #include <string.h>
#endif

// 校验
static U8 VerifyData_CP(ProtocolData* p, U32 bufPos, U32 bufLen, U32 ckOffset, U8 ckDataLen);
// stx len ck模式解析数据
static void AnalyzeData_ModeLEN_CP(ProtocolData* p);
// stx etx ck模式解析数据
static void AnalyzeData_ModeETX_CP(ProtocolData* p);


/*****************************************************************
  Description:    
                   初始化帧对象
  Input:           
                   buf              缓冲区首地址，帧对象会使用该
                                    缓冲区缓冲数据
                   bufLen           缓冲区长度
                   CheckSum         校验函数指针
                   OnGetFrame       帧解析成功后回调函数指针
                   OnGetFrameParam  帧解析成功后回调函数的参数
  Output:          
                   p                帧对象
  Return:          
                   无
*****************************************************************/
void Init_CP(ProtocolData* p, 
                U8* buf, 
                U32 bufLen, 
                pCheckSum CheckSum, 
                pOnGetFrame OnGetFrame,
                void* OnGetFrameParam)
{
    ProtocolData tmp = { 0 };
    
    ASSERT(p != NULL);
    ASSERT(buf != NULL);
    ASSERT(bufLen > 0);
    ASSERT(OnGetFrame != NULL);

    *p          = tmp;
    p->buf      = buf;
    p->bufLen   = bufLen;
    p->CheckSum = (CheckSum == NULL) ? CheckSum_CP : CheckSum;
    p->OnGetFrame = OnGetFrame;
    p->OnGetFrameParam = OnGetFrameParam;
}

/*****************************************************************
  Description:    
                   设定起始位
  Input:           
                   data             起始位数据
                   dataLen          起始位数据长度
  Output:          
                   p                帧对象
  Return:          
                   无
*****************************************************************/
void SetSTX_CP(ProtocolData* p, U8* data, U8 dataLen)
{
    ASSERT(p != NULL);
    ASSERT(dataLen > 0);
    
    memcpy(p->stx.data, data, dataLen);
    p->stx.dataLen = dataLen;
    p->stx.enabled = 1;
    p->stx.offset = 0;
}

/*****************************************************************
  Description:    
                   设定结束位
  Input:           
                   data             结束位数据
                   dataLen          结束位数据长度
  Output:          
                   p                帧对象
  Return:          
                   无
*****************************************************************/
void SetETX_CP(ProtocolData* p, U8* data, U8 dataLen)
{
    ASSERT(p != NULL);
    ASSERT(dataLen > 0);

    memcpy(p->etx.data, data, dataLen);
    p->etx.dataLen = dataLen;
    p->etx.enabled = 1;
    p->etx.offset = ETX_OFFSET;
    p->headLen = 0;
    p->len.enabled = 0;
    p->ck.enabled = 0;
}

/*****************************************************************
  Description:    
                   设定帧内有效数据长度元素
  Input:           
                   dataLen          帧内有效数据长度元素的数据长度
                   offset           帧内有效数据长度元素的位置
                   headLen          帧头长度
  Output:          
                   p                帧对象
  Return:          
                   无
*****************************************************************/
void SetLEN_CP(ProtocolData* p, U8 dataLen, S32 offset, U32 headLen)
{
    ASSERT(p != NULL);
    //ASSERT(dataLen >= 0);
    ASSERT(offset >= 0);

    p->len.dataLen = dataLen;
    p->len.offset = offset;
    p->len.enabled = 1;
    p->headLen = headLen;
    p->etx.enabled = 0;
    p->ck.enabled = 0;
}

/*****************************************************************
  Description:    
                   设定校验
  Input:           
                   dataLen          校验元素的数据长度
                   offset           校验元素的位置
  Output:          
                   p                帧对象
  Return:          
                   无
*****************************************************************/
void SetCK_CP(ProtocolData* p, U8 dataLen, S32 offset)
{
    ASSERT(p != NULL);
    ASSERT(dataLen > 0);

    p->ck.dataLen = dataLen;
    p->ck.offset = offset;
    p->ck.enabled = 1;
}

/*****************************************************************
  Description:    
                   往帧对象中存数据
  Input:           
                   dat          待存数据
                   datLen       待存数据长度
  Output:          
                   p            帧对象
  Return:          
                   1            存入成功
                   0            存入失败
*****************************************************************/
U8 PutData_CP(ProtocolData* p, U8* dat, U32 datLen)
{
    ASSERT(p != NULL);
    ASSERT(p->buf != NULL);

    if (p->datCount + datLen > p->bufLen)
    {
        return 0;
    }

    memcpy(&(p->buf[p->datCount]), dat, datLen);
    p->datCount += datLen;

    return 1;
}

/*****************************************************************
  Description:    
                   往帧对象中存数据
  Input:           
                   dat          待存数据
  Output:          
                   p            帧对象
  Return:          
                   1            存入成功
                   0            存入失败
*****************************************************************/
U8 PutDataSingleByte_CP(ProtocolData* p, U8 dat)
{
    ASSERT(p != NULL);
    ASSERT(p->buf != NULL);

    if (p->datCount + 1 > p->bufLen)
    {
        return 0;
    }

    p->buf[p->datCount] = dat;
    p->datCount++;

    return 1;
}

/*****************************************************************
  Description:    
                   得到帧对象缓冲区剩余空间
  Input:           
                   p            帧对象
  Output:          
                   无
  Return:          
                   缓冲区剩余空间
*****************************************************************/
U32 GetFreeBufferSpace_CP(ProtocolData* p)
{
    ASSERT(p != NULL);
    ASSERT(p->buf != NULL);
    
    if (p->datCount >= p->bufLen)
    {
        return 0;
    }
    else
    {
        return p->bufLen - p->datCount;
    }
}

/*****************************************************************
  Description:    
                   帧解析，解析成功后调用帧解析回调函数
  Input:           
                   无
  Output:          
                   p            帧对象
  Return:          
                   无
*****************************************************************/
void AnalyzeData_CP(ProtocolData* p)
{
    ASSERT(p != NULL);
    ASSERT(p->buf != NULL);
    ASSERT(p->bufLen > 0);
    ASSERT(p->datCount <= p->bufLen);
    ASSERT(XOR(p->len.enabled, p->etx.enabled));
    ASSERT(p->CheckSum != NULL);
    ASSERT(p->OnGetFrame != NULL);
    ASSERT(MAX_ELEMENT_SIZE_CP <= sizeof(U32));

    if (p->len.enabled)
    {
        AnalyzeData_ModeLEN_CP(p);
    }
    else
    {
        AnalyzeData_ModeETX_CP(p);
    }
}

/*****************************************************************
  Description:    
                   默认校验函数
  Input:           
                   buf      待校验缓冲区
                   bufLen   待校验缓冲区长度
                   ret      校验结果长度
  Output:          
                   ret      校验结果
  Return:          
                   无
*****************************************************************/
void CheckSum_CP(U8* buf, U32 bufLen, U8* ret, U8 retLen)
{
    U32 sum = 0; 
    U8 *p = (U8*)buf;
    U32 i = 0;
    U8 datLen = retLen > sizeof(U32) ? sizeof(U32) : retLen;

    for(i = 0; i < bufLen; i++) 
    {
        sum += *p++; 
    }
    sum = (sum >> 16) + (sum & 0xffff); 
    sum += (sum >> 16);
    sum = ~sum;

    ASSERT(ret != NULL);
    ASSERT(retLen > 0 && retLen <= MAX_ELEMENT_SIZE_CP);
    memcpy(ret, &sum, datLen);
}

/*****************************************************************
  Description:    
                   stx len ck模式解析数据
  Input:           
                   无
  Output:          
                   p                帧对象
  Return:          
                   无
*****************************************************************/
static void AnalyzeData_ModeLEN_CP(ProtocolData* p)
{
    U32 i = 0;
    U32 j = 0;
    S32 posPrev = -1;
    S32 posDst = -1;

    ASSERT(p->stx.enabled);
    ASSERT(p->stx.dataLen > 0);
    ASSERT(p->stx.dataLen <= MAX_ELEMENT_SIZE_CP);
    ASSERT(p->stx.offset == 0);
    ASSERT(p->len.enabled);
    ASSERT(p->headLen > 0);
    ASSERT(p->len.offset >= 0);
    ASSERT(p->ck.enabled);
    ASSERT(p->ck.dataLen > 0);
    ASSERT(p->ck.dataLen <= MAX_ELEMENT_SIZE_CP);
    ASSERT(p->ck.offset != 0);
    ASSERT(p->ck.offset != p->len.offset);
    ASSERT(p->len.dataLen <= MAX_ELEMENT_SIZE_CP);

    for (i = 0; i < p->datCount; )
    {
        if (i + p->stx.dataLen < p->datCount)
        {
            if (memcmp(&(p->buf[i]), p->stx.data, p->stx.dataLen) == 0)     //found stx
            {
                U32 curDataLen = 0;
                U32 curCkOffset = 0;
                U32 frameLen = 0;

                // stx len ck模式
                if (i + p->headLen <= p->datCount)
                {
                    // 获取数据长度（帧内有长度记录）
                    ASSERT(p->len.offset + p->len.dataLen <= (S32)p->headLen);
                    for (j = 0; j < p->len.dataLen; j++)
                    {
                        curDataLen += p->buf[i + p->len.offset + j] << (8 * j);
                    }

                    // 计算校验位位置
                    if (p->ck.offset >= 0)
                    {
                        curCkOffset = p->ck.offset;
                        frameLen = p->headLen + curDataLen;
                    }
                    else
                    {
                        curCkOffset = p->headLen + curDataLen;
                        frameLen = p->headLen + curDataLen + p->ck.dataLen;
                    }
                }
                else
                {
                    // 数据没收完，结束循环
                    if (posPrev < 0)
                    {
                        posPrev = i;
                    }
                    break;
                }

                // 查看数据是否接收完毕
                if (i + frameLen <= p->datCount)
                {
                    if (VerifyData_CP(p, i, frameLen, curCkOffset, p->ck.dataLen))
                    {
                        //校验成功
                        p->OnGetFrame(&(p->buf[i]), frameLen, p->OnGetFrameParam);
                        i += frameLen;

                        posPrev = -1;
                    }
                    else
                    {
                        //校验失败
                        i++;
                    }
                }
                else
                {
                    // 数据接收未完成
                    if (posPrev < 0)
                    {
                        posPrev = i;
                    }

                    i++;
                }
            }
            else
            {
                // not stx
                i++;
            }
        }
        else
        {
            break;
        }
    }//end for

    if (posPrev < 0)
    {
        posDst = i;
    }
    else
    {
        posDst = posPrev;
    }

    if (posDst > 0 && p->datCount - posDst > 0)
    {
        memmove(&(p->buf[0]), &(p->buf[posDst]), p->datCount - posDst);
    }

    p->datCount = p->datCount - posDst;
}

/*****************************************************************
  Description:    
                   stx etx ck模式解析数据，其中ck可以不初始化（不存在ck)
  Input:           
                   无
  Output:          
                   p                帧对象
  Return:          
                   无
*****************************************************************/
static void AnalyzeData_ModeETX_CP(ProtocolData* p)
{
    U32 i = 0;
    U32 j = 0;
    S32 posPrev = -1;
    S32 posDst = -1;

    //etx模式
    ASSERT(p->etx.enabled);
    ASSERT(p->etx.offset < 0);
    ASSERT(p->etx.offset < 0);
    ASSERT(p->etx.dataLen > 0 && p->etx.dataLen <= MAX_ELEMENT_SIZE_CP);

    if (p->ck.enabled)
    {
        ASSERT(p->ck.dataLen > 0);
        ASSERT(p->ck.dataLen <= MAX_ELEMENT_SIZE_CP);
        ASSERT(p->ck.offset != 0);
    }

    if (p->stx.enabled)
    {
        //stx etx模式
        ASSERT(p->stx.dataLen > 0);
        ASSERT(p->stx.dataLen <= MAX_ELEMENT_SIZE_CP);
        ASSERT(p->stx.offset == 0);
        ASSERT(p->ck.offset != p->etx.offset);
        
        for (i = 0; i < p->datCount; )
        {
            if (i + p->stx.dataLen < p->datCount)
            {
                if (memcmp(&(p->buf[i]), p->stx.data, p->stx.dataLen) == 0)     //found stx
                {
                    //U32 curDataLen = 0;
                    U32 curCkOffset = 0;
                    U32 frameLen = 0;
                    U32 foundFrame = 0;

                    if (posPrev < 0)
                    {
                        posPrev = i;
                    }

                    for (j = i + p->etx.dataLen; j < p->datCount; j++)
                    {
                        if (memcmp(&(p->buf[j]), p->etx.data, p->etx.dataLen) == 0)
                        {
                            if (p->ck.enabled)
                            {
                                if (p->ck.offset < p->etx.offset)
                                {
                                    curCkOffset = j - i + p->etx.dataLen;
                                    frameLen = curCkOffset + p->ck.dataLen;
                                }
                                else
                                {
                                    curCkOffset = j - i - p->ck.dataLen;
                                    frameLen = j - i + p->etx.dataLen;
                                }

                                if (i + frameLen <= p->datCount)
                                {
                                    if (VerifyData_CP(p, i, frameLen, curCkOffset, p->ck.dataLen))
                                    {
                                        //校验成功
                                        p->OnGetFrame(&(p->buf[i]), frameLen, p->OnGetFrameParam);
                                        i += frameLen;

                                        posPrev    = -1;
                                        foundFrame = 1;
                                        break;
                                    }
                                }
                            }
                            else
                            {
                                // 没有ck，直接成功获取帧
                                frameLen = j - i + p->etx.dataLen;
                                p->OnGetFrame(&(p->buf[i]), frameLen, p->OnGetFrameParam);
                                i += frameLen;

                                posPrev    = -1;
                                foundFrame = 1;
                                break;
                            }
                        }
                    }

                    if (!foundFrame)
                    {
                        i++;
                    }
                }
                else
                {
                    // not stx
                    i++;
                }
            }
            else
            {
                break;
            }
        }//end for

        if (posPrev < 0)
        {
            posDst = i;
        }
        else
        {
            posDst = posPrev;
        }

        if (posDst > 0 && p->datCount - posDst > 0)
        {
            memmove(&(p->buf[0]), &(p->buf[posDst]), p->datCount - posDst);
        }

        p->datCount = p->datCount - posDst;
    }
    else
    {
        posPrev = 0;
        for (i = 0; i < p->datCount; )
        {
            if (i + p->etx.dataLen <= p->datCount && memcmp(&(p->buf[i]), p->etx.data, p->etx.dataLen) == 0)
            {
                p->OnGetFrame(&(p->buf[posPrev]), i + p->etx.dataLen - posPrev, p->OnGetFrameParam);
                posPrev = i + p->etx.dataLen;
                i += p->etx.dataLen;
            }
            else
            {
                i++;
            }
        }
        
        if (posPrev > 0)
        {
            ASSERT((S32)p->datCount >= posPrev);
            if ((S32)p->datCount > posPrev)
            {
                memmove(&(p->buf[0]), &(p->buf[posPrev]), p->datCount - posPrev);
            }
            p->datCount = p->datCount - posPrev;
        }
    }
    
}

/*****************************************************************
  Description:    
                   初始化帧对象
  Input:           
                   p                帧对象
                   bufPos           缓冲区起始地址
                   bufLen           缓冲区长度
                   ckOffset         校验位置相对于缓冲区起始位置的偏移量
                   ckDataLen        校验位置所占字节数
  Output:          
                   无
  Return:          
                   无
*****************************************************************/
static U8 VerifyData_CP(ProtocolData* p, U32 bufPos, U32 bufLen, U32 ckOffset, U8 ckDataLen)
{
    // 校验
    U8  org[MAX_ELEMENT_SIZE_CP];
    U8  dst[MAX_ELEMENT_SIZE_CP];

    memcpy(org, &(p->buf[bufPos + ckOffset]), ckDataLen);
    memset(&(p->buf[bufPos + ckOffset]), 0, ckDataLen);
    p->CheckSum(&(p->buf[bufPos]), bufLen, dst, ckDataLen);
    memcpy(&(p->buf[bufPos + ckOffset]), org, ckDataLen);

    if (memcmp(dst, org, ckDataLen) == 0)
    {
        //校验成功
        return 1;
    }
    else
    {
        //校验失败
        return 0;
    }
}
