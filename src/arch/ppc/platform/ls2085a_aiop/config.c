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
__declspec(section ".aiop_init_data")   struct aiop_init_data  g_init_data;
#pragma pop

/* TODO set good default values
 * TODO Update and review structure */
struct aiop_init_data g_init_data =
{
 /* aiop_sl_init_info */
 {
  4,	/* aiop_rev_major     AIOP  */
  2,	/* aiop_rev_minor     AIOP  */
  0x6018000000,	/* dp_ddr_phys_addr      */
  0x4c00200000,	/* peb_phys_addr      */
  0,		/* sys_ddr1_phys_add  */
  0x40000000,	/* dp_ddr_virt_addr      */
  0x80000000,	/* peb_virt_addr      */
  0,	/* sys_ddr1_virt_addr */
  0,	/* ccsr_vaddr */
  0,	/* mc_portals_vaddr */
  2,	/* uart_port_id       MC */
  1,	/* mc_portal_id       MC */
  0,	/* mc_dpci_id         MC */
  1000,	/* clock_period       MC */
  {0}	/* reserved           */
 },
 /* aiop_app_init_info */
 {
  (128 * MEGABYTE),	/* dp_ddr_size */
  (2 * MEGABYTE),	/* peb_size */
  0,	/* ddr1_size */
  2048,			/* ctlu_sys_ddr_num_entries */
  2048,			/* ctlu_dp_ddr_num_entries */
  2048,			/* ctlu_peb_num_entries */
  2048,			/* mflu_sys_ddr_num_entries */
  2048,			/* mflu_dp_ddr_num_entries */
  2048,			/* mflu_peb_num_entries */
  0x100000,	/* sru_size */
  1000000,	/* tman_freq */
  4,	/* tasks_per_core */
  {0}	/* reserved */
 }
};



