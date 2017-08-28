/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // ������������֧��stx len ck��stx etx ck����ģʽ
    
  History:         
      // ��ʷ�޸ļ�¼
      <author>      <time>       <desc>
      ���Ʒ�        12/25/14      v0.8  
                                        1) ����etxģʽ��bug
      ���Ʒ�        12/09/14      v0.7
                                        1) ȥ��AnalyzeData_ModeETX_CP������ASSERT����
      ���Ʒ�        10/27/14      v0.6  
                                        1) ������etxģʽЭ�����
                                        2��ȥ��S32��U32ֱ�ӱȽϾ���
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
#include "CommonProtocol.h"

#ifdef WIN32
    #include <memory.h>
    #include <string.h>
    #include <assert.h>
#else
    #include <assert.h>
    #include <string.h>
#endif

// У��
static U8 VerifyData_CP(ProtocolData* p, U32 bufPos, U32 bufLen, U32 ckOffset, U8 ckDataLen);
// stx len ckģʽ��������
static void AnalyzeData_ModeLEN_CP(ProtocolData* p);
// stx etx ckģʽ��������
static void AnalyzeData_ModeETX_CP(ProtocolData* p);


/*****************************************************************
  Description:    
                   ��ʼ��֡����
  Input:           
                   buf              �������׵�ַ��֡�����ʹ�ø�
                                    ��������������
                   bufLen           ����������
                   CheckSum         У�麯��ָ��
                   OnGetFrame       ֡�����ɹ���ص�����ָ��
                   OnGetFrameParam  ֡�����ɹ���ص������Ĳ���
  Output:          
                   p                ֡����
  Return:          
                   ��
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
                   �趨��ʼλ
  Input:           
                   data             ��ʼλ����
                   dataLen          ��ʼλ���ݳ���
  Output:          
                   p                ֡����
  Return:          
                   ��
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
                   �趨����λ
  Input:           
                   data             ����λ����
                   dataLen          ����λ���ݳ���
  Output:          
                   p                ֡����
  Return:          
                   ��
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
                   �趨֡����Ч���ݳ���Ԫ��
  Input:           
                   dataLen          ֡����Ч���ݳ���Ԫ�ص����ݳ���
                   offset           ֡����Ч���ݳ���Ԫ�ص�λ��
                   headLen          ֡ͷ����
  Output:          
                   p                ֡����
  Return:          
                   ��
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
                   �趨У��
  Input:           
                   dataLen          У��Ԫ�ص����ݳ���
                   offset           У��Ԫ�ص�λ��
  Output:          
                   p                ֡����
  Return:          
                   ��
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
                   ��֡�����д�����
  Input:           
                   dat          ��������
                   datLen       �������ݳ���
  Output:          
                   p            ֡����
  Return:          
                   1            ����ɹ�
                   0            ����ʧ��
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
                   ��֡�����д�����
  Input:           
                   dat          ��������
  Output:          
                   p            ֡����
  Return:          
                   1            ����ɹ�
                   0            ����ʧ��
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
                   �õ�֡���󻺳���ʣ��ռ�
  Input:           
                   p            ֡����
  Output:          
                   ��
  Return:          
                   ������ʣ��ռ�
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
                   ֡�����������ɹ������֡�����ص�����
  Input:           
                   ��
  Output:          
                   p            ֡����
  Return:          
                   ��
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
                   Ĭ��У�麯��
  Input:           
                   buf      ��У�黺����
                   bufLen   ��У�黺��������
                   ret      У��������
  Output:          
                   ret      У����
  Return:          
                   ��
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
                   stx len ckģʽ��������
  Input:           
                   ��
  Output:          
                   p                ֡����
  Return:          
                   ��
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

                // stx len ckģʽ
                if (i + p->headLen <= p->datCount)
                {
                    // ��ȡ���ݳ��ȣ�֡���г��ȼ�¼��
                    ASSERT(p->len.offset + p->len.dataLen <= (S32)p->headLen);
                    for (j = 0; j < p->len.dataLen; j++)
                    {
                        curDataLen += p->buf[i + p->len.offset + j] << (8 * j);
                    }

                    // ����У��λλ��
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
                    // ����û���꣬����ѭ��
                    if (posPrev < 0)
                    {
                        posPrev = i;
                    }
                    break;
                }

                // �鿴�����Ƿ�������
                if (i + frameLen <= p->datCount)
                {
                    if (VerifyData_CP(p, i, frameLen, curCkOffset, p->ck.dataLen))
                    {
                        //У��ɹ�
                        p->OnGetFrame(&(p->buf[i]), frameLen, p->OnGetFrameParam);
                        i += frameLen;

                        posPrev = -1;
                    }
                    else
                    {
                        //У��ʧ��
                        i++;
                    }
                }
                else
                {
                    // ���ݽ���δ���
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
                   stx etx ckģʽ�������ݣ�����ck���Բ���ʼ����������ck)
  Input:           
                   ��
  Output:          
                   p                ֡����
  Return:          
                   ��
*****************************************************************/
static void AnalyzeData_ModeETX_CP(ProtocolData* p)
{
    U32 i = 0;
    U32 j = 0;
    S32 posPrev = -1;
    S32 posDst = -1;

    //etxģʽ
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
        //stx etxģʽ
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
                                        //У��ɹ�
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
                                // û��ck��ֱ�ӳɹ���ȡ֡
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
                   ��ʼ��֡����
  Input:           
                   p                ֡����
                   bufPos           ��������ʼ��ַ
                   bufLen           ����������
                   ckOffset         У��λ������ڻ�������ʼλ�õ�ƫ����
                   ckDataLen        У��λ����ռ�ֽ���
  Output:          
                   ��
  Return:          
                   ��
*****************************************************************/
static U8 VerifyData_CP(ProtocolData* p, U32 bufPos, U32 bufLen, U32 ckOffset, U8 ckDataLen)
{
    // У��
    U8  org[MAX_ELEMENT_SIZE_CP];
    U8  dst[MAX_ELEMENT_SIZE_CP];

    memcpy(org, &(p->buf[bufPos + ckOffset]), ckDataLen);
    memset(&(p->buf[bufPos + ckOffset]), 0, ckDataLen);
    p->CheckSum(&(p->buf[bufPos]), bufLen, dst, ckDataLen);
    memcpy(&(p->buf[bufPos + ckOffset]), org, ckDataLen);

    if (memcmp(dst, org, ckDataLen) == 0)
    {
        //У��ɹ�
        return 1;
    }
    else
    {
        //У��ʧ��
        return 0;
    }
}
