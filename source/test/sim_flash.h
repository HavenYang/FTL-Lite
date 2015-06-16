/*
* File Name    : sim_flash.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.12
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _SIM_FLASH_H
#define _SIM_FLASH_H

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include "flash_interface.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/


struct sim_flash_lpn_data_t
{
    U32 data;
};

struct sim_flash_page_data_t
{
    struct sim_flash_lpn_data_t lpn[LPN_SECTOR_NUM];
};

struct sim_flash_block_data_t
{
    struct sim_flash_page_data_t page[PG_PER_BLK];
};

struct sim_flash_pu_data_t
{
    struct sim_flash_block_data_t block[BLK_PER_PLN];
};


/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/

/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/

U32 sim_flash_init(void);
void sim_flash_exit(void);

U32 sim_flash_erase_block(U32 pu, U32 block);
U32 sim_flash_write_page(const struct flash_addr_t *phy_addr, const struct flash_req_t *write_req);
U32 sim_flash_write_lpn(const struct flash_addr_t *phy_addr, const struct flash_req_t *write_req);
U32 sim_flash_read_page(const struct flash_addr_t *phy_addr, const struct flash_req_t *read_req);
U32 sim_flash_read_lpn(const struct flash_addr_t *phy_addr, const struct flash_req_t *read_req);


#endif
/*====================End of this head file===================================*/

