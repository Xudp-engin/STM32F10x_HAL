#include "SPIFlash.h"

#define TIMOUT_EEPROM           1000            // �ȴ���ʱ

// ָ��궨��
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


// ���캯��
SPIFlash :: SPIFlash(U32 pagesize, U32 totalsize, SPIProfile *pspi, IOProfile *pio)
{
	m_pageSize = pagesize;
	m_totalSize = totalsize;
	m_pspi = pspi;
	m_selectPin = pio;
	InitSPI(m_pspi);
	InitIO(m_selectPin);
}

// ��������
SPIFlash :: ~SPIFlash(void)
{

}

// д�洢����ҳ
STORAGE_ERR SPIFlash::PageWrite(U32 addr, U8* pdata, U32 datalen)
{
 	STORAGE_ERR err;
	SelectDevice();
	WriteControl(WRITE_ENABLE);
    SendCommand(W25Q_PAGEPROGRAM);
	SendAddr(addr);
    WriteSPI(m_pspi, pdata, datalen);  // �����ɻ���DMAʵ��
	WaiteCurActionFinish();
	WriteControl(WRITE_DISABLE);
    DisSelectDevice();
	return err;
}

// дFlash
STORAGE_ERR SPIFlash :: Write(U32 addr, U8* pdata, U32 datalen)
{
	STORAGE_ERR err;
	
	return err;
}

// ��Flash
STORAGE_ERR SPIFlash :: Read(U32 addr, U8* pdata, U32 datalen)
{
	STORAGE_ERR err;
	
	return err;
}

// ʹ�ܴ洢��
void SPIFlash::SelectDevice(void)
{
   ResetIO((*m_selectPin));
}

// ʧ�ܴ洢��
void SPIFlash::DisSelectDevice(void)
{
    SetIO((*m_selectPin));
}

// �ȴ���ǰ�������
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


// ����ָ��
void SPIFlash::SendCommand(U8 cmd)
{
	 U8 command = cmd;
     WriteSPI(m_pspi, &command, 1);
}

// ���͵�ַ
void SPIFlash :: SendAddr(U32 addr)
{
    for(U32 i = 1; i < 4; i++)
    {
        WriteSPI(m_pspi,((U8*)(&addr) + 3 - i),1);    
    }
}

// д����
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


