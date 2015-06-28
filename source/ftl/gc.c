#include <string.h>
#include "disk_config.h"
#include "table.h"
#include "ftl.h"
#include "flash_interface.h"
#include "gc.h"

extern U8 *g_device_dram_addr;
extern struct vbt_t *vbt[MAX_PU_NUM];
extern struct rpmt_t *rpmt[MAX_PU_NUM];
extern struct pu_info_t *pu_info[MAX_PU_NUM];

static U32 gc_dest_block[PU_NUM] = {INVALID_8F};

LOCAL struct ftl_req_t gc_req[PU_NUM];


static void reset_gcr(struct ftl_req_t* gcr)
{
    U32 i;

    gcr->lpn_count = 0;

    for (i = 0; i < LPN_PER_BUF; i++)
    {
        gcr->lpn_list[i] = INVALID_8F;
    }
}


void gc_init(void)
{
    U32 pu;
    
    memset(gc_req, 0, sizeof(gc_req));
    
    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        gc_req[pu].request_type = FRT_GC_WRITE;
        gc_req[pu].buffer_addr = BUFFER_DRAM_ADDR + BUF_SIZE * (PU_NUM + pu);
        reset_gcr(&gc_req[pu]);
    }
}

U32 gc_search_source_block(U32 pu)
{
    U32 i;
    struct vbt_t *vbtinfo;
    U32 vir_block = 0;
    U32 max_dirty_count = 0;

    vbtinfo = vbt[pu];

    for (i = 0; i < vBLK_PER_PLN; i++)
    {
        if (vbtinfo->item[i].lpn_dirty_count > max_dirty_count)
        {
            vir_block = i;
            max_dirty_count = vbtinfo->item[i].lpn_dirty_count;
        }
    }

    dbg_print("try gc pu(%d) vblk(%d) dirtycount(%d)\n",pu,vir_block,max_dirty_count);

    if (max_dirty_count < LPN_IN_BLK/2) /* not need gc */
    {
        vir_block = INVALID_8F;
    }

    return vir_block;
}

U32 gc_alloc_page(U32 pu)
{
    struct flash_addr_t target_vir_addr;
    struct pu_info_t *puinfo;
    struct block_info_t *blockinfo;
    U32 dst_p_blk;

    if (INVALID_8F == gc_dest_block[pu])
    {
        gc_dest_block[pu] = flash_alloc_block(pu);
    }

    dst_p_blk = gc_dest_block[pu];

    target_vir_addr.pu_index = pu;
    target_vir_addr.lpn_in_page = 0;

    puinfo = pu_info[pu];
    blockinfo = &puinfo->block_info[dst_p_blk];

    if ((BLOCK_STATUS_ALLOCATED == blockinfo->status)&&(blockinfo->free_page_count > 0))
    {
        target_vir_addr.block_in_pu = blockinfo->vir_block_addr;
        target_vir_addr.page_in_block = PG_PER_BLK - blockinfo->free_page_count;
        blockinfo->free_page_count--;
    }
    else
    {
        dst_p_blk = flash_alloc_block(pu);

        if (0xfffffffful != dst_p_blk)
        {
            blockinfo = &puinfo->block_info[dst_p_blk];
            target_vir_addr.block_in_pu = blockinfo->vir_block_addr;
            target_vir_addr.page_in_block = PG_PER_BLK - blockinfo->free_page_count;
            blockinfo->free_page_count--;

            gc_dest_block[pu] = dst_p_blk;
        }
        else
        {
            target_vir_addr.ppn = 0xfffffffful;
        }
    }

    return target_vir_addr.ppn;
}


U32 gc_write_page(const struct ftl_req_t *write_request)
{
    U32 lpn;
    U32 pu;
    U32 i;
    struct flash_addr_t vir_addr;
    struct flash_addr_t phy_addr;
    struct flash_req_t  flash_write_req;

    lpn = write_request->lpn_list[0];
    
    pu = get_pu_from_lpn(lpn);

    vir_addr.ppn = gc_alloc_page(pu);

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

U32 garbage_collection(U32 pu, U32 src_v_blk)
{
    U32 i;
    U32 lpn;
    struct ftl_req_t *gcr;
    struct flash_addr_t src_v_addr;
    struct flash_addr_t src_p_addr;
    struct flash_req_t read;

    gcr = &gc_req[pu];

    for (i = 0; i < LPN_IN_BLK; i++)
    {
        lpn = rpmt[pu]->block[src_v_blk].lpn[i];

        if (INVALID_8F == lpn)
        {
            //fatalerror("invalid lpn in rpmt while gc");
            continue;
        }

        table_lookup_pmt(lpn, &src_v_addr);

        if (INVALID_8F == src_v_addr.ppn)   /* dirty or unwrite */
        {
            continue;
        }

        /* double check */
        if ((src_v_addr.block_in_pu != src_v_blk) 
          ||(src_v_addr.page_in_block * LPN_PER_BUF + src_v_addr.lpn_in_page != i))
        {
            //or maybe means a dirty lpn?
            fatalerror("lpn unmatch flash address");
        }

        read.data_length = LPN_SIZE;
        read.data_buffer_addr = gcr->buffer_addr + LPN_SIZE * gcr->lpn_count;

        vir_to_phy_addr(&src_v_addr, &src_p_addr);
        flash_read(&src_p_addr, &read);
        
        gcr->lpn_list[gcr->lpn_count] = lpn;
        gcr->lpn_count++;
        if (LPN_PER_BUF == gcr->lpn_count)
        {
            gc_write_page(gcr);
            reset_gcr(gcr);
        }
    }

    dbg_print("gc erase, pu(%d) pblk(%d)\n",pu,src_p_addr.block_in_pu);
    flash_erase(pu, src_p_addr.block_in_pu);
    
    return SUCCESS;
}

U32 try_garbage_collection(U32 pu)
{
    U32 src_vir_block = gc_search_source_block(pu);

    while(INVALID_8F != src_vir_block)
    {
        dbg_print("gc start, pu(%d) vblk(%d)\n", pu, src_vir_block);
        garbage_collection(pu, src_vir_block);
        src_vir_block = gc_search_source_block(pu);
    }

    return SUCCESS;
}

void gc_start(U32 pu)
{
    try_garbage_collection(pu);
}

void gc_stop(U32 pu)
{}

