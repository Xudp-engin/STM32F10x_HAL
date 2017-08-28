/*****************************************************************
  Copyright (c), 2002-2017, Getein Biotechnology Co., Ltd
  Description:   SPI模块封装
  History:         
      <author>      <time>         <desc>
     
*****************************************************************/
#include "DevWrapperSPI.h"

/******************************************************************************
** 函数名  : InitSPI
** 
** 描述    : 初始化SPI
** 
** 参数    : p  SPI对象
** 
** 返回值  :无
** 
******************************************************************************/ 
void InitSPI(SPIProfile *p)
{
    SPI_InitTypeDef sis;
    
	if(p->SPIX == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	}
	if(p->SPIX == SPI2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	}
    
	InitIO(&p->nssPin);
	InitIO(&p->sckPin);
	InitIO(&p->mosiPin);
	InitIO(&p->misoPin);
	
	SPI_Cmd(p->SPIX, DISABLE);
	sis.SPI_Mode = SPI_Mode_Master;
	sis.SPI_CPOL = SPI_CPOL_Low;
	sis.SPI_CPHA = SPI_CPHA_1Edge;
	sis.SPI_NSS  = SPI_NSS_Hard;
    sis.SPI_DataSize  = SPI_DataSize_8b;
	sis.SPI_FirstBit  = SPI_FirstBit_MSB;
    sis.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    sis.SPI_CRCPolynomial = 7;
	sis.SPI_BaudRatePrescaler = p->baudRatePrescaler;

	SPI_Init(p->SPIX, &sis);
	SPI_SSOutputCmd(p->SPIX, ENABLE);
	SPI_Cmd(p->SPIX, ENABLE);
}

/******************************************************************************
** 函数名  : WriteSPI
** 
** 描述    : 写SPI
** 
** 参数    : p      SPI对象
             buf    发送缓存
             len    发送缓存字节数
** 
** 返回值  :无
** 
******************************************************************************/ 
void WriteSPI(SPIProfile *p, U8 *buf, U32 len)
{
    U16 count = 0;
	while(len--)
	{   
        while(SPI_I2S_GetFlagStatus(p->SPIX, SPI_I2S_FLAG_TXE) != SET)
        {
            count++;
            ASSERT(count < 500);
        }
        count = 0;
		p->SPIX->DR = *(buf++);
        while(SPI_I2S_GetFlagStatus(p->SPIX, SPI_I2S_FLAG_RXNE) != SET)
        {
            count++;
            ASSERT(count < 500);
        } 
        count = 0;        
		p->SPIX->DR;
	}
}

/******************************************************************************
** 函数名  : ReadSPI
** 
** 描述    : 读SPI
** 
** 参数    : p      SPI对象
             buf    接收缓存
             len    接收缓存字节数
** 
** 返回值  :无
** 
******************************************************************************/ 
void ReadSPI(SPIProfile *p, U8* buf, U32 len)
{
    U16 count = 0;
	while(len--)
	{
        while(SPI_I2S_GetFlagStatus(p->SPIX, SPI_I2S_FLAG_TXE) != SET)
        {
            count++;
            ASSERT(count < 500);
        }
        count = 0;
		p ->SPIX->DR = 0xFF;
		while(SPI_I2S_GetFlagStatus(p->SPIX, SPI_I2S_FLAG_RXNE) != SET)
        {
            count++;
            ASSERT(count < 500);
        }
        count = 0;
		*(buf++) = p->SPIX->DR;
	}	
}
