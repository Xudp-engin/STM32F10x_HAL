
#ifndef __SYSTICK_H
#define __SYSTYCK_H

#include "CommonIncludes.h"
#include "stdint.h"
#include "stm32f10x.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
// 初始化SysTick时钟
void InitSysTick(void);
    
// 微妙级延时
void SysTick_DelayUs(U32 us);
    
// 毫秒级延时
void SysTick_DelayMs(U32 ms);
	
	
#ifdef __cplusplus
};
#endif

#endif
