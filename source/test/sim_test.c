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
#include <memory.h>
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
static U32 lpn_write_count[MAX_LPN_IN_DISK];

/*============================================================================*/
/* main code region: function implement                                       */
/*============================================================================*/
void sim_test_init(void)
{
    memset(lpn_write_count, 0, sizeof(lpn_write_count));
}

static void sim_set_write_data(U32 buffer, U32 lpn)
{
    lpn_write_count[lpn]++;

    *(U32*)buffer = lpn;
    *(U32*)(buffer + sizeof(U32)) = lpn_write_count[lpn];
}

static void sim_check_read_data(U32 buffer, U32 lpn)
{
    U32 read_lpn;
    U32 read_count;

    read_lpn = *(U32*)buffer;
    read_count = *(U32*)(buffer + sizeof(U32));
    
    if (lpn != read_lpn)
    {
        printf("lpn = %d, but read result is %d\n", lpn, read_lpn);
        dbg_getch();
    }

    if (lpn_write_count[lpn] != read_count)
    {
        printf("lpn(%d) write count = %d, but read result is %d\n", lpn, lpn_write_count[lpn], read_count);
        dbg_getch();
    }
}

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

    write_req.request_type = FRT_RAN_WRITE;
    write_req.lpn_count = 1;
    write_req.lpn_list[0] = lpn;
    write_req.buffer_addr = (U32)data_buffer;

    sim_set_write_data((U32)data_buffer, lpn);

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
        sim_set_write_data((U32)(data_buffer + LPN_SIZE * i), (start_lpn + i));
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


void sim_random_read_lpn(U32 lpn)
{
    U8 *data_buffer;
    
    struct ftl_req_t read_req;

    data_buffer = (U8 *)malloc(LPN_SIZE);

    if (NULL == data_buffer)
    {
        printf("no memory!\n");
        return;
    }

    read_req.request_type = FRT_RAN_READ;
    read_req.lpn_count = 1;
    read_req.lpn_list[0] = lpn;
    read_req.buffer_addr = (U32)data_buffer;

    ftl_read(&read_req);

    //if read_operation_finished
    {
        sim_check_read_data((U32)data_buffer, lpn);
        free(data_buffer);
        data_buffer = NULL;
    }
}

void sim_seq_read_page(U32 start_lpn)
{
    U32 i;
    U8 *data_buffer;
    
    struct ftl_req_t read_req;

    data_buffer = (U8 *)malloc(BUF_SIZE);

    if (NULL == data_buffer)
    {
        printf("no memory!\n");
        return;
    }

    read_req.request_type = FRT_SEQ_READ;
    read_req.lpn_count = LPN_PER_BUF;
    read_req.buffer_addr = (U32)data_buffer;

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        read_req.lpn_list[i] = start_lpn + i;
    }
    
    ftl_read(&read_req);

    //if read_operation_finished
    {
        for (i = 0; i < LPN_PER_BUF; i++)
        {
            sim_check_read_data((U32)(data_buffer + LPN_SIZE * i), start_lpn + i);
        }
        
        free(data_buffer);
        data_buffer = NULL;
    }
}

void sim_read(U32 start_lpn, U32 lpn_count)
{
    U32 lpn = start_lpn;
    U32 remain = lpn_count;
    
    while(lpn_not_page_align(lpn) && (remain > 0))
    {
        sim_random_read_lpn(lpn++);
        remain--;
    }

    while(remain >= LPN_PER_BUF)
    {
        sim_seq_read_page(lpn);
        lpn += LPN_PER_BUF;
        remain -= LPN_PER_BUF;
    }

    while(remain > 0)
    {
        sim_random_read_lpn(lpn++);
        remain--;
    }
}


void test_write_whole_disk(void)
{
    sim_write(0, MAX_LPN_IN_DISK);
}

void test_read_whole_disk(void)
{
    sim_read(0, MAX_LPN_IN_DISK);
}

U32 random_op_count = 0;

U32 gen_random_number(U32 max_num)
{
    return rand()%max_num;
}


void test_random_readwrite(void)
{
    U32 running;
    U32 write_start_lpn;
    U32 write_lpn_count;
    U32 read_start_lpn;
    U32 read_lpn_count;

    running = 1;
    
    while(running)
    {
        write_start_lpn = gen_random_number(MAX_LPN_IN_DISK);
        write_lpn_count = gen_random_number(LPN_PER_BUF);

        if (write_start_lpn + write_lpn_count >= MAX_LPN_IN_DISK)
        {
            write_lpn_count = MAX_LPN_IN_DISK - write_start_lpn;
        }

        sim_write(write_start_lpn, write_lpn_count);
        random_op_count++;
        
        if (0 == random_op_count%5000)
        {
            dbg_print("%d random write(%d,%d)\n", random_op_count, write_start_lpn, write_lpn_count);
            read_start_lpn = gen_random_number(MAX_LPN_IN_DISK);
            read_lpn_count = gen_random_number(MAX_LPN_IN_DISK);
            if (read_start_lpn + read_lpn_count >= MAX_LPN_IN_DISK)
            {
                read_lpn_count = MAX_LPN_IN_DISK - read_start_lpn;
            }
            dbg_print("%d random read(%d,%d)\n", random_op_count/5000, read_start_lpn, read_lpn_count);
            sim_read(read_start_lpn, read_lpn_count);
        }

    }
}

void run_test_cases(void)
{
    test_write_whole_disk();
    test_read_whole_disk();
    sim_write(0,1);
    sim_read(0,LPN_PER_BUF);

    test_write_whole_disk();
    test_write_whole_disk();
    test_write_whole_disk();
    sim_write(10,3);
    sim_write(10,3);
    sim_write(10,3);
    sim_read(10,3);
    test_read_whole_disk();

    test_random_readwrite();
}


/*====================End of this file========================================*/

