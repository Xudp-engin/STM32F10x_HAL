#pragma once

#include "DevWrapperIIC.h"

class EEPROM
{
	public:
		 EEPROM(U8 addr, U8 dataaddrlen, U32 pagesize, U32 totalsize);
		 virtual ~EEPROM();
		 virtual void InitPort(IICProfile *p);
		 virtual IIC_ERR Write(U16 dataaddr, U8 *pdata, U16 datalen);
		 virtual IIC_ERR Read(U16 dataaddr, U8 *pdata, U16 datalen);
	private:
		 IIC_ERR WritePage(U16 dataaddr, U8 *pdata, U16 datalen);
		 virtual IIC_ERR WaiteCurActionFinish(void);
	private:
		 U8  m_addr;		
		 U8  m_dataAddrLen;
		 U32 m_totalSize;
		 U32 m_pageSize;
	     IICProfile *m_piic;
};
