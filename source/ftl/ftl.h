/*
* File Name    : ftl.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _FTL_H
#define _FTL_H

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include "basedefine.h"
/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/
#ifdef SIM
#define TABLE_BASE_ADDR     ((U32)g_device_dram_addr)
#else
#define TABLE_BASE_ADDR     specific_device_address
#endif

#define VBT_BASE_ADDR       TABLE_BASE_ADDR
#define PBT_BASE_ADDR       (VBT_BASE_ADDR + (sizeof(struct vbt_t) * MAX_PU_NUM))
#define PMT_BASE_ADDR       (PBT_BASE_ADDR + (sizeof(struct pbt_t) * MAX_PU_NUM))
#define RPMT_BASE_ADDR      (PMT_BASE_ADDR + (sizeof(struct pmt_t) * MAX_PU_NUM))
#define PUINFO_BASE_ADDR    (RPMT_BASE_ADDR + (sizeof(struct rpmt_t) * MAX_PU_NUM))
#define RSVE_DRAM_ADDR      (PUINFO_BASE_ADDR + (sizeof(struct pu_info_t) * MAX_PU_NUM))


/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/
typedef enum _efr
{
    FRT_SEQ_WRITE = 0,
    FRT_SEQ_READ,
    FRT_RAN_WRITE,
    FRT_RAN_READ,
    FRT_IVLD,
}EFRT;

/* ftl request */
struct ftl_req
{
    EFRT    request_type;
    U32     buffer_addr;
    U32     lpn_count;
    U32     lpn_list[LPN_PER_BUF];
};


/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/


#endif
/*====================End of this head file===================================*/

