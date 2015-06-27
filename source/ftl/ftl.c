/*
* File Name    : ftl.c
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include <stdio.h>
#include <memory.h>

#include "disk_config.h"
#include "flash_interface.h"
#include "ftl.h"
#include "table.h"
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
LOCAL struct ftl_req_t unfull_write_req[PU_NUM];

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

static void reset_uwr(struct ftl_req_t* uwr)
{
    U32 i;

    uwr->lpn_count = 0;

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        uwr->lpn_list[i] = INVALID_8F;
    }
}

void other_init(void)
{
    U32 pu;

    memset(unfull_write_req, 0, sizeof(unfull_write_req));
    
    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        unfull_write_req[pu].request_type = FRT_RAN_WRITE;
        unfull_write_req[pu].buffer_addr = BUFFER_DRAM_ADDR + BUF_SIZE * pu;
        reset_uwr(&unfull_write_req[pu]);
    }
}

U32 ftl_llf(void)
{
    init_pu_info();
    table_llf_bbt();
    table_llf_vbt();
    //table_llf_pbt();  //pbt info move to the blockinfo in pu_info
    table_llf_pmt();
    table_llf_rpmt();
    return 0;
}

U32 ftl_init(void)
{
    table_init();
    ftl_llf();
    
    other_init();
    return 0;
}

U32 get_pu_from_lpn(U32 lpn)
{
    return (lpn >> LPN_PER_BUF_BITS ) % PU_NUM;
}

U32 addr_valid(const struct flash_addr_t *flash_addr)
{
    return (0xfffffffful != flash_addr->ppn);
}

U32 addr_invalid(const struct flash_addr_t *flash_addr)
{
    return (0xfffffffful == flash_addr->ppn);
}

U32 is_sequence_addr(const struct flash_addr_t *curr, const struct flash_addr_t *next)
{
    if ((curr->pu_index == next->pu_index)
     &&(curr->block_in_pu == next->block_in_pu)
     &&(curr->page_in_block == next->page_in_block)
     &&(curr->lpn_in_page + 1 == next->lpn_in_page))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


static U32 ftl_write_full_page(const struct ftl_req_t *write_request)
{
    U32 lpn;
    U32 pu;
    U32 i;
    struct flash_addr_t vir_addr;
    struct flash_addr_t phy_addr;
    struct flash_req_t  flash_write_req;

    lpn = write_request->lpn_list[0];
    
    pu = get_pu_from_lpn(lpn);

    vir_addr = flash_alloc_page(pu);

    if (addr_invalid(&vir_addr))
    {
        return ERROR_NO_FLASH_PAGE;
    }

    vir_to_phy_addr(&vir_addr, &phy_addr);

    for (i = 0; i < write_request->lpn_count; i++)
    {
        lpn = write_request->lpn_list[i];
        vir_addr.lpn_in_page = i;
        table_update_pmt(lpn, &vir_addr);
    }

    flash_write_req.data_buffer_addr = write_request->buffer_addr;
    flash_write_req.spare_buffer_addr = 0; //to be continue

    flash_write_req.data_length = BUF_SIZE;
    
    return flash_write(&phy_addr, &flash_write_req);
}



static U32 add_to_uwr(U32 lpn, U32 data_buffer)
{
    U32 pu;
    U32 i;
    struct ftl_req_t* uwr;
    U32 hit = FALSE;
    
    pu = get_pu_from_lpn(lpn);
    uwr = &unfull_write_req[pu];

    if (uwr->lpn_count > 0)
    {
        for (i = 0; i < LPN_PER_BUF; i++)
        {
            if (lpn == uwr->lpn_list[i])
            {
                dbg_print("write hit, lpn = %d\n", lpn);
                hit = TRUE;
                //uwr->lpn_count++;
                memcpy((void*)(uwr->buffer_addr + i * LPN_SIZE), (void*)data_buffer, LPN_SIZE);
                break;
            }
        }
    }

    if (FALSE == hit)
    {
        for (i = 0; i < LPN_PER_BUF; i++)
        {
            if (INVALID_8F == uwr->lpn_list[i])
            {
                uwr->lpn_list[i] = lpn;
                uwr->lpn_count++;
                memcpy((void*)(uwr->buffer_addr + i * LPN_SIZE), (void*)data_buffer, LPN_SIZE);
                break;
            }
        }

    }

    if (LPN_PER_BUF == uwr->lpn_count)
    {
        ftl_write_full_page(uwr);
        reset_uwr(uwr);
    }
    
    return SUCCESS;
}

void update_uwr(U32 start_lpn)
{
    U32 pu;
    U32 lpn_count;
    U32 lpn_in_uwr;
    U32 i;
    struct ftl_req_t* uwr;

    pu = get_pu_from_lpn(start_lpn);

    uwr = &unfull_write_req[pu];
    lpn_count = uwr->lpn_count;

    if (lpn_count >= LPN_PER_BUF)
    {
        fatalerror("lpn count in uwr error");
    }

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        if (0 == lpn_count)
        {
            break;
        }

        lpn_in_uwr = uwr->lpn_list[i];

        if (INVALID_8F != lpn_in_uwr)
        {
            if ((lpn_in_uwr >= start_lpn) && (lpn_in_uwr < start_lpn + LPN_PER_BUF))
            {
                dbg_print("overwrite uwr, lpn = %d\n", lpn_in_uwr);
                uwr->lpn_count--;
                uwr->lpn_list[i] = INVALID_8F;
            }

            lpn_count--;
        }
    }
}

U32 ftl_write(const struct ftl_req_t *write_request)
{
    U32 i;

    assert_null_pointer(write_request);

    if (FRT_SEQ_WRITE == write_request->request_type)
    {
        if ((LPN_PER_BUF != write_request->lpn_count) || (lpn_not_page_align(write_request->lpn_list[0])))
        {
            fatalerror("not seq write");
        }

        update_uwr(write_request->lpn_list[0]);
        
        return ftl_write_full_page(write_request);
    }
    else if(FRT_RAN_WRITE == write_request->request_type)
    {
        for (i = 0; i < write_request->lpn_count; i++)
        {
            add_to_uwr(write_request->lpn_list[i], write_request->buffer_addr + LPN_SIZE * i);
        }
    }
    else
    {
        fatalerror("unknown write request");
    }

    return SUCCESS;
}

EFRT read_request_type(const struct ftl_req_t *read_request)
{
    U32 i;
    struct flash_addr_t curr_vir_addr;
    struct flash_addr_t next_vir_addr;
    
    if (FRT_RAN_READ == read_request->request_type)
    {
        return FRT_RAN_READ;
    }

    if (FRT_SEQ_READ != read_request->request_type)
    {
        fatalerror("unknown read request type");
    }

    if (LPN_PER_BUF != read_request->lpn_count)
    {
        return FRT_RAN_READ;
    }

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        if (TRUE == hit_unflush_buffer(read_request->lpn_list[i]))
        {
            return FRT_RAN_READ;
        }
    }

    table_lookup_pmt(read_request->lpn_list[0], &curr_vir_addr);
    if (0 != curr_vir_addr.lpn_in_page)
    {
        return FRT_RAN_READ;
    }

    for (i = 1; i < LPN_PER_BUF; i++)
    {
        table_lookup_pmt(read_request->lpn_list[i], &next_vir_addr);

        if (FALSE == is_sequence_addr(&curr_vir_addr, &next_vir_addr))
        {
            return FRT_RAN_READ;
        }

        curr_vir_addr.ppn = next_vir_addr.ppn;
    }

    return FRT_SEQ_READ;
}


U32 ftl_read(const struct ftl_req_t *read_request)
{
    U32 i;
    U32 lpn;
    EFRT req_type;
    struct flash_addr_t vir_addr;
    struct flash_addr_t phy_addr;
    struct flash_req_t  flash_read_req;

    assert_null_pointer(read_request);

    req_type = read_request_type(read_request);

    if (FRT_SEQ_READ == req_type)
    {
        lpn = read_request->lpn_list[0];
        table_lookup_pmt(lpn, &vir_addr);
        vir_to_phy_addr(&vir_addr, &phy_addr);
        
        flash_read_req.data_buffer_addr = read_request->buffer_addr;
        flash_read_req.spare_buffer_addr = 0; //to be continue
        flash_read_req.data_length = BUF_SIZE;

        flash_read(&phy_addr, &flash_read_req);
    }
    else
    {
        for (i = 0; i < read_request->lpn_count; i++)
        {
            lpn = read_request->lpn_list[i];

            if (hit_unflush_buffer(lpn))
            {
                hit_read(lpn, read_request->buffer_addr + LPN_SIZE * i);
                printf("read hit unflush, lpn = %d!\n", lpn);
                continue;
            }
            
            table_lookup_pmt(lpn, &vir_addr);

            if (addr_valid(&vir_addr))
            {
                vir_to_phy_addr(&vir_addr, &phy_addr);
                
                flash_read_req.data_buffer_addr = read_request->buffer_addr + LPN_SIZE * i;
                flash_read_req.spare_buffer_addr = 0; //to be continue
                flash_read_req.data_length = LPN_SIZE;

                flash_read(&phy_addr, &flash_read_req);
            }
            else
            {
                printf("read without write, lpn = %d!\n", lpn);
            }
        }
    }

    return SUCCESS;
}


U32 hit_unflush_buffer(U32 lpn)
{
    U32 pu;
    U32 i;
    struct ftl_req_t* uwr;

    pu = get_pu_from_lpn(lpn);
    uwr = &unfull_write_req[pu];

    for (i = 0; i < uwr->lpn_count; i++)
    {
        if (lpn == uwr->lpn_list[i])
        {
            return TRUE;
        }
    }

    return FALSE;
}

U32 hit_read(U32 lpn, U32 buffer_addr)
{
    U32 pu;
    U32 i;
    struct ftl_req_t* uwr;

    pu = get_pu_from_lpn(lpn);
    uwr = &unfull_write_req[pu];

    for (i = 0; i < uwr->lpn_count; i++)
    {
        if (lpn == uwr->lpn_list[i])
        {
            memcpy((void*)buffer_addr, (void*)(uwr->buffer_addr + LPN_SIZE * i), LPN_SIZE);
            return TRUE;
        }
    }

    return FALSE;
}

/*====================End of this file========================================*/

