#include "EEPROM.h"
#include "sysTick.h"

#define TIMOUT_EEPROM           1000            // 等待超时
#define WAIT_TRY_TIM            5               // 等待尝试次数

// 构造函数
EEPROM::EEPROM(U8 addr, U8 dataaddrlen, U32 pagesize, U32 totalsize)
{
	m_addr =  addr;
	m_dataAddrLen =  dataaddrlen;
	m_totalSize = totalsize;
	m_pageSize =  pagesize;
}

// 析构函数
EEPROM::~EEPROM()
{

}

// 端口初始化
void EEPROM::InitPort(IICProfile *p)
{
    m_piic = p;
	InitIIC(*p);
}

// 写页
IIC_ERR EEPROM::WritePage(U16 dataaddr, U8 *pdata, U16 datalen)
{
	IIC_ERR err;
	err =  WriteIIC(m_piic, m_addr, dataaddr, m_dataAddrLen, pdata, datalen);       // 写页
    if(err != IIC_ERR_OK)
    {
       return err;
    }
    else
    {
        err = WaiteCurActionFinish();   											//	等待写完成
        if( err != IIC_ERR_OK)
        {
            return err;
        }
    }
	return err;
}

// 等待当前EEPROM动作完成
IIC_ERR EEPROM::WaiteCurActionFinish(void)
{
    IIC_ERR err;
    U32 timout = TIMOUT_EEPROM;
    U32 waitetrytime = 0;
    
    while(I2C_GetFlagStatus(m_piic->I2Cx, I2C_FLAG_BUSY))							  // 总线忙报错退出
	{
		if((timout--) == 0)
		{
			err = IIC_ERR_BUSY;
			return err;
		}
	}
    
    while(1)
    {
																						
        I2C_GenerateSTART(m_piic->I2Cx, ENABLE);                                      // 发送起始位
        err = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);		   		  // 等待发送完成
        if( err != IIC_ERR_OK)												   		  // 超时报错退出
        {
            return err;
        }
        
        I2C_Send7bitAddress(m_piic->I2Cx, m_addr, I2C_Direction_Transmitter);         // 发送器件地址
		err = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);    // 等待地址发送成功
		if( err == IIC_ERR_OK)											
		{
			I2C_GenerateSTOP(m_piic->I2Cx, ENABLE);								      // 器件地址发送成功表明器件空闲
			return IIC_ERR_OK;                                                        // 发送终止位
		}
		if (waitetrytime++ == WAIT_TRY_TIM)                                           // 检查次数
		{
			return IIC_ERR_TRANS_SELECT;
		}
    }
}

// EEPROM 连续写
IIC_ERR EEPROM::Write(U16 dataaddr, U8 *pdata, U16 datalen)
{
	ASSERT(datalen <= m_totalSize);
	ASSERT(pdata != NULL);
	
	IIC_ERR err;
	U16 byteofstartpage = 0;													// 首页所要写的字节数(非完整页) 
	U16 pages = 0;																// 完整页数
	U16 byteoflastpage = 0;														// 最后页所要写的字节数(非完整页)
	
    // 起始地址为页对齐
	if((dataaddr % m_pageSize) == 0)
	{
		// 整夜连续写
		pages =  datalen / m_pageSize;
		for(U32 i = 0; i < pages; i++)
		{
			err = WritePage(dataaddr, pdata, m_pageSize);                       // 任意一页写错报错退出
			if(err != IIC_ERR_OK)
			{
			   return err;
			}
			dataaddr += m_pageSize;
			pdata +=  m_pageSize;
		}
		
		// 非完整末尾页写
		byteoflastpage = datalen % m_pageSize;
		if(byteoflastpage != 0)
		{
			err =  WritePage(dataaddr, pdata, byteoflastpage);
			if(err != IIC_ERR_OK)
			{
				return err;
			}
		}
	}
	// 起始地址未页对齐
	else
	{
		// 起始页写
		byteofstartpage =  m_pageSize - dataaddr % m_pageSize;
		err = WritePage(dataaddr, pdata, byteofstartpage);
		if(err != IIC_ERR_OK)
		{
		   return err;
		}

		// 完整页写
		pages = (datalen - byteofstartpage) / m_pageSize;
		for(U32 i = 0; i < pages; i++)
		{
			err = WritePage(dataaddr, pdata, m_pageSize);                       // 任意一页写错报错退出
			if(err != IIC_ERR_OK)
			{
			   return err;
			}
			dataaddr += m_pageSize;
			pdata +=  m_pageSize;
		}
		
		byteoflastpage = (datalen - byteofstartpage) % m_pageSize;             // 末尾页写
		if(byteoflastpage != 0)
		{
			err =  WritePage(dataaddr, pdata, byteoflastpage);
			if(err != IIC_ERR_OK)
			{
				return err;
			}
		}
	}
	return IIC_ERR_OK;
}

// EEPROM读
IIC_ERR EEPROM::Read(U16 dataaddr, U8 *pdata, U16 datalen)
{
	IIC_ERR err;
	err = ReadIIC(m_piic, m_addr, dataaddr, m_dataAddrLen, pdata, datalen);
	return err;
}
