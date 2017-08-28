
#ifndef __SYSTICK_H
#define __SYSTYCK_H

#include "CommonIncludes.h"
#include "stdint.h"
#include "stm32f10x.h"

#ifdef __cplusplus
extern "C"
{
#endif
    
// ��ʼ��SysTickʱ��
void InitSysTick(void);
    
// ΢���ʱ
void SysTick_DelayUs(U32 us);
    
// ���뼶��ʱ
void SysTick_DelayMs(U32 ms);
	
	
#ifdef __cplusplus
};
#endif

#endif
