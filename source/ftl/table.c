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
            vbt[pu]->item[block].phy_block_addr = 0xffff;
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
            pbt[pu]->item[block].virtual_block_addr = 0xffff;
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




/*====================End of this file========================================*/
