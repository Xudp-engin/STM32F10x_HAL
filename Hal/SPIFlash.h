#pragma once

#include "DevWrapperSPI.h"
#include "BasicStorage.h"

// SPIFlash¿‡
class SPIFlash : public BasicStorage
{
	public:
		SPIFlash(U32 pagesize, U32 totalsize, SPIProfile *pspi, IOProfile *pio);
	    virtual ~SPIFlash(void);
		virtual STORAGE_ERR Write(U32 addr, U8* pdata, U32 datalen);
		virtual STORAGE_ERR Read(U32 addr, U8* pdata, U32 datalen);
	private:
		virtual STORAGE_ERR PageWrite(U32 addr, U8* pdata, U32 datalen);
		virtual STORAGE_ERR WaiteCurActionFinish(void);
		void SelectDevice(void);
		void DisSelectDevice(void);
		void WriteControl(U8 status);
		void SendCommand(U8 cmd);
		void SendAddr(U32 addr);
	private: 
		U32 m_pageSize;
		U32 m_totalSize;
		SPIProfile *m_pspi;
		IOProfile *m_selectPin;
};

