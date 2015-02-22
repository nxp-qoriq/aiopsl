#include "types.h"
#include "aiop_common.h"
#include "fsl_gen.h"
#include "apps.h"
#include "platform.h"
#include "fsl_dpni_drv.h"

/*
 * AIOP SL users should not modify this file.
 * AIOP SL users should set the values at apps.h
 */

#define SRU_SIZE			(1 * MEGABYTE)	 /**< FDMA sru size*/

/* .aiop_init_data section  is for dynamic aiop load.
 * Place MC <-> AIOP structures at fixed address.
 * Don't create new macro for section because no one else should use it */
#pragma push
#pragma force_active on
#pragma section  RW ".aiop_init_data" ".aiop_init_data_bss"
__declspec(section ".aiop_init_data")   struct aiop_init_info  g_init_data;
#pragma pop


struct platform_app_params g_app_params =
{
 APP_DPNI_NUM_BUFS_IN_POOL,
 APP_DPNI_BUF_SIZE_IN_POOL,
 APP_DPNI_BUF_ALIGN_IN_POOL,
 APP_INIT_APP_MAX_NUM
};

/* TODO set good default values */
struct aiop_init_info g_init_data =
{
 /* aiop_sl_init_info */
 {
  2,		/* aiop_rev_major     AIOP  */
  0,		/* aiop_rev_minor     AIOP  */
  0,		/* revision           AIOP */
  0,            /* base_spid MC */
  0x6000000000,	/* dp_ddr_phys_addr      */
  0x40000000,	/* dp_ddr_virt_addr      */
  0x4c00000000,	/* peb_phys_addr      */
  0x80000000,	/* peb_virt_addr      */
  0,		/* sys_ddr1_phys_add  */
  0,	        /* sys_ddr1_virt_addr */
  0x08000000,	/* ccsr_paddr */
  0x10000000,	/* ccsr_vaddr */
  0x80c000000,	/* mc_portals_paddr */
  0x0c000000,	/* mc_portals_vaddr */
  2,	        /* uart_port_id       MC */
  1,	        /* mc_portal_id       MC */
  0,	        /* mc_dpci_id         MC */
  1000,	        /* clock_period       MC */
  0,		/* physical address of log buffer */
  0,		/* size of log buffer */
  0,		/* system clock in KHz */
  0,		/* options       MC */
  0,		/* args_size       MC */
  {0},		/* args       MC */
  {0}	        /* reserved           */
 },
 /* aiop_app_init_info */
 {	
  APP_MEM_DP_DDR_SIZE,
  APP_MEM_PEB_SIZE,
  APP_MEM_SYS_DDR1_SIZE,
  APP_CTLU_SYS_DDR_NUM_ENTRIES,
  APP_CTLU_DP_DDR_NUM_ENTRIES,
  APP_CTLU_PEB_NUM_ENTRIES,
  APP_MFLU_SYS_DDR_NUM_ENTRIES,
  APP_MFLU_DP_DDR_NUM_ENTRIES,
  APP_MFLU_PEB_NUM_ENTRIES,
  SRU_SIZE,
  APP_INIT_TASKS_PER_CORE,
  SOC_MAX_NUM_OF_DPNI, /*SPID_COUNT*/
  {0}/* reserved */
 }

};

