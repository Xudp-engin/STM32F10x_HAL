/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // 基本数据类型定义
    
  History:         
      // 历史修改记录
      <author>      <time>       <desc>
      宰云峰        12/09/14      v0.5
                                    1) BOOL类型修改为signed int
      宰云峰        09/28/14      v0.4
                                    1) U32和S32类型修改为unsigned long
      宰云峰        08/26/14      v0.3
                                    1) 增加CHAR类型
      宰云峰        08/19/14      v0.2
                                    1）数据类型增加宏定义判断
      宰云峰        06/24/14      v0.1
                    
*****************************************************************/

#ifndef    __COMMONTYPE_H
#define    __COMMONTYPE_H

//==================================================================================================
//  Basic data type definition
//==================================================================================================
#ifndef CHAR
    typedef char            CHAR;
#endif

#ifndef S8
    typedef signed char     S8;
#endif

#ifndef U8
    typedef unsigned char   U8;
#endif

#ifndef S16
    typedef signed short    S16;
#endif

#ifndef U16
    typedef unsigned short  U16;
#endif

#ifndef S32
    typedef signed long      S32;
#endif

#ifndef U32
    typedef unsigned long    U32;
#endif

#ifndef F32
    typedef float           F32;
#endif

#ifndef F64
    typedef double          F64;
#endif

#ifndef BOOL
    typedef signed int      BOOL;
#endif

#ifndef  TRUE
    #define    TRUE    1
#endif

#ifndef  FALSE
    #define    FALSE   0
#endif

#ifndef NULL
    #ifdef __cplusplus
        #define NULL    0
    #else
        #define NULL    ((void *)0)
    #endif
#endif

#endif  // COMMON TYPE
