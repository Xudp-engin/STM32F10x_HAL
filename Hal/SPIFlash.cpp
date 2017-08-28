#include "SPIFlash.h"

#define TIMOUT_EEPROM           1000            // 等待超时

// 指令宏定义
#define W25Q_WRITEENABLE             0x06
#define W25Q_WRITEDISABLE            0x04
#define W25Q_READSTATUSREG1          0x05
#define W25Q_READSTATUSREG2          0x35
#define W25Q_WRITESTATUSREG          0x01
#define W25Q_WRITEENABLEVOLASTREG    0x50
#define W25Q_READDATA		         0x03 
//#define W25Q_FastReadData			 0x0B 
#define W25Q_PAGEPROGRAM	         0x02 
#define W25Q_BLOCKERASE		         0xD8 
#define W25Q_SECTORERASE		     0x20 
#define W25Q_CHIPERASE		         0xC7 
#define W25Q_POWERDOWN		         0xB9 
#define W25Q_RELEASEPOWERDOWN        0xAB 
#define W25Q_DEVICEID		         0xAB 
#define W25Q_MANUFACTDEVICEID        0x90 
#define W25Q_JEDECDEVICEID	         0x9F 
#define W25Q_UNIQUEIDNUNBER          0x4B


// 构造函数
SPIFlash :: SPIFlash(U32 pagesize, U32 totalsize, SPIProfile *pspi, IOProfile *pio)
{
	m_pageSize = pagesize;
	m_totalSize = totalsize;
	m_pspi = pspi;
	m_selectPin = pio;
	InitSPI(m_pspi);
	InitIO(m_selectPin);
}

// 析构函数
SPIFlash :: ~SPIFlash(void)
{

}

// 写存储器单页
STORAGE_ERR SPIFlash::PageWrite(U32 addr, U8* pdata, U32 datalen)
{
 	STORAGE_ERR err;
	SelectDevice();
	WriteControl(WRITE_ENABLE);
    SendCommand(W25Q_PAGEPROGRAM);
	SendAddr(addr);
    WriteSPI(m_pspi, pdata, datalen);  // 后续可换成DMA实现
	WaiteCurActionFinish();
	WriteControl(WRITE_DISABLE);
    DisSelectDevice();
	return err;
}

// 写Flash
STORAGE_ERR SPIFlash :: Write(U32 addr, U8* pdata, U32 datalen)
{
	STORAGE_ERR err;
	
	return err;
}

// 读Flash
STORAGE_ERR SPIFlash :: Read(U32 addr, U8* pdata, U32 datalen)
{
	STORAGE_ERR err;
	
	return err;
}

// 使能存储器
void SPIFlash::SelectDevice(void)
{
   ResetIO((*m_selectPin));
}

// 失能存储器
void SPIFlash::DisSelectDevice(void)
{
    SetIO((*m_selectPin));
}

// 等待当前动作完成
STORAGE_ERR SPIFlash:: WaiteCurActionFinish(void)
{
   	STORAGE_ERR err;
	U32 timeout = TIMOUT_EEPROM;
	U8 sratusReg1=0;
    SelectDevice();
    SendCommand(W25Q_READSTATUSREG1);
    do
    {
		timeout --;
		if(timeout == 0)
		{
		    DisSelectDevice();
			err = STORAGE_ERR_BUSY; 
			break;
		}
        ReadSPI(m_pspi, &sratusReg1, 1);
    }while((sratusReg1 & 0x01) == 1);
	DisSelectDevice();
	err = STORAGE_ERR_OK;
	return err;
}


// 发送指令
void SPIFlash::SendCommand(U8 cmd)
{
	 U8 command = cmd;
     WriteSPI(m_pspi, &command, 1);
}

// 发送地址
void SPIFlash :: SendAddr(U32 addr)
{
    for(U32 i = 1; i < 4; i++)
    {
        WriteSPI(m_pspi,((U8*)(&addr) + 3 - i),1);    
    }
}

// 写控制
void SPIFlash :: WriteControl(U8 status)
{   
	if(status == WRITE_ENABLE)
	{
		SendCommand(W25Q_WRITEENABLE);
	}
	else if(status == WRITE_DISABLE)
	{
		SendCommand(W25Q_WRITEDISABLE);
	}  
}


