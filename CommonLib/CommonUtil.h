/*****************************************************************
  Copyright (c), 2002-2014, Getein Biotechnology Co., Ltd
  Description:     
      // 常用宏定义
    
  History:         
      // 历史修改记录
      <author>      <time>       <desc>
      宰云峰        10/04/2014    v1.0
                                    1）修改SetBit和ClrBit宏
      宰云峰        09/18/2014    v0.9
                                    1）增加百分比计算函数
      宰云峰        09/09/2014    v0.8
                                    1）增加IntCeil宏
      宰云峰        09/09/2014    v0.7
                                    1）增加自定义printf函数
      宰云峰        09/05/2014    v0.6
                                    1）增加自定义ASSERT函数
      宰云峰        07/27/2014    v0.5
                                    1）增加ASSERT中打印__FILE__和__LINE__
                                    2）增加位操作
      宰云峰        07/05/2014    v0.4
                                    1）增加UCOS III支持
                                    2）printf重定向
      宰云峰        07/04/2014    v0.3
                                    1）TRACE和ASSERT防止重复定义
      宰云峰        06/26/2014    v0.2     
                                    1）包含必要的头文件
                                    2）定义ASSERT，TRACE
                                    3）去掉不常用的宏
      季小虎        06/25/2014    v0.1
*****************************************************************/
#ifndef    __COMMONUTIL_H
#define    __COMMONUTIL_H

#include "CommonType.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <memory.h>
#endif

#ifndef NDEBUG
    #ifdef USE_UCOS_III
        #include "os.h"
    #endif
            
    #ifndef ASSERT
        #define ASSERT(n)       { if (!(n)) { AssertFailed_CU(__FILE__, __LINE__); };  }
    #endif
    
    #ifndef TRACE
        #define TRACE(n)        { Printf_CU##n; }
    #endif
#else
    #ifndef ASSERT
        #define ASSERT(n)
    #endif
    
    #ifndef TRACE
        #define TRACE(n)
    #endif
#endif

// 得到指定地址上的一个字节或字
#define  Mem_B( x )             ( *( (U8 *) (x) ) )  
#define  Mem_W( x )             ( *( (U16 *) (x) ) ) 

// 求最大值和最小值
#define  Max( x, y )            ( ((x) > (y)) ? (x) : (y) )  
#define  Min( x, y )            ( ((x) < (y)) ? (x) : (y) )

// x除以y后上取整
#define  IntCeil( x, y )        ( (x) % (y) == 0 ? ((x) / (y)) : ((x) / (y) + 1) ) 

// 得到一个field在结构体(struct)中的偏移量
#define  FOffset( type, field ) ( (U32) &(( type *) 0)->field )

// 得到一个结构体中field所占用的字节数
#define  FSize( type, field )   sizeof( ((type *) 0)->field )  

// 得到一个变量的地址（word宽度）
#define  B_Ptr(var)             ((U8 *)  (void *) &(var))  
#define  W_Ptr(var)             ((U16 *) (void *) &(var))  

// 得到一个字的高位和低位字节
#define  Word_L(n)              ((U8) (((U16)(n)) & 255))  
#define  Word_H(n)              ((U8) (((U16)(n)) >> 8))
#define  MakeWord(L, H)         ((U16)(((U8)(((U16)(L)) & 0xff)) | ((U16)((U8)(((U16)(H)) & 0xff))) << 8))

// 返回数组元素的个数
#define  ArraySize(a)           (sizeof (a) / sizeof (a[0]))
    
// 位操作 
#define  IsValidBit(dat, n)	    ((dat & (1 << (n))) ? 1 : 0)
#define  SetBit(dat, n)         (dat |= (1 << (n)))
#define  ClrBit(dat, n)         (dat &= ~(1 << (n)))

// 定义同或和异或操作
#ifndef XOR
    #define XOR(a, b)           ((!(a) && (b)) || ((a) && (!b))) 
#endif

#ifndef XNOR
    #define XNOR(a, b)          ((!(a) && !(b)) || ((a) && (b))) 
#endif

// 函数声明
#ifdef __cplusplus
extern "C"
{
#endif

// 断言失败后进入此函数
void AssertFailed_CU(CHAR* file, U32 line);

// 初始化printf
void InitPrintf_CU(void);
    
// 自定义格式化字符串输出
void Printf_CU(const CHAR * _Format, ...);

// 计算百分比
U32  CalcPercent(U32 n, U32 total);

#ifdef __cplusplus
};
#endif

#endif  
