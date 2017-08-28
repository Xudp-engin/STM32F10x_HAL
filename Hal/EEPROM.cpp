#include "EEPROM.h"
#include "sysTick.h"

#define TIMOUT_EEPROM           1000            // �ȴ���ʱ
#define WAIT_TRY_TIM            5               // �ȴ����Դ���

// ���캯��
EEPROM::EEPROM(U8 addr, U8 dataaddrlen, U32 pagesize, U32 totalsize)
{
	m_addr =  addr;
	m_dataAddrLen =  dataaddrlen;
	m_totalSize = totalsize;
	m_pageSize =  pagesize;
}

// ��������
EEPROM::~EEPROM()
{

}

// �˿ڳ�ʼ��
void EEPROM::InitPort(IICProfile *p)
{
    m_piic = p;
	InitIIC(*p);
}

// дҳ
IIC_ERR EEPROM::WritePage(U16 dataaddr, U8 *pdata, U16 datalen)
{
	IIC_ERR err;
	err =  WriteIIC(m_piic, m_addr, dataaddr, m_dataAddrLen, pdata, datalen);       // дҳ
    if(err != IIC_ERR_OK)
    {
       return err;
    }
    else
    {
        err = WaiteCurActionFinish();   											//	�ȴ�д���
        if( err != IIC_ERR_OK)
        {
            return err;
        }
    }
	return err;
}

// �ȴ���ǰEEPROM�������
IIC_ERR EEPROM::WaiteCurActionFinish(void)
{
    IIC_ERR err;
    U32 timout = TIMOUT_EEPROM;
    U32 waitetrytime = 0;
    
    while(I2C_GetFlagStatus(m_piic->I2Cx, I2C_FLAG_BUSY))							  // ����æ�����˳�
	{
		if((timout--) == 0)
		{
			err = IIC_ERR_BUSY;
			return err;
		}
	}
    
    while(1)
    {
																						
        I2C_GenerateSTART(m_piic->I2Cx, ENABLE);                                      // ������ʼλ
        err = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);		   		  // �ȴ��������
        if( err != IIC_ERR_OK)												   		  // ��ʱ�����˳�
        {
            return err;
        }
        
        I2C_Send7bitAddress(m_piic->I2Cx, m_addr, I2C_Direction_Transmitter);         // ����������ַ
		err = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);    // �ȴ���ַ���ͳɹ�
		if( err == IIC_ERR_OK)											
		{
			I2C_GenerateSTOP(m_piic->I2Cx, ENABLE);								      // ������ַ���ͳɹ�������������
			return IIC_ERR_OK;                                                        // ������ֹλ
		}
		if (waitetrytime++ == WAIT_TRY_TIM)                                           // ������
		{
			return IIC_ERR_TRANS_SELECT;
		}
    }
}

// EEPROM ����д
IIC_ERR EEPROM::Write(U16 dataaddr, U8 *pdata, U16 datalen)
{
	ASSERT(datalen <= m_totalSize);
	ASSERT(pdata != NULL);
	
	IIC_ERR err;
	U16 byteofstartpage = 0;													// ��ҳ��Ҫд���ֽ���(������ҳ) 
	U16 pages = 0;																// ����ҳ��
	U16 byteoflastpage = 0;														// ���ҳ��Ҫд���ֽ���(������ҳ)
	
    // ��ʼ��ַΪҳ����
	if((dataaddr % m_pageSize) == 0)
	{
		// ��ҹ����д
		pages =  datalen / m_pageSize;
		for(U32 i = 0; i < pages; i++)
		{
			err = WritePage(dataaddr, pdata, m_pageSize);                       // ����һҳд�����˳�
			if(err != IIC_ERR_OK)
			{
			   return err;
			}
			dataaddr += m_pageSize;
			pdata +=  m_pageSize;
		}
		
		// ������ĩβҳд
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
	// ��ʼ��ַδҳ����
	else
	{
		// ��ʼҳд
		byteofstartpage =  m_pageSize - dataaddr % m_pageSize;
		err = WritePage(dataaddr, pdata, byteofstartpage);
		if(err != IIC_ERR_OK)
		{
		   return err;
		}

		// ����ҳд
		pages = (datalen - byteofstartpage) / m_pageSize;
		for(U32 i = 0; i < pages; i++)
		{
			err = WritePage(dataaddr, pdata, m_pageSize);                       // ����һҳд�����˳�
			if(err != IIC_ERR_OK)
			{
			   return err;
			}
			dataaddr += m_pageSize;
			pdata +=  m_pageSize;
		}
		
		byteoflastpage = (datalen - byteofstartpage) % m_pageSize;             // ĩβҳд
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

// EEPROM��
IIC_ERR EEPROM::Read(U16 dataaddr, U8 *pdata, U16 datalen)
{
	IIC_ERR err;
	err = ReadIIC(m_piic, m_addr, dataaddr, m_dataAddrLen, pdata, datalen);
	return err;
}
