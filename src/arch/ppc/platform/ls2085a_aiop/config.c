#include "types.h"
#include "aiop_common.h"
#include "fsl_gen.h"
#include "apps.h"
#include "platform.h"
#include "fsl_dpni_drv.h"


/*********************************************************************/
/*
 * This section  is for dynamic aiop load
 */
/* Place MC <-> AIOP structures at fixed address.
 * Don't create new macro for section because no one else should use it */
#pragma push
#pragma force_active on
#pragma section  RW ".aiop_init_data" ".aiop_init_data_bss"
__declspec(section ".aiop_init_data")   struct aiop_init_info  g_init_data;
#pragma pop

/* This value should match AIOP_DDR_END - AIOP_DDR_START from aiop_link.lcf
 * This is the size that AIOP image occupies in DP_DDR.
 * The user SHOULDN'T edit edit this. */
#define AIOP_DP_DDR_SIZE 0xb10000

const uint32_t g_aiop_lcf_ddr_size = AIOP_DP_DDR_SIZE;



struct platform_app_params g_app_params =
{
	DPNI_NUMBER_BUFFERS_IN_POOL,
	DPNI_BUFFER_SIZE_IN_POOL,
	DPNI_BUFFER_ALIGNMENT,
	APP_MAX_NUM
};

/* TODO set good default values
 * TODO Update and review structure */
struct aiop_init_info g_init_data =
{
 /* aiop_sl_init_info */
 {
  2,		/* aiop_rev_major     AIOP  */
  0,		/* aiop_rev_minor     AIOP  */
  1,		/* revision           AIOP */
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
  0,		/* options       MC */
  0,		/* args_size       MC */
  {0},		/* args       MC */
  {0}	        /* reserved           */
 },
 /* aiop_app_init_info */
 {
    AIOP_SL_AND_APP_DDR_SIZE,
 	PEB_SIZE,
 	SYS_DDR1_SIZE,
 	CTLU_SYS_DDR_NUM_ENTRIES,
 	CTLU_DP_DDR_NUM_ENTRIES,
 	CTLU_PEB_NUM_ENTRIES,
 	MFLU_SYS_DDR_NUM_ENTRIES,
 	MFLU_DP_DDR_NUM_ENTRIES,
 	MFLU_PEB_NUM_ENTRIES,
 	SRU_SIZE,
 	AIOP_TASKS_PER_CORE,
 	SOC_MAX_NUM_OF_DPNI, /*SPID_COUNT*/
 	{0}/* reserved */
 }

};

