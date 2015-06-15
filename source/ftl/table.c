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
#include "basedefine.h"
#include "disk_config.h"
#include "env_sim.h"
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

void table_llf_bbt(void)
{
    //bbt = (struct bbt_t *)get_bbt_baseaddr();
}

void table_llf_vbt(void)
{
    U32 pu;
    U32 block;
    
    U32 vbt_base = get_vbt_baseaddr();

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        vbt[pu] = (struct vbt_t *)(vbt_base + sizeof(struct vbt_t) * pu);
        
        for (block = 0; block < BLK_PER_PLN; block++)
        {
            vbt[pu]->item[block].phy_block_addr = block; /* consider no bad block */
            vbt[pu]->item[block].lpn_dirty_count = 0;
            vbt[pu]->item[block].reserved = 0;
        }
    }
}

void table_llf_pbt(void)
{
    U32 pu;
    U32 block;
    
    U32 pbt_base = get_pbt_baseaddr();

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        pbt[pu] = (struct pbt_t *)(pbt_base + sizeof(struct pbt_t) * pu);
        
        for (block = 0; block < BLK_PER_PLN; block++)
        {
            pbt[pu]->item[block].virtual_block_addr = block; /* consider no bad block */
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
    
    U32 pmt_base = get_pmt_baseaddr();

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        pmt[pu] = (struct pmt_t *)(pmt_base + sizeof(struct pmt_t) * pu);
        
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
    
    U32 rpmt_base = get_rpmt_baseaddr();

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        rpmt[pu] = (struct rpmt_t *)(rpmt_base + sizeof(struct rpmt_t) * pu);
        
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
}


void init_pu_info(void)
{
    U32 pu;
    U32 block;
    
    U32 puinfo_base = get_puinfo_baseaddr();

    for (pu = 0; pu < MAX_PU_NUM; pu++)
    {
        pu_info[pu] = (struct pu_info_t *)(puinfo_base + sizeof(struct pu_info_t) * pu);
        
        for (block = 0; block < BLK_PER_PLN; block++)
        {
            pu_info[pu]->bad_block_count = 0; //to be defined by bbt
            pu_info[pu]->block_count = BLK_PER_PLN;
            pu_info[pu]->curr_block = 0;
            pu_info[pu]->free_block_count = BLK_PER_PLN - 2;    //to be defined by reserved block count
            table_init_block_info(&pu_info[pu]->block_info[block]);
        }
    }
}


struct flash_addr_t get_phy_flash_addr(struct flash_addr_t vir_addr)
{
    struct flash_addr_t phy_addr;
    U32 pu;
    U32 vir_block;
    
    phy_addr = vir_addr;
    pu = vir_addr.pu_index;
    vir_block = vir_addr.block_in_pu;

    phy_addr.block_in_pu = vbt[pu]->item[vir_block].phy_block_addr;

    return phy_addr;
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
        fatalerror();
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

    if (blockinfo->free_page_count > 0)
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
    
    assert_null_pointer(old_addr);
    assert_null_pointer(new_addr);

    if (old_addr->pu_index != new_addr->pu_index)
    {
        fatalerror();
    }

    new_rpmt = &rpmt[new_addr->pu_index]->block[new_addr->block_in_pu];
    old_rpmt = &rpmt[old_addr->pu_index]->block[old_addr->block_in_pu];

    old_rpmt->lpn[old_addr->page_in_block * LPN_PER_BUF + old_addr->lpn_in_page] = 0xfffffffful;
    new_rpmt->lpn[new_addr->page_in_block * LPN_PER_BUF + new_addr->lpn_in_page] = lpn;

    vbt[old_addr->pu_index]->item[old_addr->block_in_pu].lpn_dirty_count++;

    return SUCCESS;
}


U32 table_update_pmt(U32 lpn, const struct flash_addr_t *new_vir_addr)
{
    U32 pu;
    U32 lpn_in_pu;
    U32 pmtpage_in_pu;
    U32 offset_in_mptpage;
    struct flash_addr_t *old_vir_addr;

    pu = get_pu_from_lpn(lpn);

    lpn_in_pu = ((lpn/PU_NUM) & (~LPN_PER_BUF_MSK)) + (lpn % LPN_PER_BUF);

    pmtpage_in_pu = lpn_in_pu/LPN_CNT_PER_PMTPAGE;
    
    offset_in_mptpage = lpn_in_pu%LPN_CNT_PER_PMTPAGE;

    old_vir_addr = &pmt[pu]->page[pmtpage_in_pu].item[offset_in_mptpage].vir_flash_addr;

    table_update_rpmt(lpn, old_vir_addr, new_vir_addr);

    old_vir_addr->ppn = new_vir_addr->ppn;

    return 0;
}


/*====================End of this file========================================*/

