/*
* File Name    : table.c
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

#include "basedefine.h"
#include "disk_config.h"
#include "table.h"
#include "ftl.h"
#include "flash_interface.h"
#include "gc.h"
/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

/*============================================================================*/
/* extern region: extern global variable & function prototype                 */
/*============================================================================*/

/*============================================================================*/
/* global region: declare global variable                                     */
/*============================================================================*/
//struct bbt_t *bbt[MAX_PU_NUM];
struct vbt_t *vbt[MAX_PU_NUM];
struct pbt_t *pbt[MAX_PU_NUM];
struct pmt_t *pmt[MAX_PU_NUM];
struct rpmt_t *rpmt[MAX_PU_NUM];
struct pu_info_t *pu_info[MAX_PU_NUM];

/*============================================================================*/
/* local region:  declare local variable & local function prototype           */
/*============================================================================*/

/*============================================================================*/
/* main code region: function implement                                       */
/*============================================================================*/

void table_init(void)
{
    U32 pu;
    U32 pbt_base = get_pbt_baseaddr();
    U32 vbt_base = get_vbt_baseaddr();
    U32 pmt_base = get_pmt_baseaddr();
    U32 rpmt_base = get_rpmt_baseaddr();
    U32 puinfo_base = get_puinfo_baseaddr();

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        vbt[pu] = (struct vbt_t *)(vbt_base + sizeof(struct vbt_t) * pu);
        pbt[pu] = (struct pbt_t *)(pbt_base + sizeof(struct pbt_t) * pu);
        pmt[pu] = (struct pmt_t *)(pmt_base + sizeof(struct pmt_t) * pu);
        rpmt[pu] = (struct rpmt_t *)(rpmt_base + sizeof(struct rpmt_t) * pu);
        pu_info[pu] = (struct pu_info_t *)(puinfo_base + sizeof(struct pu_info_t) * pu);
    }
}

void table_llf_bbt(void)
{
    U32 pu;
    U32 block;
    U32 success_count;

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        success_count = 0;
        
        for (block = 0; block < pBLK_PER_PLN; block++)
        {
            if (SUCCESS == flash_erase(pu, block))
            {
                success_count++;
                if (success_count <= vBLK_PER_PLN)
                {
                    update_tables_after_erase(pu, block, SUCCESS);
                }
            }
            else
            {
                update_tables_after_erase(pu, block, ERROR_FLASH_ERASE);
            }
        }
    }
}

void table_llf_vbt(void)
{
    U32 pu;
    U32 v_blk;
    U32 p_blk;
    
    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        p_blk = 0;
        
        for (v_blk = 0; v_blk < vBLK_PER_PLN; v_blk++)
        {
            vbt[pu]->item[v_blk].lpn_dirty_count = 0;
            vbt[pu]->item[v_blk].reserved = 0;
            
            p_blk = search_a_valid_block(pu, p_blk);

            if (INVALID_8F != p_blk)
            {
                table_set_vir_block(pu, p_blk, v_blk);
                table_set_phy_block(pu, v_blk, p_blk);
                p_blk++;
            }
            else
            {
                fatalerror("not enough valid phy block in pu");
            }
        }
    }
}

void table_llf_pbt(void)
{
    U32 pu;
    U32 block;
    
    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        for (block = 0; block < pBLK_PER_PLN; block++)
        {
            //pbt[pu]->item[block].virtual_block_addr = block;
            pbt[pu]->item[block].block_erase_count = 0;
            pbt[pu]->item[block].reserved = 0;
        }
    }
}

void table_init_pmt_page(struct pmt_page_t *pmt_page)
{
    U32 lpn;

    for (lpn = 0; lpn < LPN_CNT_PER_PMTPAGE; lpn++)
    {
        pmt_page->item[lpn].vir_flash_addr.ppn = 0xfffffffful;
    }
}

void table_llf_pmt(void)
{
    U32 pu;
    U32 page;
    
    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        for (page = 0; page < PMT_PAGE_IN_PU; page++)
        {
            table_init_pmt_page(&pmt[pu]->page[page]);
        }
    }
}

void table_init_rpmt_block(struct rpmt_item_t *rpmt_block)
{
    U32 lpn;

    for (lpn = 0; lpn < LPN_IN_BLK; lpn++)
    {
        rpmt_block->lpn[lpn] = 0xfffffffful;
    }
}

void table_llf_rpmt(void)
{
    U32 pu;
    U32 block;
    
    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        for (block = 0; block < vBLK_PER_PLN; block++)
        {
            table_init_rpmt_block(&rpmt[pu]->block[block]);
        }
    }
}

void table_init_block_info(struct block_info_t* block_info)
{
    block_info->status = BLOCK_STATUS_RSV;
    block_info->erase_count = 0;    //to be defined
    block_info->free_page_count = PG_PER_BLK;
    block_info->vir_block_addr = 0xffff;
}

void table_init_bba(U16 *bad_block_addr)
{
    U32 i;

    for (i = 0; i < MAX_BB_PER_PLN; i++)
    {
        bad_block_addr[i] = 0xffff;
    }
}

void init_pu_info(void)
{
    U32 pu;
    U32 block;
    
    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        for (block = 0; block < pBLK_PER_PLN; block++)
        {
            pu_info[pu]->bad_block_count = 0; //to be defined by bbt
            pu_info[pu]->free_block_count = 0;//BLK_PER_PLN;
            pu_info[pu]->rsv_block_count = BB_RSV_BLOCK;
            pu_info[pu]->total_block_count = pBLK_PER_PLN;
            pu_info[pu]->curr_user_write_block = INVALID_4F;
            pu_info[pu]->curr_gc_write_block = INVALID_4F;
            table_init_bba(&pu_info[pu]->bad_block_addr[0]);
            table_init_block_info(&pu_info[pu]->block_info[block]);
        }
    }
}


void vir_to_phy_addr(const struct flash_addr_t *vir_addr_from, struct flash_addr_t *phy_addr_to)
{
    U32 pu;
    U32 vir_block;

    phy_addr_to->ppn = vir_addr_from->ppn;

    if (addr_valid(vir_addr_from))
    {
        pu = vir_addr_from->pu_index;
        vir_block = vir_addr_from->block_in_pu;
        phy_addr_to->block_in_pu = vbt[pu]->item[vir_block].phy_block_addr;
    }
}

void phy_to_vir_addr(const struct flash_addr_t *phy_addr_from, struct flash_addr_t *vir_addr_to)
{
    U32 pu;
    U32 phy_block;

    vir_addr_to->ppn = phy_addr_from->ppn;

    if (addr_valid(phy_addr_from))
    {
        pu = phy_addr_from->pu_index;
        phy_block = phy_addr_from->block_in_pu;
        vir_addr_to->block_in_pu = table_get_vir_block(pu, phy_block);// vbt[pu]->item[vir_block].phy_block_addr;
    }
}


U32 flash_alloc_block(U32 pu, EBT block_type)
{
    struct pu_info_t *puinfo;
    struct block_info_t *blockinfo;
    U32 phy_block;
    U32 vir_block = INVALID_8F;
    U32 loop = 0;
    
    puinfo = pu_info[pu];

    if (0 == puinfo->free_block_count)
    {
        fatalerror("no more free block");
    }

    // search start from this block, no matter block type
    phy_block = puinfo->curr_user_write_block;
    if (INVALID_4F == phy_block)
    {
        phy_block = 0;
    }
    
    while (loop++ < pBLK_PER_PLN)
    {
        blockinfo = &puinfo->block_info[phy_block];

        if ((BLOCK_STATUS_FREE == blockinfo->status) && (INVALID_4F != blockinfo->vir_block_addr))
        {
            vir_block = blockinfo->vir_block_addr;
            break;
        }
        
        phy_block++;
        if(pBLK_PER_PLN == phy_block)
        {
            phy_block = 0;
        }
    }

    if (INVALID_8F != vir_block)
    {
        if (BLOCK_TYPE_USER_DATA_WRITE == block_type)
        {
            puinfo->curr_user_write_block = phy_block;
        }
        else if (BLOCK_TYPE_GC_WRITE == block_type)
        {
            puinfo->curr_gc_write_block = phy_block;
        }
        else
        {
            fatalerror("to be continue");
        }
        
        puinfo->free_block_count--;
        blockinfo->status = BLOCK_STATUS_ALLOCATED;
    }
    else
    {
        dbg_print("no free flash resource in pu(%d)\n",pu);
        fatalerror("no free block");
    }
    
    if (PG_PER_BLK != blockinfo->free_page_count)
    {
        fatalerror("new block page count error");
    }

    if (table_get_phy_block(pu, vir_block) != phy_block)
    {
        fatalerror("vir block/phy block not match");
    }

    return phy_block;
}

struct flash_addr_t flash_alloc_page(U32 pu)
{
    struct flash_addr_t target_vir_addr;
    struct pu_info_t *puinfo;
    struct block_info_t *blockinfo;
    U32 curr_block;

    target_vir_addr.pu_index = pu;
    target_vir_addr.lpn_in_page = 0;

    puinfo = pu_info[pu];
    curr_block = puinfo->curr_user_write_block;

    if (INVALID_4F == curr_block)
    {
        curr_block = flash_alloc_block(pu, BLOCK_TYPE_USER_DATA_WRITE);
    }

    blockinfo = &puinfo->block_info[curr_block];

    if ((BLOCK_STATUS_ALLOCATED == blockinfo->status)&&(blockinfo->free_page_count > 0))
    {
        target_vir_addr.block_in_pu = blockinfo->vir_block_addr;
        target_vir_addr.page_in_block = PG_PER_BLK - blockinfo->free_page_count;
        blockinfo->free_page_count--;

        if (0 == blockinfo->free_page_count)
        {
            blockinfo->status = BLOCK_STATUS_FULL;
        }
    }
    else
    {
        curr_block = flash_alloc_block(pu, BLOCK_TYPE_USER_DATA_WRITE);

        if (puinfo->free_block_count < (FTL_RSV_BLOCK/2))
        {
            gc_start(pu);
        }

        if (0xfffffffful != curr_block)
        {
            blockinfo = &puinfo->block_info[curr_block];
            target_vir_addr.block_in_pu = blockinfo->vir_block_addr;
            target_vir_addr.page_in_block = PG_PER_BLK - blockinfo->free_page_count;
            blockinfo->free_page_count--;
        }
        else
        {
            target_vir_addr.ppn = 0xfffffffful;
        }
    }

    return target_vir_addr;
}

U32 table_update_rpmt(U32 lpn, const struct flash_addr_t *old_addr, const struct flash_addr_t *new_addr)
{
    struct rpmt_item_t *new_rpmt;
    struct rpmt_item_t *old_rpmt;
    U32 old_pu;
    U32 old_blk;
    U32 old_page;

    static struct flash_addr_t debug_addr;
    
    assert_null_pointer(new_addr);

    if ((NULL != old_addr) && addr_valid(old_addr))
    {
        if (old_addr->pu_index != new_addr->pu_index)
        {
            fatalerror("not in the same pu");
        }

        if (old_addr->ppn == new_addr->ppn)
        {
            dbg_print("lpn(%d) ppn from %d to %d\n",lpn,old_addr->ppn,new_addr->ppn);
            fatalerror("in the same vir_addr");
        }
        
        old_pu = old_addr->pu_index;
        old_blk = old_addr->block_in_pu;
        old_page = old_addr->page_in_block;
        
        old_rpmt = &rpmt[old_pu]->block[old_blk];

        /* double check */
        if (lpn != old_rpmt->lpn[old_page * LPN_PER_BUF + old_addr->lpn_in_page])
        {
            dbg_print("lpn(%d)=>vaddr(%d-%d-%d-%d),ppn(0x%x)=>rpmt->lpn(%d)\n",
            lpn,old_pu,old_blk,old_page,old_addr->lpn_in_page,old_addr->ppn,
            old_rpmt->lpn[old_page * LPN_PER_BUF + old_addr->lpn_in_page]);

            old_rpmt = &rpmt[debug_addr.pu_index]->block[debug_addr.block_in_pu];
            dbg_print("last valid vaddr->ppn(0x%x) => rpmt->lpn(%d)\n",debug_addr.ppn,
            old_rpmt->lpn[debug_addr.page_in_block * LPN_PER_BUF + debug_addr.lpn_in_page]);
            
            fatalerror("rpmt not match pmt");
        }
        
        old_rpmt->lpn[old_page * LPN_PER_BUF + old_addr->lpn_in_page] = 0xfffffffful;
        vbt[old_pu]->item[old_blk].lpn_dirty_count++;

    }

    new_rpmt = &rpmt[new_addr->pu_index]->block[new_addr->block_in_pu];
    new_rpmt->lpn[new_addr->page_in_block * LPN_PER_BUF + new_addr->lpn_in_page] = lpn;

    /*if ((0 == lpn) || (49152 == lpn)) //debug lpn
    {
        dbg_print("lpn(%d): vaddr ppn from 0x%x to 0x%x\n",lpn, old_addr->ppn, new_addr->ppn);
        debug_addr.ppn = new_addr->ppn;
    }*/

    return SUCCESS;
}

struct flash_addr_t *get_loc_in_pmt(U32 lpn)
{
    U32 pu;
    U32 lpn_in_pu;
    U32 pmtpage_in_pu;
    U32 offset_in_mptpage;

    pu = get_pu_from_lpn(lpn);

    lpn_in_pu = ((lpn/PU_NUM) & (~LPN_PER_BUF_MSK)) + (lpn % LPN_PER_BUF);

    pmtpage_in_pu = lpn_in_pu/LPN_CNT_PER_PMTPAGE;
    
    offset_in_mptpage = lpn_in_pu%LPN_CNT_PER_PMTPAGE;

    return &pmt[pu]->page[pmtpage_in_pu].item[offset_in_mptpage].vir_flash_addr;
}


U32 table_update_pmt(U32 lpn, const struct flash_addr_t *new_vir_addr)
{
    struct flash_addr_t *old_vir_addr;

    if (lpn >= MAX_LPN_IN_DISK)
    {
        fatalerror("invalid lpn");
    }

    old_vir_addr = get_loc_in_pmt(lpn);

    table_update_rpmt(lpn, old_vir_addr, new_vir_addr);
    
    old_vir_addr->ppn = new_vir_addr->ppn;

    return SUCCESS;
}

U32 table_lookup_pmt(U32 lpn, struct flash_addr_t *dest_vir_addr)
{
    struct flash_addr_t *src_vir_addr = get_loc_in_pmt(lpn);

    dest_vir_addr->ppn = src_vir_addr->ppn;

    return SUCCESS;
}

U32 update_tables_after_erase(U32 pu, U32 phy_block_addr, U32 erase_status)
{
    U32 vir_block_addr = table_get_vir_block(pu, phy_block_addr);
    
    if (SUCCESS == erase_status)
    {
        //pbt[pu]->item[phy_block_addr].block_erase_count++;
        vbt[pu]->item[vir_block_addr].lpn_dirty_count = 0;
        pu_info[pu]->free_block_count++;
        pu_info[pu]->block_info[phy_block_addr].erase_count++;
        pu_info[pu]->block_info[phy_block_addr].status = BLOCK_STATUS_FREE;
        pu_info[pu]->block_info[phy_block_addr].free_page_count = PG_PER_BLK;
    }
    else
    {
        pu_info[pu]->bad_block_addr[pu_info[pu]->bad_block_count] = phy_block_addr;
        pu_info[pu]->bad_block_count++;
        pu_info[pu]->rsv_block_count++;
        pu_info[pu]->block_info[phy_block_addr].erase_count++;
        pu_info[pu]->block_info[phy_block_addr].status = BLOCK_STATUS_BADBLCOK;
        vbt[pu]->item[vir_block_addr].phy_block_addr = 0xffff;
        //pbt[pu]->item[phy_block_addr].virtual_block_addr = 0xffff;

        if (pu_info[pu]->bad_block_count > MAX_BB_PER_PLN)
        {
            fatalerror("too much bad block");
        }
    }

    return SUCCESS;
}

void table_set_vir_block(U32 pu, U32 phy_block_addr, U32 vir_block_addr)
{
    pu_info[pu]->block_info[phy_block_addr].vir_block_addr = vir_block_addr;
}

U32 table_get_vir_block(U32 pu, U32 phy_block_addr)
{
    return (U32)pu_info[pu]->block_info[phy_block_addr].vir_block_addr;
}

void table_set_phy_block(U32 pu, U32 vir_block_addr, U32 phy_block_addr)
{
    vbt[pu]->item[vir_block_addr].phy_block_addr = phy_block_addr;
}


U32 table_get_phy_block(U32 pu, U32 vir_block_addr)
{
    return (U32)vbt[pu]->item[vir_block_addr].phy_block_addr;
}

U32 search_a_valid_block(U32 pu, U32 start_phy_block_addr)
{
    U32 p_blk;

    if (INVALID_8F == start_phy_block_addr)
    {
        return INVALID_8F;
    }

    for (p_blk = start_phy_block_addr; p_blk < pBLK_PER_PLN; p_blk++)
    {
        if ((BLOCK_STATUS_BADBLCOK != pu_info[pu]->block_info[p_blk].status)
        &&(BLOCK_STATUS_RSV != pu_info[pu]->block_info[p_blk].status))
        {
            return p_blk;
        }
    }

    return INVALID_8F;
}

void show_pu_info(U32 pu)
{
    struct pu_info_t *puinfo = pu_info[pu];
    struct block_info_t *blockinfo;
    U32 p_blk;
    U32 curr_dirty_count;
    U32 max_erase_count = 0;
    U32 min_erase_count = 0xfffffffe;
    U32 total_erase_count = 0;
    U32 used_block_count = 0;
    U32 max_dirty_count = 0;
    U32 min_dirty_count = 0xfffffffe;
    U32 total_dirty_count = 0;

    dbg_print("####################################\n");
    dbg_print("pu %d information:\n",pu);
    dbg_print("total_block_count: %d\n", puinfo->total_block_count);
    dbg_print("bad_block_count:   %d\n", puinfo->bad_block_count);
    dbg_print("free_block_count:  %d\n", puinfo->free_block_count);
    dbg_print("rsv_block_count:   %d\n", puinfo->rsv_block_count);

    for (p_blk = 0; p_blk < pBLK_PER_PLN; p_blk++)
    {
        blockinfo = &puinfo->block_info[p_blk];
        if ((BLOCK_STATUS_RSV == blockinfo->status) || (BLOCK_STATUS_BADBLCOK == blockinfo->status))
        {
            continue;
        }

        used_block_count++;
        
        curr_dirty_count = vbt[pu]->item[blockinfo->vir_block_addr].lpn_dirty_count;
        
        total_dirty_count += curr_dirty_count;
        max_dirty_count = max(curr_dirty_count, max_dirty_count);
        min_dirty_count = min(curr_dirty_count, min_dirty_count);

        total_erase_count += blockinfo->erase_count;
        max_erase_count = max(blockinfo->erase_count, max_erase_count);
        min_erase_count = min(blockinfo->erase_count, min_erase_count);
    }
    
    dbg_print("max erase count: %d\n", max_erase_count);
    dbg_print("min erase count: %d\n", min_erase_count);
    dbg_print("avr erase count: %d\n", total_erase_count/used_block_count);
    dbg_print("max dirty count: %d\n", max_dirty_count);
    dbg_print("min dirty count: %d\n", min_dirty_count);
    dbg_print("avr dirty count: %d\n", total_dirty_count/used_block_count);
    
}

void show_detail_info(void)
{
    U32 pu;

    for (pu = 0 ; pu < PU_NUM; pu++)
    {
        show_pu_info(pu);
    }
}

/*====================End of this file========================================*/

