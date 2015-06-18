/*
* File Name    : basedefine.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _BASEDEFINE_H
#define _BASEDEFINE_H

#ifdef SIM
#include <stdio.h>
#endif

/*some type definitions*/
typedef unsigned char    U8;
typedef unsigned short   U16;
typedef unsigned int     U32;
typedef int              S32;
typedef short            S16;
typedef char             S8;

/* quaword(64bit unsigned integer) type definition */
typedef struct _QWORD
{
    U32 LowDw;
    U32 HighDw;
}QWORD;

#ifndef BOOL
typedef S32 BOOL;
#endif

#define    GLOBAL    
#define    LOCAL     static

/*long*/
#ifndef    INVALID_8F        
#define    INVALID_8F        0xFFFFFFFF
#endif

/*short*/
#ifndef    INVALID_4F        
#define    INVALID_4F        0xFFFF
#endif

/*char*/
#ifndef    INVALID_2F        
#define    INVALID_2F        0xFF
#endif

#ifndef     MSK_F    
#define     MSK_F        0xf

#define     MSK_1F         0xf
#define     MSK_2F         0xff
#define     MSK_3F         0xfff
#define     MSK_4F         0xffff
#define     MSK_5F         0xfffff
#define     MSK_6F         0xffffff
#define     MSK_7F         0xfffffff
#define     MSK_8F         0xffffffff
#endif


#ifndef NULL
#define NULL 0
#endif

#define TRUE    1
#define FALSE   0

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define BIT(n)              (1<<(n))
#define ByteToDWord(n)      (((n)+3)>>2)
#define BYTE_0(dw)          ((dw) & 0xff)
#define BYTE_1(dw)          (((dw)>>8) & 0xff)
#define BYTE_2(dw)          (((dw)>>16) & 0xff)
#define BYTE_3(dw)          (((dw)>>24) & 0xff)

void dbg_getch(void);


#define fatalerror(str)     do {\
printf("%s, line %d : %s\n",__FILE__,__LINE__, (str)); \
dbg_getch();\
}while(0)


#define assert_null_pointer(p)  if (NULL == (p)) {fatalerror("null pointer");}

#define equal8f(value)   (0xfffffffful == (value))
#define equal4f(value)   (0xffff == (value))
#define equal2f(value)   (0xff == (value))


#endif
/*====================End of this head file===================================*/

