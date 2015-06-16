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


U32 flash_write(struct flash_addr_t *phy_addr, struct flash_req_t *flash_req)
{
    return SUCCESS;
}

U32 flash_read(struct flash_addr_t *phy_addr, struct flash_req_t *flash_req)
{
    return SUCCESS;
}

U32 flash_erase(U32 pu, U32 phy_block_addr)
{
    return SUCCESS;
}


/*====================End of this file========================================*/

