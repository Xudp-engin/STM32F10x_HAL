#pragma once

#include "DevWrapperIIC.h"
#include "BasicStorage.h"


class E2PROM : public BasicStorage
{
    public:
		E2PROM(U8 addr, U8 dataaddrlen, U32 pagesize, U32 totalsize);
		virtual ~E2PROM();
		void InitPort(IICProfile *p);
		virtual STORAGE_ERR Write(U32 dataaddr, U8* pdata, U32 datalen);
		virtual STORAGE_ERR Read(U32 fataaddr, U8* pdata, U32 datalen);

	private:
		virtual STORAGE_ERR WaiteCurActionFinish(void);
	    virtual STORAGE_ERR PageWrite(U32 dataaddr, U8* pdata, U32 datalen);
		virtual void WriteControl(U8 status);

	private:
		U8  m_addr;					// E2PROM 器件地址
		U8  m_dataAddrLen;			// E2PROM 内地址宽度
		IICProfile *m_piic;			// E2PROM IIC接口
};
