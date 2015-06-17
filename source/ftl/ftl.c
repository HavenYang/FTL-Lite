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
LOCAL struct ftl_req_t unfull_write_req;

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



void other_init(void)
{
    //unfull_write_req.buffer_addr = 
}

U32 ftl_llf(void)
{
    table_llf_bbt();
    table_llf_vbt();
    table_llf_pbt();
    table_llf_pmt();
    return 0;
}

U32 ftl_init(void)
{
    ftl_llf();
    init_pu_info();
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


U32 ftl_write(const struct ftl_req_t *write_request)
{
    U32 lpn;
    U32 pu;
    U32 i;
    struct flash_addr_t vir_addr;
	struct flash_addr_t phy_addr;
	struct flash_req_t  flash_write_req;

    assert_null_pointer(write_request);

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

    flash_write_req.data_length = write_request->lpn_count * LPN_SIZE;
    
    return flash_write(&phy_addr, &flash_write_req);
}


U32 ftl_read(const struct ftl_req_t *read_request)
{
    U32 lpn;
    struct flash_addr_t vir_addr;
	struct flash_addr_t phy_addr;
	struct flash_req_t  flash_read_req;

    assert_null_pointer(read_request);

    lpn = read_request->lpn_list[0];
    
    table_lookup_mpt(lpn, &vir_addr);

    if (addr_valid(&vir_addr))
    {
        vir_to_phy_addr(&vir_addr, &phy_addr);

        flash_read_req.data_buffer_addr = read_request->buffer_addr;
        flash_read_req.spare_buffer_addr = 0; //to be continue

        flash_read_req.data_length = read_request->lpn_count * LPN_SIZE;
        
        return flash_read(&phy_addr, &flash_read_req);
    }
    else
    {
        printf("read without write!\n");
        // to be continue;
        return SUCCESS;
    }
}


/*====================End of this file========================================*/

