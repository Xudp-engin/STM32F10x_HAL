/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:  
                STM32的CAN模块封装
  History:         
      <author>          <time>         <desc>
       张小三           07/18/2014      build this module
*****************************************************************/
#ifndef __DEVWRAPPER_CAN_H
#define __DEVWRAPPER_CAN_H

#include "includes.h"
#include "DevWrapperIO.h"
#include "stm32f10x_can.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CAN_BPS_100K    100000
#define CAN_BPS_200K    200000
#define CAN_BPS_500K    500000 
#define CAN_BPS_1M      1000000    
    
// CAN的结构
typedef struct
{
    CAN_TypeDef* CANX;
    IOProfile    CAN_TX;
    IOProfile    CAN_RX;
    U32          BPS;
    U32          startAddr;
    U32          endAddr;
}CANProfile;

// CAN初始化函数
extern void InitCAN(CANProfile* p);
// 写发送数据
extern void WriteCAN(CANProfile* p, U32 addr, U8* dat, U8 len);
// 读CAN接收的数据
extern void ReadCAN(CANProfile* p, U32* addr, U8* dat, U8 bufLen, U8* retLen);


#ifdef __cplusplus
};
#endif

#endif

