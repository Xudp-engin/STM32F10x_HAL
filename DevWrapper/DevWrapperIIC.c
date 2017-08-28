/*****************************************************************
  Copyright (c), 2002-2017, Getein Biotechnology Co., Ltd
  Description:  
                STM32的CAN模块封装
  History:         
      <author>          <time>         <desc>
       XDP
*****************************************************************/
#include "stm32f10x_dma.h"
#include "DevWrapperIIC.h"

#define IIC_TIMEOUT    				5000        // IIC 操作超时
DMA_InitTypeDef   g_iicDMA_InitStructure;	    // IIC DMA参数结构体全局变量

// iic初始化
void InitIIC(IICProfile iic)
{
	I2C_InitTypeDef I2C_InitStructure;
	
	InitIO(&(iic.scl));
	InitIO(&(iic.sda));
	if(iic.I2Cx == I2C1)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);	
	}
	else if(iic.I2Cx == I2C2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	}
	
	// I2C参数初始化
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x0A;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = iic.iicspeed;
	I2C_Init(iic.I2Cx, &I2C_InitStructure);
	
	// 判断iic是否使用DMA
	if(iic.isDMA == TRUE)
	{
		I2C_DMACmd(I2C2, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		if(iic.I2Cx == I2C1)
		{   
			DMA_DeInit(DMA1_Channel6);
			g_iicDMA_InitStructure.DMA_PeripheralBaseAddr = (U32)&(I2C1->DR);
		}
		else if(iic.I2Cx == I2C2)
		{
			DMA_DeInit(DMA1_Channel4);
			g_iicDMA_InitStructure.DMA_PeripheralBaseAddr = (U32)&(I2C2->DR);
		}	
		g_iicDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   					//   在后续使用DMA时再配置
		g_iicDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    					//   在后续使用DMA时再配置
		g_iicDMA_InitStructure.DMA_BufferSize = 0xFFFF;            					//   在后续使用DMA时再配置
		g_iicDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		g_iicDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		g_iicDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
		g_iicDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		g_iicDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		g_iicDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
		g_iicDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	}
	
	I2C_Cmd(iic.I2Cx, ENABLE);
}

// 写IIC
IIC_ERR WriteIIC(IICProfile *p, U8 deviceaddr, U16 wordaddr, U8 wordaddrlen, U8 *pdata, U16 datalen)
{
	IIC_ERR err;
	U32 timeout =  IIC_TIMEOUT;
	U32 dmaFlag = 0;
	DMA_Channel_TypeDef*  dmaChannel = (DMA_Channel_TypeDef *)0;
	
	// iic忙
	while(I2C_GetFlagStatus(p->I2Cx, I2C_FLAG_BUSY))						// 总线忙报错退出
	{
		if((timeout--) == 0)
		{
			err = IIC_ERR_BUSY;
			return err;
		}
	}
	timeout =  IIC_TIMEOUT;
	
	// 发送起始位
	I2C_GenerateSTART(p->I2Cx, ENABLE);
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					// 等待发送完成
	if( err != IIC_ERR_OK)													// 超时报错退出
	{
		return err;
	}

	// 发送器件地址
	I2C_Send7bitAddress(p->I2Cx, deviceaddr, I2C_Direction_Transmitter);     // 等待发送完成
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);    // 超时报错退出
	if( err != IIC_ERR_OK)											
	{
		return err;
	}
	
	// 发送器件内部地址
	if(wordaddrlen == 1)
	{
	     I2C_SendData(p->I2Cx, wordaddr);									// 发送单字节地址
	}
	else if(wordaddrlen == 2)
	{
		I2C_SendData(p->I2Cx, (U8)((wordaddr & 0xFF00) >> 8));				// 发送双字节地址高字节 
		err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
		if( err != IIC_ERR_OK)
		{
			return err;
		}
		I2C_SendData(p->I2Cx, (U8)(wordaddr & 0x00FF));                    // 发送双字节地址低字节
	}
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	if( err != IIC_ERR_OK)											
	{
		return err;
	}
	
	// 多个字节写
	if(p->isDMA == TRUE)
	{
		g_iicDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pdata;
		g_iicDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    
		g_iicDMA_InitStructure.DMA_BufferSize = (uint32_t)datalen;  

		if(p->I2Cx == I2C1)
		{
			dmaChannel = DMA1_Channel6;
			dmaFlag = DMA1_FLAG_TC6;
		}
		else if(p->I2Cx == I2C2)
		{
			dmaChannel = DMA1_Channel4;
			dmaFlag = DMA1_FLAG_TC4;
		}
		DMA_Init(dmaChannel, &g_iicDMA_InitStructure);
		DMA_Cmd(dmaChannel, ENABLE);
        while(DMA_GetFlagStatus(dmaFlag) == RESET)
		{
		    timeout --;
			if(timeout == 0)
			{
				err = IIC_ERR_WRITE;
				return err;
			}
		}
		DMA_ClearFlag(dmaFlag);
		I2C_GenerateSTOP(p->I2Cx, ENABLE);
	    DMA_Cmd(dmaChannel, DISABLE);
	}
	else if(p->isDMA == FALSE)
	{
		while(datalen--)
		{
			I2C_SendData(p->I2Cx, *pdata);
			pdata ++;
			err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
			if( err != IIC_ERR_OK)											
			{
				return err;
			}
		}
		I2C_GenerateSTOP(p->I2Cx, ENABLE);
	}
	return  IIC_ERR_OK;
}


// 读IIC
IIC_ERR ReadIIC(IICProfile *p, U8 deviceaddr, U16 wordaddr, U8 wordaddrlen, U8 *pdata, U16 datalen)
{
	IIC_ERR err;
	U32 timeout =  IIC_TIMEOUT;
	
	// iic忙不忙
	while(I2C_GetFlagStatus(p->I2Cx, I2C_FLAG_BUSY))						// 总线忙报错退出
	{
		if((timeout--) == 0)
		{
			err = IIC_ERR_BUSY;
			return err;
		}
	}

	// 发送起始位
	I2C_GenerateSTART(p->I2Cx, ENABLE);
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					// 等待发送完成
	if( err != IIC_ERR_OK)												// 超时报错退出
	{
		return err;
	}
	
	// 发送器件地址
	I2C_Send7bitAddress(p->I2Cx, deviceaddr, I2C_Direction_Transmitter);     // 等待发送完成
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);    // 超时报错退出
	if( err != IIC_ERR_OK)											
	{
		return err;
	}
	
	// 发送器件内部地址
	if(wordaddrlen == 1)
	{
	     I2C_SendData(p->I2Cx, wordaddr);									// 发送单字节地址
	}
	else if(wordaddrlen == 2)
	{
		I2C_SendData(p->I2Cx, (U8)((wordaddr & 0xFF00) >> 8));				// 发送双字节地址高字节 
		err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
		if( err != IIC_ERR_OK)
		{
			return err;
		}
		I2C_SendData(p->I2Cx, (U8)(wordaddr & 0x00FF));                    // 发送双字节地址低字节
	}
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	if( err != IIC_ERR_OK)											
	{
		return err;
	}
	
	// 发送起始位
	I2C_GenerateSTART(p->I2Cx, ENABLE);
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);					// 等待发送完成
	if( err != IIC_ERR_OK)												    // 超时报错退出
	{
		return err;
	}
	
	I2C_Send7bitAddress(p->I2Cx, deviceaddr, I2C_Direction_Receiver);
	err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);     // 等待发送完成
	if( err != IIC_ERR_OK)												   // 超时报错退出
	{
		return err;
	}
	
	// 逐字节读取
	while(datalen)
	{   
		if(datalen == 1)
		{
			I2C_AcknowledgeConfig(p->I2Cx, DISABLE);
		}
		err = WaiteFlag(p->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED);			// 等待接收数据
		if( err != IIC_ERR_OK)											    // 超时报错退出
		{
			return err;
		}
        *pdata = I2C_ReceiveData(p->I2Cx);
	    pdata ++;
		if(datalen == 1)
		{
			I2C_GenerateSTOP(p->I2Cx, ENABLE);
            timeout =  IIC_TIMEOUT;
            while(p->I2Cx->CR1 & I2C_CR1_STOP)            					// 等待停止位发送完毕
            {
				if((timeout--) == 0)
				{
					return IIC_ERR_READ;
				}
            }
		}
		datalen --;
	}
	I2C_AcknowledgeConfig(p->I2Cx, ENABLE);
	return IIC_ERR_OK;
}

// 等待标志位
IIC_ERR WaiteFlag(I2C_TypeDef* I2Cx, U32 I2C_EVENT)
{
	IIC_ERR err;
	U32 timeout = IIC_TIMEOUT;
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT))
	{
		if((timeout--) == 0) 
		{
			switch(I2C_EVENT)
			{   
				case I2C_EVENT_MASTER_MODE_SELECT:				   // EV5
					 err = IIC_ERR_START;
					 break;
				case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:   // EV6
					 err = IIC_ERR_TRANS_SELECT;
				     break;
				case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:	   // EV6
				     err = IIC_ERR_RECV_SELECT;
				     break;
				case I2C_EVENT_MASTER_BYTE_TRANSMITTED:            // EV8_2
					 err = IIC_ERR_WRITE;
					 break;
				case I2C_EVENT_MASTER_BYTE_RECEIVED:			   // EV7
					 err = IIC_ERR_READ;
				     break;
				default:
					 break;
			}
			return err;	  
		}
	}
	return IIC_ERR_OK;
}
