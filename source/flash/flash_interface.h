/*
* File Name    : flash_interface.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _FLASH_INTERFACE_H
#define _FLASH_INTERFACE_H

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/
#include "basedefine.h"

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

#define BLK_PER_PLN_BITS          10
#define BLK_PER_PLN               (1 << BLK_PER_PLN_BITS)
#define BLK_PER_PLN_MSK           (BLK_PER_PLN - 1)

#define PG_PER_BLK_BITS           9
#define PG_PER_BLK                (1 << PG_PER_BLK_BITS)    
#define PG_PER_BLK_MSK            (PG_PER_BLK - 1)



/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/


struct flash_addr_t
{
    union
    {
        U32 ppn;
        struct
        {
            U32 pu_index : 8;
            U32 block_in_pu : 10;
            U32 page_in_block : 9;
            U32 lpn_in_page : 5;
        };
    };
};

struct flash_req_t
{
    U32 data_buffer_addr;
    U32 spare_buffer_addr;
    U32 data_length;
    U32 data_offset;
};

/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/

U32 flash_write(struct flash_addr_t *phy_addr, struct flash_req_t *flash_req);


#endif
/*====================End of this head file===================================*/

