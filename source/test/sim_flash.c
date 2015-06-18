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

void sim_check_flash_addr(const struct flash_addr_t *phy_addr)
{
    assert_null_pointer(phy_addr);
    
    if (phy_addr->pu_index >= PU_NUM)
    {
        fatalerror("pu num overflow");
    }

    if (phy_addr->block_in_pu >= BLK_PER_PLN)
    {
        fatalerror("block num overflow");
    }

    if (phy_addr->page_in_block >= PG_PER_BLK)
    {
        fatalerror("page num overflow");
    }

    if (phy_addr->lpn_in_page >= LPN_PER_BUF)
    {
        fatalerror("lpn in page num overflow");
    }
}

void sim_check_flash_req(const struct flash_req_t *flash_req)
{
    assert_null_pointer(flash_req);
    if (0 == flash_req->data_buffer_addr)
    {
        fatalerror("buffer addr is null");
    }

    if (0 == flash_req->data_length)
    {
        fatalerror("data length zero");
    }

    if (flash_req->data_length > BUF_SIZE)
    {
        fatalerror("data length overflow");
    }

    if (len_not_lpn_align(flash_req->data_length))
    {
        fatalerror("data length not lpn align");
    }
}

static void sim_set_flash_data(struct sim_flash_lpn_data_t* lpn_data, U32 buffer)
{
    lpn_data->lpn = *(U32*)buffer;
    lpn_data->write_count = *(U32*)(buffer + sizeof(U32));
}

static void sim_get_flash_data(const struct sim_flash_lpn_data_t* lpn_data, U32 buffer)
{
    *(U32*)buffer = lpn_data->lpn;
    *(U32*)(buffer + sizeof(U32)) = lpn_data->write_count;
}

U32 sim_flash_erase_block(U32 pu, U32 block)
{
    return SIM_SUCCESS;
}

U32 sim_flash_write_page(const struct flash_addr_t *phy_addr, const struct flash_req_t *write_req)
{
    U32 i;
    U32 buffer;
    struct sim_flash_page_data_t *sim_page_data;
    
    sim_check_flash_addr(phy_addr);
    sim_check_flash_req(write_req);

    if (0 != phy_addr->lpn_in_page)
    {
        fatalerror("flash addr not page align");
    }

    if (BUF_SIZE != write_req->data_length)
    {
        fatalerror("not full page data");
    }

    sim_page_data = &sim_flash_data[phy_addr->pu_index]->block[phy_addr->block_in_pu].page[phy_addr->page_in_block];
    buffer = write_req->data_buffer_addr;

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        sim_set_flash_data(&sim_page_data->lpn[i], buffer + LPN_SIZE * i);
    }
    
    return SIM_SUCCESS;
}

U32 sim_flash_read_page(const struct flash_addr_t *phy_addr, const struct flash_req_t *read_req)
{
    U32 i;
    U32 buffer;
    struct sim_flash_page_data_t *sim_page_data;

    sim_check_flash_addr(phy_addr);
    sim_check_flash_req(read_req);

    if (0 != phy_addr->lpn_in_page)
    {
        fatalerror("flash addr not page align");
    }

    if (BUF_SIZE != read_req->data_length)
    {
        fatalerror("not full page data");
    }

    sim_page_data = &sim_flash_data[phy_addr->pu_index]->block[phy_addr->block_in_pu].page[phy_addr->page_in_block];

    buffer = read_req->data_buffer_addr;

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        sim_get_flash_data(&sim_page_data->lpn[i], buffer + LPN_SIZE * i);
    }
    
    return SIM_SUCCESS;
}

U32 sim_flash_read_lpn(const struct flash_addr_t *phy_addr, const struct flash_req_t *read_req)
{
    struct sim_flash_page_data_t *sim_page_data;

    sim_check_flash_addr(phy_addr);
    sim_check_flash_req(read_req);

    sim_page_data = &sim_flash_data[phy_addr->pu_index]->block[phy_addr->block_in_pu].page[phy_addr->page_in_block];

    sim_get_flash_data(&sim_page_data->lpn[phy_addr->lpn_in_page], read_req->data_buffer_addr);
    
    return SIM_SUCCESS;
}


/*====================End of this file========================================*/

