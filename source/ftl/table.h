/*
* File Name    : table.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _TABLE_H
#define _TABLE_H

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include "flash_interface.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

typedef enum write_type_e
{
    WRITE_TYPE_SEQ = 0,
    WRITE_TYPE_RND,
    WRITE_TYPE_ALL
}EWT;

typedef enum block_status_e
{
    BLOCK_STATUS_RSV = 0,
    BLOCK_STATUS_FREE,
    BLOCK_STATUS_ALLOCATED,
    BLOCK_STATUS_FULL,
    BLOCK_STATUS_GC,
    BLOCK_STATUS_BADBLCOK,
    BLOCK_STATUS_ALL,
}EBS;

typedef enum block_type_e
{
    BLOCK_TYPE_USER_DATA_WRITE = 0,
    BLOCK_TYPE_GC_WRITE,
    BLOCK_TYPE_TABLE,
}EBT;

/* page management table item , lpn -> vir_flash_addr */
struct pmt_item_t
{
    struct flash_addr_t vir_flash_addr;
};

/* pmt page */
struct pmt_page_t
{
    struct pmt_item_t item[LPN_CNT_PER_PMTPAGE];
};

struct pmt_t
{
    struct pmt_page_t page[PMT_PAGE_IN_PU];
};

/* reverse pmt table, vir_flash_addr -> lpn */
struct rpmt_item_t
{
    U32 lpn[LPN_IN_BLK];
};

struct rpmt_t
{
    struct rpmt_item_t block[vBLK_PER_PLN];
};

/* virtual block information table : point to physical block  */
struct vbt_item_t
{
    union
    {
        U32 dw[2];
        struct
        {
            U32 phy_block_addr:16;
            U32 lpn_dirty_count:16;

            U32 reserved; //to be detail
        };
    };
};

struct vbt_t
{
    struct vbt_item_t item[vBLK_PER_PLN];
};


/* physical block information table : point to virtual block  */
struct pbt_item_t
{
    union
    {
        U32 dw[2];
        struct
        {
            U32 virtual_block_addr:16;
            U32 block_erase_count:16;

            U32 reserved;   //to be detail
        };
    };
};

struct pbt_t
{
    struct pbt_item_t item[pBLK_PER_PLN];
};


struct block_info_t
{
    EBS status;
    U32 erase_count;
    U32 vir_block_addr:16;
    U32 free_page_count:16;
};

struct pu_info_t
{
    U32 curr_user_write_block:16;
    U32 curr_gc_write_block:16;
    U32 total_block_count:16;
    U32 rsv_block_count:16;
    U32 free_block_count:16;
    U32 bad_block_count:16;
    U16 bad_block_addr[MAX_BB_PER_PLN];
    struct block_info_t block_info[pBLK_PER_PLN];
};

/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/

/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/
void table_init(void);
void table_llf_bbt(void);
void table_llf_vbt(void);
void table_llf_pbt(void);
void table_llf_pmt(void);
void table_llf_rpmt(void);
void init_pu_info(void);


struct flash_addr_t flash_alloc_page(U32 pu);
void vir_to_phy_addr(const struct flash_addr_t *vir_addr_from, struct flash_addr_t *phy_addr_to);
U32 flash_alloc_block(U32 pu, EBT block_type);
U32 table_update_pmt(U32 lpn, const struct flash_addr_t *new_vir_addr);
U32 table_lookup_pmt(U32 lpn, struct flash_addr_t *dest_vir_addr);
U32 table_update_rpmt(U32 lpn, const struct flash_addr_t *old_addr, const struct flash_addr_t *new_addr);

U32 update_tables_after_erase(U32 pu, U32 phy_block_addr, U32 erase_status);

void table_set_vir_block(U32 pu, U32 phy_block_addr, U32 vir_block_addr);
U32 table_get_vir_block(U32 pu, U32 phy_block_addr);
void table_set_phy_block(U32 pu, U32 vir_block_addr, U32 phy_block_addr);
U32 table_get_phy_block(U32 pu, U32 vir_block_addr);
U32 search_a_valid_block(U32 pu, U32 start_phy_block_addr);


void show_detail_info(void);

#endif
/*====================End of this head file===================================*/

