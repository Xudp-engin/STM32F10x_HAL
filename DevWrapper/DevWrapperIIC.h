/*****************************************************************
  Copyright (c), 2002-2017, Getein Biotechnology Co., Ltd
  Description:  
                STM32的CAN模块封装
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

// iic结构体
typedef struct
{
	I2C_TypeDef* I2Cx;
	U32 iicspeed;
	BOOL isDMA;
	IOProfile    scl;
	IOProfile    sda;
}IICProfile;

// iic错误编码 (iic操作超时)
typedef  enum
{   IIC_ERR_OK     = 0,                	// 正常
	IIC_ERR_BUSY   = 1,					// 总线忙
	IIC_ERR_START  = 2,					// 无法起始
	IIC_ERR_TRANS_SELECT = 3,           // 发送出错
	IIC_ERR_RECV_SELECT = 4,			// 接收出错
	IIC_ERR_WRITE  = 5,                 // 写出错
	IIC_ERR_READ   = 6,	                // 读出错
}IIC_ERR;


// 初始化IIC
void InitIIC(IICProfile iic);

// IIC写
IIC_ERR WriteIIC(IICProfile *p, U8 deviceaddr, U16 wordaddr, U8 wordaddrlen, U8 *pdata, U16 datalen);

// IIC读
IIC_ERR ReadIIC(IICProfile *p, U8 deviceaddr, U16 wordaddr, U8 wordaddrlen, U8 *pdata, U16 datalen);

// 等待标志事件
IIC_ERR WaiteFlag(I2C_TypeDef* I2Cx, U32 I2C_EVENT);

#ifdef __cplusplus
};
#endif
