/*****************************************************************
  Copyright (c), 2002-2017, Getein Biotechnology Co., Ltd
  Description:  
                STM32L1xx的精确延时
  History:         
      <author>      <time>       <desc>
*****************************************************************/
#include "systick.h"

static U32 fac_us = 0;     //!< usDelay Mut
static U32 fac_ms = 0;     //!< msDelay Mut

/******************************************************************************
**函数名 ：SyTtick_Init
**
**描述   ：初始化SysTick时钟
**
**参数   ：无
**
**返回值 ：无
** 
******************************************************************************/  
void InitSysTick(void)
{
    /* Set clock source = core clock */
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; 
    fac_us = 32000000;
    fac_us /= 1000000;
    fac_ms = fac_us * 1000;
}

/******************************************************************************
**函数名 ：Systick_DelayUs
**
**描述   ：微秒级延时函数
**
**参数   ：us 延时时间 单位us
**
**返回值 ：无
** 
******************************************************************************/
void SysTick_DelayUs(U32 us)
{
    U32 temp = 0;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    }
    while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL =0X00;
}

/******************************************************************************
**函数名 ：SysTick_DelayMs
**
**描述   ：毫秒级延时函数
**
**参数   ：ms 延时
**
**返回值 ：无
** 
******************************************************************************/
void SysTick_DelayMs(U32 ms)
{
    U32 temp = 0;
    U32 i;
    SysTick->LOAD = fac_ms;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    for(i = 0; i < ms; i++)
	{
		SysTick->VAL = 0;
		do
		{
			temp = SysTick->CTRL;
		}
		while((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));
	}
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL =0X00;
}
