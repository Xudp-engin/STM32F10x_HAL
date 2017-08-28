#include "E2PROM.h"

#define TIMOUT_EEPROM           1000            // �ȴ���ʱ
#define WAIT_TRY_TIM            5               // �ȴ����Դ���

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

// �ȴ��洢���ڲ���һ�������
STORAGE_ERR E2PROM::WaiteCurActionFinish(void)
{
	IIC_ERR iicErr;
	STORAGE_ERR err;
    U32 timout = TIMOUT_EEPROM;
    U32 waitetrytime = 0;
    
    while(I2C_GetFlagStatus(m_piic->I2Cx, I2C_FLAG_BUSY))							  // ����æ�����˳�
	{
		if((timout--) == 0)
		{
			err = STORAGE_ERR_BUSY;
			return err;
		}
	}
	
	// ����5��
    while(1)
    {
																						
        I2C_GenerateSTART(m_piic->I2Cx, ENABLE);                                      // ������ʼλ
        iicErr = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_MODE_SELECT);		   		  // �ȴ��������
        if(iicErr != IIC_ERR_OK)												   	  // ��ʱ�����˳�
        {
			err = STORAGE_ERR_BUSY;
            return err;
        }
        
        I2C_Send7bitAddress(m_piic->I2Cx, m_addr, I2C_Direction_Transmitter);         // ����������ַ
		iicErr = WaiteFlag(m_piic->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED); // �ȴ���ַ���ͳɹ�
		if( iicErr == IIC_ERR_OK)											
		{
			I2C_GenerateSTOP(m_piic->I2Cx, ENABLE);								      // ������ַ���ͳɹ�������������
			err = STORAGE_ERR_OK;
			return err;                                                        		  // ������ֹλ
		}
		if (waitetrytime++ == WAIT_TRY_TIM)                                           // ���Դ����ۼ�
		{
			err = STORAGE_ERR_BUSY;                                                   // ���Դ�������5�Σ�����æ
			return err;
		}
    }
}

// E2PROM дҳ
STORAGE_ERR E2PROM::PageWrite(U32 dataaddr, U8* pdata, U32 datalen)
{
	STORAGE_ERR err;
	IIC_ERR iicerr;
	iicerr =  WriteIIC(m_piic, m_addr, dataaddr, m_dataAddrLen, pdata, datalen);       // дҳ
    if(iicerr != IIC_ERR_OK)                                                           // дҳʧ��
    {
		err = STORAGE_ERR_BUSY;
		return err;
	}
    else
    {
        err = WaiteCurActionFinish();   											   // �ȴ�д���
		return err;
    }
}

// д����
void E2PROM::WriteControl(U8 status)
{
	
}

// д�洢��
STORAGE_ERR E2PROM::Write(U32 dataaddr, U8* pdata, U32 datalen)
{
	ASSERT(datalen <= m_totalSize);
	ASSERT(pdata != NULL);
		
	STORAGE_ERR err;
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
			err = PageWrite(dataaddr, pdata, m_pageSize);                       // ����һҳд�����˳�
			if(err != STORAGE_ERR_OK)
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
			err =  PageWrite(dataaddr, pdata, byteoflastpage);
			if(err != STORAGE_ERR_OK)
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
		err = PageWrite(dataaddr, pdata, byteofstartpage);
		if(err != STORAGE_ERR_OK)
		{
		   return err;
		}

		// ����ҳд
		pages = (datalen - byteofstartpage) / m_pageSize;
		for(U32 i = 0; i < pages; i++)
		{
			err = PageWrite(dataaddr, pdata, m_pageSize);                       // ����һҳд�����˳�
			if(err != STORAGE_ERR_OK)
			{
			   return err;
			}
			dataaddr += m_pageSize;
			pdata +=  m_pageSize;
		}
		
		byteoflastpage = (datalen - byteofstartpage) % m_pageSize;             // ĩβҳд
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
