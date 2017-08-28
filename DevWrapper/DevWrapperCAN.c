/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:  
                STM32的CAN模块封装 
  History:         
      <author>      <time>       <desc>
       张小三     07/24/2014     build this module  
*****************************************************************/

#include "DevWrapperCAN.h"

/******************************************************************************
** 函数名  : InitCAN
**
** 描述    : 初始化CAN
**
** 参数    : *P
**
** 返回值  ：无
** 
******************************************************************************/ 
void InitCAN(CANProfile *p) 
{    
    CAN_InitTypeDef       CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef      NVIC_InitStructure;
        
    ASSERT(p != NULL);
    ASSERT((p->BPS == CAN_BPS_100K) || (p->BPS == CAN_BPS_200K) || (p->BPS == CAN_BPS_500K) || (p->BPS == CAN_BPS_1M) );
    ASSERT((p->CANX == CAN1) || (p->CANX == CAN2));
     	
    if(p->CANX == CAN1)
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
    }   
    
    CAN_DeInit(p->CANX);
    CAN_StructInit(&CAN_InitStructure);
    
    InitIO(&p->CAN_RX);
    InitIO(&p->CAN_TX);
    
    CAN_InitStructure.CAN_TTCM = DISABLE;/* 时间触发禁止, 时间触发：CAN硬件的内部定时器被激活，并且被用于产生时间戳 */
    CAN_InitStructure.CAN_ABOM = DISABLE;/* 自动离线禁止，自动离线：一旦硬件监控到128次11个隐性位，就自动退出离线状态。在这里要软件设定后才能退出 */
    CAN_InitStructure.CAN_AWUM = DISABLE;/* 自动唤醒禁止，有报文来的时候自动退出休眠*/
    CAN_InitStructure.CAN_NART = DISABLE;/* 报文重传, 如果错误一直传到成功止，否则只传一次 */
    CAN_InitStructure.CAN_RFLM = ENABLE; /* 接收FIFO锁定, 1--锁定后接收到新的报文摘不要，0--接收到新的报文则覆盖前一报文	*/
    CAN_InitStructure.CAN_TXFP = ENABLE; /* 发送优先级  0---由标识符决定  1---由发送请求顺序决定*/                               // CAN 发送FIFO模式
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;// 正常模式

    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;/* 重新同步跳宽，只有can硬件处于初始化模式时才能访问这个寄存器 */                                
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;/* 时间段1 */
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;/* 时间段2 */
    if(p->BPS == CAN_BPS_100K)
    {
        CAN_InitStructure.CAN_Prescaler = 60;    /* 波特率预分频数 */
    }
    else if( p->BPS == CAN_BPS_200K)
    {
        CAN_InitStructure.CAN_Prescaler = 30;    /* 波特率预分频数 */
    }
    else if( p->BPS == CAN_BPS_500K)
    {
        CAN_InitStructure.CAN_Prescaler = 12;    /* 波特率预分频数 */
    }
    else if( p->BPS == CAN_BPS_1M)
    {
        CAN_InitStructure.CAN_Prescaler = 6;    /* 波特率预分频数 */
    }
    
    CAN_Init(p->CANX, &CAN_InitStructure); 
           
    if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED) 		
    {
    /* 初始化时先设置CAN_MCR的初始化位 */  														
    /* 然后查看硬件是否真的设置了CAN_MSR的初始化位来确认是否进入了初始化模式  */		 													                  
    }

    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 
		
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0;                         //要过滤的ID高位 
    CAN_FilterInitStructure.CAN_FilterIdLow = 0;                          //要过滤的ID低位 
	  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;                     //过滤器高16位想要过滤的位  为1的位表示想要过滤的位
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;                      //过滤器低16位想要过滤的位  
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;  //能够通过该过滤器的报文存到fifo0中 
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;    
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    CAN_ITConfig(p->CANX, CAN_IT_FMP0, ENABLE);
    CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);   
 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);                 

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;      
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;             
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;      
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************
** 函数名  : WriteCan
**
** 描述    : 写数据到CAN
**
** 参数    : CANProfile* p, U8 addr, U8* dat, U8 len
**
** 返回值  ：无
** 
******************************************************************************/ 
void WriteCAN(CANProfile* p, U32 addr, U8* dat, U8 len)
{
    CanTxMsg TxMessage;
    
    ASSERT(p != NULL);

    TxMessage.ExtId = addr;
    TxMessage.RTR = CAN_RTR_DATA;                       // 数据帧
    TxMessage.IDE = CAN_ID_EXT;                         // 标准帧11bit
    TxMessage.DLC = len;    
    memcpy(TxMessage.Data, dat, len);
    CAN_Transmit(p->CANX, &TxMessage);                  // CAN发送
}

/******************************************************************************
** 函数名  : ReadCAN
**
** 描述    : 读CAN接收的数据
**
** 参数    : CANProfile* p, U32* addr, U8* dat, U8 bufLen, U8* retLen
**
** 返回值  ：无
** 
******************************************************************************/ 
void ReadCAN(CANProfile* p, U32* addr, U8* dat, U8 bufLen, U8* retLen)
{
    CanRxMsg RxMessage;
    
    ASSERT(p != NULL);
    ASSERT(bufLen == 8);
    
    CAN_Receive(p->CANX, CAN_FIFO0, &RxMessage);/*接收通信数据*/
    *addr = RxMessage.ExtId;             //实际长度m_addr
    *retLen = RxMessage.DLC;
    memcpy(dat, RxMessage.Data, *retLen);
}

