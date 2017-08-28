/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:  
                STM32��CANģ���װ 
  History:         
      <author>      <time>       <desc>
       ��С��     07/24/2014     build this module  
*****************************************************************/

#include "DevWrapperCAN.h"

/******************************************************************************
** ������  : InitCAN
**
** ����    : ��ʼ��CAN
**
** ����    : *P
**
** ����ֵ  ����
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
    
    CAN_InitStructure.CAN_TTCM = DISABLE;/* ʱ�䴥����ֹ, ʱ�䴥����CANӲ�����ڲ���ʱ����������ұ����ڲ���ʱ��� */
    CAN_InitStructure.CAN_ABOM = DISABLE;/* �Զ����߽�ֹ���Զ����ߣ�һ��Ӳ����ص�128��11������λ�����Զ��˳�����״̬��������Ҫ����趨������˳� */
    CAN_InitStructure.CAN_AWUM = DISABLE;/* �Զ����ѽ�ֹ���б�������ʱ���Զ��˳�����*/
    CAN_InitStructure.CAN_NART = DISABLE;/* �����ش�, �������һֱ�����ɹ�ֹ������ֻ��һ�� */
    CAN_InitStructure.CAN_RFLM = ENABLE; /* ����FIFO����, 1--��������յ��µı���ժ��Ҫ��0--���յ��µı����򸲸�ǰһ����	*/
    CAN_InitStructure.CAN_TXFP = ENABLE; /* �������ȼ�  0---�ɱ�ʶ������  1---�ɷ�������˳�����*/                               // CAN ����FIFOģʽ
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;// ����ģʽ

    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;/* ����ͬ������ֻ��canӲ�����ڳ�ʼ��ģʽʱ���ܷ�������Ĵ��� */                                
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;/* ʱ���1 */
    CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;/* ʱ���2 */
    if(p->BPS == CAN_BPS_100K)
    {
        CAN_InitStructure.CAN_Prescaler = 60;    /* ������Ԥ��Ƶ�� */
    }
    else if( p->BPS == CAN_BPS_200K)
    {
        CAN_InitStructure.CAN_Prescaler = 30;    /* ������Ԥ��Ƶ�� */
    }
    else if( p->BPS == CAN_BPS_500K)
    {
        CAN_InitStructure.CAN_Prescaler = 12;    /* ������Ԥ��Ƶ�� */
    }
    else if( p->BPS == CAN_BPS_1M)
    {
        CAN_InitStructure.CAN_Prescaler = 6;    /* ������Ԥ��Ƶ�� */
    }
    
    CAN_Init(p->CANX, &CAN_InitStructure); 
           
    if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED) 		
    {
    /* ��ʼ��ʱ������CAN_MCR�ĳ�ʼ��λ */  														
    /* Ȼ��鿴Ӳ���Ƿ����������CAN_MSR�ĳ�ʼ��λ��ȷ���Ƿ�����˳�ʼ��ģʽ  */		 													                  
    }

    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 
		
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0;                         //Ҫ���˵�ID��λ 
    CAN_FilterInitStructure.CAN_FilterIdLow = 0;                          //Ҫ���˵�ID��λ 
	  CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;                     //��������16λ��Ҫ���˵�λ  Ϊ1��λ��ʾ��Ҫ���˵�λ
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;                      //��������16λ��Ҫ���˵�λ  
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;  //�ܹ�ͨ���ù������ı��Ĵ浽fifo0�� 
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
** ������  : WriteCan
**
** ����    : д���ݵ�CAN
**
** ����    : CANProfile* p, U8 addr, U8* dat, U8 len
**
** ����ֵ  ����
** 
******************************************************************************/ 
void WriteCAN(CANProfile* p, U32 addr, U8* dat, U8 len)
{
    CanTxMsg TxMessage;
    
    ASSERT(p != NULL);

    TxMessage.ExtId = addr;
    TxMessage.RTR = CAN_RTR_DATA;                       // ����֡
    TxMessage.IDE = CAN_ID_EXT;                         // ��׼֡11bit
    TxMessage.DLC = len;    
    memcpy(TxMessage.Data, dat, len);
    CAN_Transmit(p->CANX, &TxMessage);                  // CAN����
}

/******************************************************************************
** ������  : ReadCAN
**
** ����    : ��CAN���յ�����
**
** ����    : CANProfile* p, U32* addr, U8* dat, U8 bufLen, U8* retLen
**
** ����ֵ  ����
** 
******************************************************************************/ 
void ReadCAN(CANProfile* p, U32* addr, U8* dat, U8 bufLen, U8* retLen)
{
    CanRxMsg RxMessage;
    
    ASSERT(p != NULL);
    ASSERT(bufLen == 8);
    
    CAN_Receive(p->CANX, CAN_FIFO0, &RxMessage);/*����ͨ������*/
    *addr = RxMessage.ExtId;             //ʵ�ʳ���m_addr
    *retLen = RxMessage.DLC;
    memcpy(dat, RxMessage.Data, *retLen);
}

