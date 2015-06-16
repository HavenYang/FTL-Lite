/*
* File Name    : sim_flash.c
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
#include <memory.h>

#include "sim_flash.h"
#include "sim_test.h"

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
static struct sim_flash_pu_data_t *sim_flash_data[PU_NUM];
static U8 *g_sim_flash_data_addr = NULL;
static U32 g_sim_flash_data_size = 0;


/*============================================================================*/
/* main code region: function implement                                       */
/*============================================================================*/
U32 sim_flash_init(void)
{
    U32 pu;
    
    g_sim_flash_data_size = sizeof(struct sim_flash_pu_data_t) * PU_NUM;
    g_sim_flash_data_addr = (U8*)malloc(g_sim_flash_data_size);

    if (NULL == g_sim_flash_data_addr)
    {
        printf("malloc sim flash memory failed!\n");
        return 0;
    }
    else
    {
        printf("sim flash memory size : 0x%x \n", g_sim_flash_data_size);
    }

    memset(g_sim_flash_data_addr, 0, g_sim_flash_data_size);

    for (pu = 0; pu < PU_NUM; pu++)
    {
        sim_flash_data[pu] = (struct sim_flash_pu_data_t*)(g_sim_flash_data_addr +  sizeof(struct sim_flash_pu_data_t) * pu);
    }

    return g_sim_flash_data_size;
}

void sim_flash_exit(void)
{
    if (NULL != g_sim_flash_data_addr)
    {
        free(g_sim_flash_data_addr);
        g_sim_flash_data_addr = NULL;
    }
}


U32 sim_flash_erase_block(U32 pu, U32 block)
{
    return SIM_SUCCESS;
}

U32 sim_flash_write_page(const struct flash_addr_t *phy_addr, const struct flash_req_t *write_req)
{
    return SIM_SUCCESS;
}

U32 sim_flash_write_lpn(const struct flash_addr_t *phy_addr, const struct flash_req_t *write_req)
{
    return SIM_SUCCESS;
}

U32 sim_flash_read_page(const struct flash_addr_t *phy_addr, const struct flash_req_t *read_req)
{
    return SIM_SUCCESS;
}

U32 sim_flash_read_lpn(const struct flash_addr_t *phy_addr, const struct flash_req_t *read_req)
{
    return SIM_SUCCESS;
}


/*====================End of this file========================================*/

