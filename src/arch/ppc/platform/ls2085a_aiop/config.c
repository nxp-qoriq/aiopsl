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

/** 
 * Keeping backward compatibility
 */
#ifndef SRU_SIZE
#define SRU_SIZE			(1 * MEGABYTE)	 /**< FDMA sru size*/
#else
#warning "SRU_SIZE has been deprecated delete, it from apps.h" 
#endif

#ifndef APP_DPNI_NUM_BUFS_IN_POOL
#define APP_DPNI_NUM_BUFS_IN_POOL DPNI_NUMBER_BUFFERS_IN_POOL
#warning "DPNI_NUMBER_BUFFERS_IN_POOL has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_DPNI_BUF_SIZE_IN_POOL
#define APP_DPNI_BUF_SIZE_IN_POOL DPNI_BUFFER_SIZE_IN_POOL
#warning "DPNI_BUFFER_SIZE_IN_POOL has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_DPNI_BUF_ALIGN_IN_POOL
#define APP_DPNI_BUF_ALIGN_IN_POOL DPNI_BUFFER_ALIGNMENT
#warning "DPNI_BUFFER_ALIGNMENT has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_INIT_APP_MAX_NUM
#define APP_INIT_APP_MAX_NUM APP_MAX_NUM
#warning "APP_MAX_NUM has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_INIT_TASKS_PER_CORE
#define APP_INIT_TASKS_PER_CORE AIOP_TASKS_PER_CORE
#warning "AIOP_TASKS_PER_CORE has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_MEM_DP_DDR_SIZE
const uint32_t g_aiop_lcf_ddr_size = 0xb10000;
#define APP_MEM_DP_DDR_SIZE AIOP_SL_AND_APP_DDR_SIZE
#warning "AIOP_SL_AND_APP_DDR_SIZE has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_MEM_PEB_SIZE
#define APP_MEM_PEB_SIZE PEB_SIZE
#warning "PEB_SIZE has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_MEM_SYS_DDR1_SIZE
#define APP_MEM_SYS_DDR1_SIZE SYS_DDR1_SIZE
#warning "PEB_SIZE has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_CTLU_SYS_DDR_NUM_ENTRIES
#define APP_CTLU_SYS_DDR_NUM_ENTRIES CTLU_SYS_DDR_NUM_ENTRIES
#warning "CTLU_SYS_DDR_NUM_ENTRIES has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_CTLU_DP_DDR_NUM_ENTRIES
#define APP_CTLU_DP_DDR_NUM_ENTRIES CTLU_DP_DDR_NUM_ENTRIES
#warning "CTLU_DP_DDR_NUM_ENTRIES has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_CTLU_PEB_NUM_ENTRIES
#define APP_CTLU_PEB_NUM_ENTRIES CTLU_PEB_NUM_ENTRIES
#warning "CTLU_PEB_NUM_ENTRIES has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_MFLU_SYS_DDR_NUM_ENTRIES
#define APP_MFLU_SYS_DDR_NUM_ENTRIES MFLU_SYS_DDR_NUM_ENTRIES
#warning "MFLU_SYS_DDR_NUM_ENTRIES has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_MFLU_DP_DDR_NUM_ENTRIES
#define APP_MFLU_DP_DDR_NUM_ENTRIES MFLU_DP_DDR_NUM_ENTRIES
#warning "MFLU_DP_DDR_NUM_ENTRIES has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_MFLU_PEB_NUM_ENTRIES
#define APP_MFLU_PEB_NUM_ENTRIES MFLU_PEB_NUM_ENTRIES
#warning "MFLU_PEB_NUM_ENTRIES has been deprecated, copy apps.h from app_process_packet" 
#endif

#ifndef APP_DPNI_SPID_COUNT
#define APP_DPNI_SPID_COUNT 8
#warning "Add APP_DPNI_SPID_COUNT to apps.h, see apps.h of app_process_packet" 
#endif

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
 APP_INIT_APP_MAX_NUM,
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
  APP_DPNI_SPID_COUNT, /*SPID_COUNT*/
  {0}/* reserved */
 }

};

