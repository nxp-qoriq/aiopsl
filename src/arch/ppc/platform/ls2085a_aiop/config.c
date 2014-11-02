#include "types.h"
#include "aiop_common.h"
#include "fsl_gen.h"


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

/* This is an application required DP_DDR memory, user SHOULD edit this.
 * In this example the total dp_ddr memory is 128 MB , 
 * 0xb10000 out of it is occupied by aiop image dp_ddr and the rest is dedicated 
 * for application */
#define APPLICATION_DP_DDR_SIZE ((128 * MEGABYTE) - AIOP_DP_DDR_SIZE)

/* TODO set good default values
 * TODO Update and review structure */

struct aiop_init_info g_init_data =
{
 /* aiop_sl_init_info */
 {
  0,		/* aiop_rev_major     AIOP  */
  4,		/* aiop_rev_minor     AIOP  */
  6,		/* revision           AIOP */
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
  {0}	        /* reserved           */
 },
 /* aiop_app_init_info */
 {
  /* dp_ddr_size.
   * It is the  sum of AIOP image DDR size and 
   * application required DP_DDR memory.
   * The value should be aligned to a power of 2 */
  AIOP_DP_DDR_SIZE + APPLICATION_DP_DDR_SIZE,	
  
  /* peb_size.
   * Should be a power of 2.
   * Applications cannot require more that this maximum size */
  (512 * KILOBYTE),
  /* sys_ddr1_size = 0. Currently no dynamic allocation from system ddr */
  0,	
  2048,			/* ctlu_sys_ddr_num_entries */
  2048,			/* ctlu_dp_ddr_num_entries */
  2048,			/* ctlu_peb_num_entries */
  2048,			/* mflu_sys_ddr_num_entries */
  2048,			/* mflu_dp_ddr_num_entries */
  2048,			/* mflu_peb_num_entries */
  0x100000,	/* sru_size */
  800,	/* tman_freq */
  4,	/* tasks_per_core */
  {0}	/* reserved */
 }
};



