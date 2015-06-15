/*
* File Name    : env_sim.c
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.12
*===============================================================================
* Modify Record:
*=============================================================================*/

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include "basedefine.h"
#include "disk_config.h"
#include "table.h"
#include "ftl.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

/*============================================================================*/
/* extern region: extern global variable & function prototype                 */
/*============================================================================*/
extern U8 *g_device_dram_addr;

/*============================================================================*/
/* global region: declare global variable                                     */
/*============================================================================*/

/*============================================================================*/
/* local region:  declare local variable & local function prototype           */
/*============================================================================*/

/*============================================================================*/
/* main code region: function implement                                       */
/*============================================================================*/

U32 get_pmt_baseaddr(void)
{
    return PMT_BASE_ADDR;
}

U32 get_pbt_baseaddr(void)
{
    return PBT_BASE_ADDR;
}

U32 get_vbt_baseaddr(void)
{
    return VBT_BASE_ADDR;
}

U32 get_rpmt_baseaddr(void)
{
    return RPMT_BASE_ADDR;
}

U32 get_puinfo_baseaddr(void)
{
    return PUINFO_BASE_ADDR;
}



/*====================End of this file========================================*/

