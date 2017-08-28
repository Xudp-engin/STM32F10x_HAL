/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:  
                STM32的IO模块封装   
  History:         
      <author>      <time>       <desc>
       张小三     07/18/2014     build this module  
*****************************************************************/

#include "DevWrapperIO.h"

/*****************************************************************
  Description:    初始化IO口
  Input:          ioPro  io对象
  Output:         无
  Return:         无 
*****************************************************************/
void InitIO(IOProfile *ioPro)
{
    ASSERT(ioPro != NULL);
    
    if (ioPro->isUsed)                                        //使用到的IO口才初始化
    {
        GPIO_InitTypeDef  GPIO_InitStructure;
        
        if (ioPro->GPIOx == GPIOA)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        }
        else if (ioPro->GPIOx == GPIOB)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        }
        else if (ioPro->GPIOx == GPIOC)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        }
        else if (ioPro->GPIOx == GPIOD)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        }
        else if (ioPro->GPIOx == GPIOE)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
        }
        else if (ioPro->GPIOx == GPIOF)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
        }
        else if (ioPro->GPIOx == GPIOG)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
        }
        else
        {
            ASSERT(0);
        }
        
        GPIO_InitStructure.GPIO_Mode  = ioPro->mode;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin   = ioPro->GPIO_Pin;
        GPIO_Init(ioPro->GPIOx, &GPIO_InitStructure);
    }
}


