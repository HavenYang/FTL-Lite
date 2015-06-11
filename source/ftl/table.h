/*
* File Name    : table.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _TABLE_H
#define _TABLE_H

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/

/* page management table item */
struct pmt_item_t
{
    struct phy_flash_addr_t pfa;
};

/* pmt page */
struct pmt_page_t
{
    struct pmt_item_t item[LPN_CNT_PER_PMTPAGE];
};

struct pmt_t
{
    struct pmt_page_t page[PMT_PAGE_CNT];
};


/* virtual block table : point to physical block information */
struct vbt_t
{
    
};


/* physical block table : point to virtual block information */
struct pbt_t
{
    
};

/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/

/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/


#endif
/*====================End of this head file===================================*/

