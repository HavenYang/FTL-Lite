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
#include <memory.h>
#include "basedefine.h"
#include "disk_config.h"
#include "table.h"
#include "sim_flash.h"
#include "sim_test.h"
#include "ftl.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

void test_env_exit(void);

struct tables
{
    struct pmt_t pmt;
    struct pbt_t pbt;
    struct vbt_t vbt;
    struct rpmt_t rpmt;
    struct pu_info_t puinfo;
};

struct buffers
{
    U8 buffer[BUF_SIZE];
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
U32 g_ulDbgEnable = 1;

void dbg_getch(void)
{
    U32 ulTestLoop = 0;
    printf("Fatal Error, DBG_Getch!!!\n");

    while (g_ulDbgEnable)
    {
        ulTestLoop++;
    }
    g_ulDbgEnable = 1;

    test_env_exit();
}


U32 sim_dram_init(void)
{
    g_device_dram_size = (sizeof(struct tables) + BUF_SIZE*2) * MAX_PU_NUM;
    g_device_dram_addr = (U8*)malloc(g_device_dram_size);

    assert_null_pointer(g_device_dram_addr);

    if (NULL == g_device_dram_addr)
    {
        printf("malloc sim dram memory failed!\n");
        return 0;
    }
    else
    {
        printf("sim dram memory size : 0x%x \n", g_device_dram_size);
    }

    memset(g_device_dram_addr, 0, g_device_dram_size);
    
    return g_device_dram_size;
}

void sim_dram_exit(void)
{
    if (NULL != g_device_dram_addr)
    {
        free(g_device_dram_addr);
        g_device_dram_addr = NULL;
    }
}

U32 test_env_init(void)
{
    if (0 == sim_dram_init())
    {
        return SIM_FAIL;
    }

    if (0 == sim_flash_init())
    {
        sim_dram_exit();
        return SIM_FAIL;
    }

    sim_test_init();

    return SIM_SUCCESS;
}

void test_env_exit(void)
{
    sim_dram_exit();
    sim_flash_exit();
    exit(0);
}

int main(int argc, char* argv)
{
    if (SIM_SUCCESS != test_env_init())
    {
        return 1;
    }
    
    ftl_init();    
    run_test_cases();

    test_env_exit();

    return 0;
}

/*====================End of this file========================================*/

