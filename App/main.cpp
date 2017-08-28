#include "Includes.h"
#include "DevWrapperIO.h"
#include "sysTick.h"
#include "E2PROM.h"

static char str1[100] = "EEPROM.cpp Test SuccessFDSFDFSFSDFSDFDSFWFSDFSDFDSFSDFSFSDFS!";
static char str2[100] = {0};

// E2PROM iic
static   IICProfile g_iicAT24C02 = 
{
	I2C2,
	300000,
	TRUE,
	{GPIOB, GPIO_Pin_10, GPIO_Mode_AF_OD, TRUE},
	{GPIOB, GPIO_Pin_11, GPIO_Mode_AF_OD, TRUE}
};

// E2PROM
static  E2PROM g_at24c02(0xA0, 1, 8, 256);
// 函数入口
int main(void)
{   
	InitSysTick();
	g_at24c02.InitPort(&g_iicAT24C02);
    /*初始化串口*/
    init_serial();
    TRACE(("it's shit %d \r\n", 20));
    while(1)
    {   
			if(g_at24c02.Write(0, (U8*)str1, 100) == IIC_ERR_OK)
			{
				 if(g_at24c02.Read(0, (U8*)str2, 100) == IIC_ERR_OK)
				 {
					 TRACE(("-->%s", &str2[0]))			 
				 }
			}
    }
}


#ifdef  USE_FULL_ASSERT


/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}

#endif

