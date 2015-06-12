/*
* File Name    : main.c
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.12
*===============================================================================
* Modify Record:
*=============================================================================*/

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include <stdio.h>
#include <stdlib.h>

#include "basedefine.h"
#include "disk_config.h"
#include "table.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

struct tables
{
    struct pmt_t pmt;
    struct pbt_t pbt;
    struct vbt_t vbt;
    struct rpmt_t rpmt;
    struct pu_info_t puinfo;
};

/*============================================================================*/
/* extern region: extern global variable & function prototype                 */
/*============================================================================*/

/*============================================================================*/
/* global region: declare global variable                                     */
/*============================================================================*/
U8 *g_device_dram_addr = NULL;
U32 g_device_dram_size = 0;


/*============================================================================*/
/* local region:  declare local variable & local function prototype           */
/*============================================================================*/

/*============================================================================*/
/* main code region: function implement                                       */
/*============================================================================*/

U32 sim_dram_init(void)
{
    g_device_dram_size = (sizeof(struct tables)) * MAX_PU_NUM;
    g_device_dram_addr = (U8*)malloc(g_device_dram_size);

    assert_null_pointer(g_device_dram_addr);
    return g_device_dram_size;
}

U32 test_env_init(void)
{
    sim_dram_init();
    return 0;
}

int main(int argc, char* argv)
{
    return 0;
}

/*====================End of this file========================================*/

