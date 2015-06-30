/*
* File Name    : disk_config.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _DISK_CONFIG_H
#define _DISK_CONFIG_H

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include "basedefine.h"
#include "flash_interface.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

/* Sector size is 512 Bytes */
#define SEC_SIZE_BITS         9     /* 512 Byte */
#define SEC_SIZE              (1<<SEC_SIZE_BITS)
#define SEC_SIZE_MSK          (SEC_SIZE - 1)

#define LPN_SIZE_BITS         12    /* 4 KB */
#define LPN_SIZE              (1 << LPN_SIZE_BITS)
#define LPN_SIZE_MSK          (LPN_SIZE - 1)

#define BUF_SIZE_BITS         15    /* 32 KB */
#define BUF_SIZE              (1 << BUF_SIZE_BITS)      
#define BUF_SIZE_MSK          (BUF_SIZE - 1)

#define SEC_PER_BUF_BITS      (BUF_SIZE_BITS - SEC_SIZE_BITS)
#define SEC_PER_BUF           (1<<SEC_PER_BUF_BITS)
#define SEC_PER_BUF_MSK       (SEC_PER_BUF -1)

#define SEC_PER_LPN_BITS      (LPN_SIZE_BITS - SEC_SIZE_BITS)
#define SEC_PER_LPN           (1<<SEC_PER_LPN_BITS)
#define SEC_PER_LPN_MSK       (SEC_PER_LPN -1)

#define LPN_PER_BUF_BITS      (SEC_PER_BUF_BITS - SEC_PER_LPN_BITS)
#define LPN_PER_BUF           (1 << LPN_PER_BUF_BITS)
#define LPN_PER_BUF_MSK       (LPN_PER_BUF -1)
#define LPN_PER_BUF_BITMAP    ((1<< LPN_PER_BUF) - 1)


#define LPN_SECTOR_BIT        SEC_PER_LPN_BITS
#define LPN_SECTOR_NUM        SEC_PER_LPN
#define LPN_SECTOR_MSK        SEC_PER_LPN_MSK


#define PMT_ITEM_SIZE           (sizeof(U32))
#define PMT_PAGE_SIZE           (BUF_SIZE)
#define LPN_CNT_PER_PMTPAGE     (PMT_PAGE_SIZE / PMT_ITEM_SIZE)

// 32KB * 512 * 1024 = 16GB per PU
#define FTL_RSV_BLOCK           30
#define BB_RSV_BLOCK            10
#define LPN_IN_BLK              (1 << (PG_PER_BLK_BITS + LPN_PER_BUF_BITS))
#define LPN_IN_PU               (LPN_IN_BLK * BLK_PER_PLN)
#define vBLK_PER_PLN            (BLK_PER_PLN + FTL_RSV_BLOCK)
#define pBLK_PER_PLN            (vBLK_PER_PLN + BB_RSV_BLOCK)

#define PU_NUM  2 /* runtime pu number, <= MAX_PU_NUM */
#define MAX_PU_NUM              PU_NUM
#define MAX_LPN_IN_DISK         (PU_NUM * LPN_IN_PU)
#define MAX_LBA_IN_DISK         (MAX_LPN_IN_DISK << SEC_PER_LPN_BITS)

#define PMT_PAGE_IN_PU          (LPN_IN_PU/LPN_CNT_PER_PMTPAGE)
#define PMT_PAGE_CNT            (MAX_LPN_IN_DISK/LPN_CNT_PER_PMTPAGE)

#define MAX_BB_PER_PLN          (BB_RSV_BLOCK + FTL_RSV_BLOCK/2)


/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/


/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/


#endif
/*====================End of this head file===================================*/


