/*****************************************************************
  Copyright (c), 2002-2017, Getein Biotechnology Co., Ltd
  Description:   SPIģ���װ
  History:         
      <author>      <time>         <desc>
     
*****************************************************************/
#ifndef __DEVWRAPPER_SPI_H
#define __DEVWRAPPER_SPI_H

#include "includes.h"
#include "DevWrapperIO.h"
#include "stm32f10x_spi.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
// SPI����    
typedef struct
{
	SPI_TypeDef* SPIX;
	U32          baudRatePrescaler;
	IOProfile    nssPin;
	IOProfile    sckPin;
	IOProfile    mosiPin;
	IOProfile    misoPin;
}SPIProfile;

// ��ʼ��
void InitSPI(SPIProfile *p);

// дSPI
void WriteSPI(SPIProfile *p, U8 *buf, U32 len);

// ��SPI
void ReadSPI(SPIProfile *p, U8* buf, U32 len);

#ifdef __cplusplus
};
#endif
#endif
