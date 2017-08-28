#pragma once

#include "DevWrapperSPI.h"

#define WRITE_ENABLE					1
#define WRITE_DISABLE					0

// 存储设备操作错误代码
typedef enum
{
	STORAGE_ERR_OK    = 0,
	STORAGE_ERR_BUSY,
	STORAGE_ERR_READ,
	STORAGE_ERR_WRITE,
}STORAGE_ERR;

// 存储设备基类
class BasicStorage
{
	public:
		BasicStorage(void);
	    virtual ~BasicStorage(void);
		virtual STORAGE_ERR Write(U32 dataaddr, U8* pdata, U32 datalen) = 0;
		virtual STORAGE_ERR Read(U32 dataaddr, U8* pdata, U32 datalen) = 0;
	private:
		virtual void WriteControl(U8 status) = 0;
		virtual STORAGE_ERR WaiteCurActionFinish(void) = 0;
		virtual STORAGE_ERR PageWrite(U32 addr, U8* pdata, U32 datalen) = 0;
	protected:
		U32 m_pageSize;
		U32 m_totalSize;
};

