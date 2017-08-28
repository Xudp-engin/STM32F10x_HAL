/*****************************************************************
  Copyright (c), 2002-2017, Getein Biotechnology Co., Ltd
  Description:  
                STM32��CANģ���װ
  History:         
      <author>          <time>         <desc>
       XDP           	
*****************************************************************/
#define __DEVWRAPPER_IIC_H
#define __DEVWRAPPER_IIC_H

#include "includes.h"
#include "stm32f10x_i2c.h"
#include "DevWrapperIO.h"


#ifdef __cplusplus
extern "C"
{
#endif

// iic�ṹ��
typedef struct
{
	I2C_TypeDef* I2Cx;
	U32 iicspeed;
	BOOL isDMA;
	IOProfile    scl;
	IOProfile    sda;
}IICProfile;

// iic������� (iic������ʱ)
typedef  enum
{   IIC_ERR_OK     = 0,                	// ����
	IIC_ERR_BUSY   = 1,					// ����æ
	IIC_ERR_START  = 2,					// �޷���ʼ
	IIC_ERR_TRANS_SELECT = 3,           // ���ͳ���
	IIC_ERR_RECV_SELECT = 4,			// ���ճ���
	IIC_ERR_WRITE  = 5,                 // д����
	IIC_ERR_READ   = 6,	                // ������
}IIC_ERR;


// ��ʼ��IIC
void InitIIC(IICProfile iic);

// IICд
IIC_ERR WriteIIC(IICProfile *p, U8 deviceaddr, U16 wordaddr, U8 wordaddrlen, U8 *pdata, U16 datalen);

// IIC��
IIC_ERR ReadIIC(IICProfile *p, U8 deviceaddr, U16 wordaddr, U8 wordaddrlen, U8 *pdata, U16 datalen);

// �ȴ���־�¼�
IIC_ERR WaiteFlag(I2C_TypeDef* I2Cx, U32 I2C_EVENT);

#ifdef __cplusplus
};
#endif
