#include "includes.h"
#include <stdio.h>
#include <rt_misc.h>
#include "serial.h"

static struct
{
    queue_hdr_t hdr;                             
    U8  items[1000]; 
} sTxQueue;

/*----------------------------------------------------------------------------
  Initialize UART pins, Baud-rate
 *----------------------------------------------------------------------------*/
void init_serial (void) 
{
    USART_InitTypeDef USART_InitStructure; 
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                                   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                                   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
    NVIC_Init(&NVIC_InitStructure);	

    USART_Cmd(USART1, DISABLE);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                 
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                      
    USART_InitStructure.USART_Parity = USART_Parity_No;                         
    USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx ; 

    QUEUE_INIT(sTxQueue);

    USART_Init(USART1, &USART_InitStructure); 
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ClearFlag(USART1,USART_FLAG_TC);
    USART_Cmd(USART1, ENABLE);
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int sendchar (int c) 
{
    if (QUEUE_FULL(sTxQueue))
    {
        //ASSERT(0);
    }
    else
    {
        QUEUE_PUT(sTxQueue, ((U8)c));
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
	
    return (c);
}


/*----------------------------------------------------------------------------
  Read character from Serial Port   (blocking read)
 *----------------------------------------------------------------------------*/
int getkey (void) 
{
    while (!(USART1->SR & 0x0020));
    return (USART1->DR);
}


/*----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval : None
  */
void USART1_IRQHandler(void)
{
    U8 dat;

    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {     
        if (!QUEUE_EMPTY(sTxQueue))
        {
            QUEUE_GET(sTxQueue, dat);				 
            USART_SendData(USART1, dat);		
        }
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}
