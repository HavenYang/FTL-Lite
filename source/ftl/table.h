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

/* page management table item */
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
    struct vbt_item_t item[BLK_PER_PLN];
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
    struct pbt_item_t item[BLK_PER_PLN];
};


/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/

/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/
void table_llf_bbt(void);

void table_llf_vbt(void);

void table_llf_pbt(void);

void table_llf_pmt(void);


#endif
/*====================End of this head file===================================*/

