#include "E2PROM.h"

#define TIMOUT_EEPROM           1000            // 等待超时
#define WAIT_TRY_TIM            5               // 等待尝试次数

E2PROM::E2PROM(U8 addr, U8 dataaddrlen, U32 pagesize, U32 totalsize)
{
	m_addr = addr;
	m_dataAddrLen = dataaddrlen;
	m_pageSize = pagesize;
	m_totalSize = totalsize;
}

E2PROM::~E2PROM(void)
{

}

void E2PROM :: InitPort(IICProfile *p)
{
	m_piic = p;
	InitIIC(*p);
}

// 等待存储器内部上一动作完成
STORAGE_ERR E2PROM::WaiteCurActionFinish(void)
{
	IIC_ERR iicErr;
	STORAGE_ERR err;
    U32 timout = TIMOUT_EEPROM;
    U32 waitetrytime = 0;
    
    while(I2C_GetFlagStatus(m_piic->I2Cx, I2C_FLAG_BUSY))							  // 总线忙报错退出
	{
		if((timout--) == 0)
		{
			err = STORAGE_ERR_BUSY;
			return err;
		}
	}
	
	// 尝试5次
    while(1)
    {
																						
        I2C_GenerateSTART(m_piic->I2Cx, ENABLE);                                      // 发送起始位
        iicErr = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);		   		  // 等待发送完成
        if(iicErr != IIC_ERR_OK)												   	  // 超时报错退出
        {
			err = STORAGE_ERR_BUSY;
            return err;
        }
        
        I2C_Send7bitAddress(m_piic->I2Cx, m_addr, I2C_Direction_Transmitter);         // 发送器件地址
		iicErr = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED); // 等待地址发送成功
		if( iicErr == IIC_ERR_OK)											
		{
			I2C_GenerateSTOP(m_piic->I2Cx, ENABLE);								      // 器件地址发送成功表明器件空闲
			err = STORAGE_ERR_OK;
			return err;                                                        		  // 发送终止位
		}
		if (waitetrytime++ == WAIT_TRY_TIM)                                           // 尝试次数累加
		{
			err = STORAGE_ERR_BUSY;                                                   // 尝试次数超过5次，器件忙
			return err;
		}
    }
}

// E2PROM 写页
STORAGE_ERR E2PROM::PageWrite(U32 dataaddr, U8* pdata, U32 datalen)
{
	STORAGE_ERR err;
	IIC_ERR iicerr;
	iicerr =  WriteIIC(m_piic, m_addr, dataaddr, m_dataAddrLen, pdata, datalen);       // 写页
    if(iicerr != IIC_ERR_OK)                                                           // 写页失败
    {
		err = STORAGE_ERR_BUSY;
		return err;
	}
    else
    {
        err = WaiteCurActionFinish();   											   // 等待写完成
		return err;
    }
}

// 写控制
void E2PROM::WriteControl(U8 status)
{
	
}

// 写存储器
STORAGE_ERR E2PROM::Write(U32 dataaddr, U8* pdata, U32 datalen)
{
	ASSERT(datalen <= m_totalSize);
	ASSERT(pdata != NULL);
		
	STORAGE_ERR err;
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
			err = PageWrite(dataaddr, pdata, m_pageSize);                       // 任意一页写错报错退出
			if(err != STORAGE_ERR_OK)
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
			err =  PageWrite(dataaddr, pdata, byteoflastpage);
			if(err != STORAGE_ERR_OK)
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
		err = PageWrite(dataaddr, pdata, byteofstartpage);
		if(err != STORAGE_ERR_OK)
		{
		   return err;
		}

		// 完整页写
		pages = (datalen - byteofstartpage) / m_pageSize;
		for(U32 i = 0; i < pages; i++)
		{
			err = PageWrite(dataaddr, pdata, m_pageSize);                       // 任意一页写错报错退出
			if(err != STORAGE_ERR_OK)
			{
			   return err;
			}
			dataaddr += m_pageSize;
			pdata +=  m_pageSize;
		}
		
		byteoflastpage = (datalen - byteofstartpage) % m_pageSize;             // 末尾页写
		if(byteoflastpage != 0)
		{
			err =  PageWrite(dataaddr, pdata, byteoflastpage);
			if(err != STORAGE_ERR_OK)
			{
				return err;
			}
		}
	}
	return STORAGE_ERR_OK;
}

STORAGE_ERR E2PROM::Read(U32 dataaddr, U8* pdata, U32 datalen)
{
    STORAGE_ERR err;
	IIC_ERR iicerr;
	iicerr = ReadIIC(m_piic, m_addr, dataaddr, m_dataAddrLen, pdata, datalen);
	if(iicerr != IIC_ERR_OK)
	{
		err = STORAGE_ERR_READ;
		return err;
	}
}
