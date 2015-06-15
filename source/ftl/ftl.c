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

U32 flt_init(void)
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

U32 ftl_write(const struct ftl_req_t *write_request)
{
    U32 lpn;
    U32 pu;
    U32 i;
    struct flash_addr_t vir_addr;
	struct flash_addr_t phy_addr;
	struct flash_req_t  flash_req;

    assert_null_pointer(write_request);

    lpn = write_request->lpn_list[0];
    
    pu = get_pu_from_lpn(lpn);

    vir_addr = flash_alloc_page(pu);

    phy_addr = get_phy_flash_addr(vir_addr);

    for (i = 0; i < write_request->lpn_count; i++)
    {
        lpn = write_request->lpn_list[i];
        vir_addr.lpn_in_page = i;
        table_update_pmt(lpn, &vir_addr);
    }

    flash_req.data_buffer_addr = write_request->buffer_addr;
    flash_req.spare_buffer_addr = 0; //to be continue

    flash_req.data_length = write_request->lpn_count * LPN_SIZE;
    flash_req.data_offset = 0;
    
    return flash_write(&phy_addr, &flash_req);
}


U32 ftl_read(const struct ftl_req_t *read_request)
{
    return 0;
}


/*====================End of this file========================================*/

