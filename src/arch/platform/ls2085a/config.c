/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "fsl_types.h"
#include "fsl_aiop_common.h"
#include "fsl_gen.h"
#include "apps.h"
#include "fsl_platform.h"

/*
 * AIOP SL users should not modify this file.
 * AIOP SL users should set the values at apps.h
 */

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
  6,		/* aiop_rev_major     AIOP  */
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
  (1 * MEGABYTE), /* FDMA sru size*/
  APP_INIT_TASKS_PER_CORE,
  APP_DPNI_SPID_COUNT, /*SPID_COUNT*/
  {0}/* reserved */
 }

};

