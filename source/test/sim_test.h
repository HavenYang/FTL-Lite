/*
* File Name    : env_sim.h
* Discription  : 
* CreateAuthor : Haven Yang
* CreateDate   : 2015.6.11
*===============================================================================
* Modify Record:
*=============================================================================*/
#ifndef _ENV_SIM_H
#define _ENV_SIM_H

/*============================================================================*/
/* #include region: include std lib & other head file                         */
/*============================================================================*/

/*============================================================================*/
/* #define region: constant & MACRO defined here                              */
/*============================================================================*/
#define SIM_SUCCESS 0
#define SIM_FAIL    1

/*============================================================================*/
/* #typedef region: global data structure & data type typedefed here          */
/*============================================================================*/
struct big_data_t
{
    U32 low;
    U32 mid;
    U32 high;
};

struct top_data_t
{
    struct big_data_t user_write_lpn_count;
    struct big_data_t flash_write_lpn_count;
    U32 wa_x_100;
    U32 user_block;
    U32 ftl_rsv_block;
    U32 disk_size_m;
    U32 max_erase_count;
    U32 min_erase_count;
    U32 avr_erase_count;
    U32 gc_count;
    U32 max_dirty_count;
    U32 min_dirty_count;
    U32 avr_dirty_count;
};

/*============================================================================*/
/* function declaration region: declare global function prototype             */
/*============================================================================*/
void sim_test_init(void);
void run_test_cases(void);
void show_top_data(void);

void sim_calc_user_write(U32 lpn_count);
void sim_calc_flash_write(U32 lpn_count);


#endif
/*====================End of this head file===================================*/

