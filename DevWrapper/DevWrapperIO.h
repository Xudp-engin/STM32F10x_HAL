/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:  
                STM32��IOģ���װ   
  History:         
      <author>          <time>         <desc>
       ��С��           07/18/2014      build this module
*****************************************************************/
#ifndef __DEVWRAPPER_IO_H
#define __DEVWRAPPER_IO_H

#include "includes.h"
#include "stm32f10x_gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

//
#define SetIO(n)      { n.GPIOx->BSRR  = n.GPIO_Pin; }
#define ResetIO(n)    { n.GPIOx->BRR = n.GPIO_Pin; }
#define ReadIO(n)     ((n.GPIOx->IDR & n.GPIO_Pin) ? 1 : 0)

// GPIO�Ľṹ
typedef struct
{
    GPIO_TypeDef* GPIOx;
    U16 GPIO_Pin;
    GPIOMode_TypeDef mode;
    U16 isUsed;
}IOProfile;

//IO�ڳ�ʼ������
void InitIO(IOProfile* ioPro);

#ifdef __cplusplus
};
#endif

#endif

