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

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        for (block = 0; block < BLK_PER_PLN; block++)
        {
            flash_erase(pu, block);
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
        
        for (v_blk = 0; v_blk < BLK_PER_PLN; v_blk++)
        {
            vbt[pu]->item[v_blk].lpn_dirty_count = 0;
            vbt[pu]->item[v_blk].reserved = 0;
            
            p_blk = search_a_valid_block(pu, p_blk);
            table_set_phy_block(pu, v_blk, p_blk);

            if (INVALID_8F != p_blk)
            {
                table_set_vir_block(pu, p_blk, v_blk);
                p_blk++;
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
        for (block = 0; block < BLK_PER_PLN; block++)
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
        for (block = 0; block < BLK_PER_PLN; block++)
        {
            table_init_rpmt_block(&rpmt[pu]->block[block]);
        }
    }
}

void table_init_block_info(struct block_info_t* block_info)
{
    block_info->status = BLOCK_STATUS_FREE;
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
        for (block = 0; block < BLK_PER_PLN; block++)
        {
            pu_info[pu]->bad_block_count = 0; //to be defined by bbt
            pu_info[pu]->free_block_count = 0;//BLK_PER_PLN;
            pu_info[pu]->block_count = BLK_PER_PLN;
            pu_info[pu]->curr_block = 0;
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


U32 flash_alloc_block(U32 pu)
{
    struct pu_info_t *puinfo;
    struct block_info_t *blockinfo;
    U32 target_block;

    
    puinfo = pu_info[pu];

    if (0 == puinfo->free_block_count)
    {
        return 0xfffffffful;
    }
    
    target_block = (puinfo->curr_block + 1)%BLK_PER_PLN;
    blockinfo = &puinfo->block_info[target_block];

    while (BLOCK_STATUS_FREE != blockinfo->status)
    {
        target_block++;
        if(BLK_PER_PLN == target_block)
        {
            target_block = 0;
        }

        blockinfo = &puinfo->block_info[target_block];
    }

    puinfo->curr_block = target_block;
    puinfo->free_block_count--;
    blockinfo->status = BLOCK_STATUS_ALLOCATED;

    if (PG_PER_BLK != blockinfo->free_page_count)
    {
        fatalerror("new block page count error");
    }

    return target_block;
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
    curr_block = puinfo->curr_block;

    blockinfo = &puinfo->block_info[curr_block];

    if ((BLOCK_STATUS_ALLOCATED == blockinfo->status)&&(blockinfo->free_page_count > 0))
    {
        target_vir_addr.block_in_pu = curr_block;
        target_vir_addr.page_in_block = PG_PER_BLK - blockinfo->free_page_count;
        blockinfo->free_page_count--;
    }
    else
    {
        curr_block = flash_alloc_block(pu);

        if (0xfffffffful != curr_block)
        {
            blockinfo = &puinfo->block_info[curr_block];
            target_vir_addr.block_in_pu = curr_block;
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
    
    assert_null_pointer(old_addr);
    assert_null_pointer(new_addr);

    if (addr_valid(old_addr))
    {
        if (old_addr->pu_index != new_addr->pu_index)
        {
            fatalerror("not in the same pu");
        }
        old_pu = old_addr->pu_index;
        old_blk = old_addr->block_in_pu;
        old_page = old_addr->page_in_block;
        
        old_rpmt = &rpmt[old_pu]->block[old_blk];
        old_rpmt->lpn[old_page * LPN_PER_BUF + old_addr->lpn_in_page] = 0xfffffffful;
        vbt[old_pu]->item[old_blk].lpn_dirty_count++;

        if (LPN_IN_BLK == vbt[old_pu]->item[old_blk].lpn_dirty_count)
        {
            flash_erase(old_pu, vbt[old_pu]->item[old_blk].phy_block_addr);
            dbg_print("erase a all dirty block, pu(%d) blk(%d)\n", old_pu, old_blk);
        }
    }

    new_rpmt = &rpmt[new_addr->pu_index]->block[new_addr->block_in_pu];
    new_rpmt->lpn[new_addr->page_in_block * LPN_PER_BUF + new_addr->lpn_in_page] = lpn;

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
    struct flash_addr_t *old_vir_addr = get_loc_in_pmt(lpn);

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
        pu_info[pu]->bad_block_count++;
        pu_info[pu]->block_info[phy_block_addr].status = BLOCK_STATUS_BADBLCOK;
        vbt[pu]->item[vir_block_addr].phy_block_addr = 0xffff;
        //pbt[pu]->item[phy_block_addr].virtual_block_addr = 0xffff;
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

    for (p_blk = start_phy_block_addr; p_blk < BLK_PER_PLN; p_blk++)
    {
        if (BLOCK_STATUS_BADBLCOK != pu_info[pu]->block_info[p_blk].status)
        {
            return p_blk;
        }
    }

    return INVALID_8F;
}

/*====================End of this file========================================*/

