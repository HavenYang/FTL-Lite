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
#define BUFFER_DRAM_ADDR    (PUINFO_BASE_ADDR + (sizeof(struct pu_info_t) * MAX_PU_NUM))
#define RSVE_DRAM_ADDR      (BUFFER_DRAM_ADDR + (BUF_SIZE * MAX_PU_NUM))


enum{
    SUCCESS            = 0,
    ERROR_FLASH_READ,
    ERROR_FLASH_WRITE,
    ERROR_FLASH_ERASE,
    ERROR_NO_FLASH_PAGE,
    READ_WITHOUT_WRITE,
    ERROR_UNKNOWN,
};

#define lpn_page_align(lpn)     (0 == ((lpn) & LPN_PER_BUF_MSK))
#define lpn_not_page_align(lpn) (0 != ((lpn) & LPN_PER_BUF_MSK))
#define len_lpn_align(len)      (0 == ((len) & LPN_SIZE_MSK))
#define len_not_lpn_align(len)  (0 != ((len) & LPN_SIZE_MSK))
#define len_page_align(len)     (0 == ((len) & BUF_SIZE_MSK))
#define len_not_page_align(len) (0 != ((len) & BUF_SIZE_MSK))


/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/
/* ftl request type */
typedef enum _efrt
{
    FRT_SEQ_WRITE = 0,
    FRT_SEQ_READ,
    FRT_RAN_WRITE,
    FRT_RAN_READ,
    FRT_IVLD,
}EFRT;

/* ftl request */
struct ftl_req_t
{
    EFRT    request_type;
    U32     buffer_addr;
    U32     lpn_count;
    U32     lpn_list[LPN_PER_BUF];
};


/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/
U32 ftl_init(void);

U32 get_pmt_baseaddr(void);
U32 get_pbt_baseaddr(void);
U32 get_vbt_baseaddr(void);
U32 get_rpmt_baseaddr(void);
U32 get_puinfo_baseaddr(void);

U32 get_pu_from_lpn(U32 lpn);
U32 table_update_pmt(U32 lpn, const struct flash_addr_t *new_vir_addr);
U32 addr_valid(const struct flash_addr_t *flash_addr);
U32 addr_invalid(const struct flash_addr_t *flash_addr);

U32 ftl_write(const struct ftl_req_t *write_request);
U32 ftl_read(const struct ftl_req_t *read_request);

U32 hit_unflush_buffer(U32 lpn);
U32 hit_read(U32 lpn, U32 buffer_addr);

#endif
/*====================End of this head file===================================*/

