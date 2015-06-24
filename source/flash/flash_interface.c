/*
* File Name    : flash_interface.c
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.16
*===============================================================================
* Modify Record:
*=============================================================================*/

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include "flash_interface.h"
#include "disk_config.h"
#include "ftl.h"
#include "sim_flash.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

/*============================================================================*/
/* extern region: extern global variable & function prototype                 */
/*============================================================================*/

/*============================================================================*/
/* global region: declare global variable                                     */
/*============================================================================*/

/*============================================================================*/
/* local region:  declare local variable & local function prototype           */
/*============================================================================*/

/*============================================================================*/
/* main code region: function implement                                       */
/*============================================================================*/


U32 flash_write(struct flash_addr_t *phy_addr, struct flash_req_t *write_req)
{
    U32 result;
    
    if (BUF_SIZE == write_req->data_length)
    {
        result = sim_flash_write_page(phy_addr, write_req);
    }
    else
    {
        result = ERROR_FLASH_WRITE;
    }

    return result;
}

U32 flash_read(struct flash_addr_t *phy_addr, struct flash_req_t *read_req)
{
    U32 result;
    
    if (BUF_SIZE == read_req->data_length)
    {
        result = sim_flash_read_page(phy_addr, read_req);
    }
    else if (LPN_SIZE == read_req->data_length)
    {
        result = sim_flash_read_lpn(phy_addr, read_req);
    }
    else
    {
        result = ERROR_FLASH_READ;
    }

    return result;
}

U32 flash_erase(U32 pu, U32 phy_block_addr)
{
    U32 vir_blk = pbt[pu]->item[phy_block_addr].virtual_block_addr;
    
    if (SUCCESS == sim_flash_erase_block(pu, phy_block_addr))
    {
        pbt[pu]->item[phy_block_addr].block_erase_count++;
        vbt[pu]->item[vir_blk]->lpn_dirty_count = 0;
        pu_info[pu]->free_block_count++;
        pu_info[pu]->block_info[phy_block_addr].erase_count++;
    }
    else
    {
        pu_info[pu]->bad_block_count++;
        pu_info[pu]->block_info[phy_block_addr].status = BLOCK_STATUS_BADBLCOK;
    }
}


/*====================End of this file========================================*/

