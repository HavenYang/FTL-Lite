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
#include "table.h"
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
    if (SUCCESS == sim_flash_erase_block(pu, phy_block_addr))
    {
        //update_tables_after_erase(pu, phy_block_addr, SUCCESS);
        
        return SUCCESS;
    }
    else
    {
        //update_tables_after_erase(pu, phy_block_addr, ERROR_FLASH_ERASE);
        
        return ERROR_FLASH_ERASE;
    }
}


/*====================End of this file========================================*/

