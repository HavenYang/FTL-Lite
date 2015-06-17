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
#include <stdio.h>
#include <stdlib.h>
#include "basedefine.h"
#include "disk_config.h"
#include "flash_interface.h"
#include "sim_flash.h"
#include "table.h"
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

void sim_random_write_lpn(U32 lpn)
{
    U8 *data_buffer;
    
    struct ftl_req_t write_req;

    data_buffer = (U8 *)malloc(LPN_SIZE);

    if (NULL == data_buffer)
    {
        printf("no memory!\n");
        return;
    }

    *(U32*)data_buffer = lpn;

    write_req.request_type = FRT_RAN_WRITE;
    write_req.lpn_count = 1;
    write_req.lpn_list[0] = lpn;
    write_req.buffer_addr = (U32)data_buffer;

    ftl_write(&write_req);

    //if write_operation_finished
    {
        free(data_buffer);
        data_buffer = NULL;
    }
}

void sim_seq_write_page(start_lpn)
{
    U8 *data_buffer;
    U32 i;
    
    struct ftl_req_t write_req;

    if (lpn_not_page_align(start_lpn)) //lpn is not page align
    {
        fatalerror("lpn is not page align");
    }

    data_buffer = (U8 *)malloc(BUF_SIZE);

    if (NULL == data_buffer)
    {
        printf("no memory!\n");
        return;
    }

    write_req.request_type = FRT_SEQ_WRITE;
    write_req.lpn_count = LPN_PER_BUF;
    write_req.buffer_addr = (U32)data_buffer;

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        write_req.lpn_list[i] = start_lpn + i;
        *(U32*)(data_buffer + LPN_SIZE * i ) = start_lpn + i;
    }
    
    ftl_write(&write_req);

    //if write_operation_finished
    {
        free(data_buffer);
        data_buffer = NULL;
    }
}

void sim_write(U32 start_lpn, U32 lpn_count)
{
    U32 lpn = start_lpn;
    U32 remain = lpn_count;
    
    while(lpn_not_page_align(lpn) && (remain > 0))
    {
        sim_random_write_lpn(lpn++);
        remain--;
    }

    while(remain >= LPN_PER_BUF)
    {
        sim_seq_write_page(lpn);
        lpn += LPN_PER_BUF;
        remain -= LPN_PER_BUF;
    }

    while(remain > 0)
    {
        sim_random_write_lpn(lpn++);
        remain--;
    }
}

void sim_write_whole_disk(void)
{
    sim_write(0, LPN_IN_PU * PU_NUM);
}

void run_test_cases(void)
{
    //sim_write_whole_disk();
    sim_write(0,LPN_PER_BUF);
}


/*====================End of this file========================================*/

