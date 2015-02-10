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

/**************************************************************************//**
@File		system.c

@Description	This file contains the AIOP SW system interface.

*//***************************************************************************/
#include "system.h"
#include "id_pool.h"
#include "fsl_cdma.h"
#include "fsl_platform.h"

#include "fsl_ipsec.h"
#include "fsl_keygen.h"
#include "ipsec.h"
#include "fsl_sys.h"
#include "time.h"
#include "aiop_common.h"
/* #include "fsl_sl_dbg.h" */
#include "fsl_slab.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"

extern void tman_timer_callback(void);
/* The offset of the Work Scheduler registers */
#define AIOP_WRKS_REGISTERS_OFFSET				0x0209d000

#else
#include "slab.h"
#include "platform.h"

#include "fsl_dbg.h"
#include "fsl_io.h"
#include "aiop_common.h"
#include "ipr.h"

extern void tman_timer_callback(void);
extern int ipr_init(void);
#ifndef CDC_ROC
extern int aiop_snic_init(void);
extern void aiop_snic_free(void);
#endif

#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,                 \
						0,                          \
						E_MAPPED_MEM_TYPE_GEN_REGS) \
						+ SOC_PERIPH_OFF_AIOP_WRKS)

#endif /* AIOP_VERIF */

/* Global parameters*/
uint64_t ext_prpid_pool_address;
uint64_t ext_keyid_pool_address;
uint16_t bpid_prpid;
uint16_t bpid_keyid;
extern struct  ipr_global_parameters ipr_global_parameters1;

/* Time module globals */
extern struct aiop_cmgw_regs *time_cmgw_regs;
extern _time_get_t *time_get_func_ptr;
/* Storage profiles array */
__PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];

/* Global Parameters for TKT226361 (MFLU) WA - TODO remove for non Rev1 */
#include "table_inline.h"
#ifndef AIOP_VERIF
extern struct aiop_init_info g_init_data;
#endif

void sys_prpid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;


	status = slab_find_and_reserve_bpid(1, (SYS_NUM_OF_PRPIDS+3), 2,
			MEM_PART_DP_DDR,
			NULL, &buffer_pool_id);
	if (status < 0)
		system_init_exception_handler(SYS_PRPID_POOL_CREATE,
			__LINE__,
			SYSTEM_INIT_SLAB_FAILURE);

	id_pool_init(SYS_NUM_OF_PRPIDS, buffer_pool_id,
					&ext_prpid_pool_address);
}


void sys_keyid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;


	status = slab_find_and_reserve_bpid(1, (SYS_NUM_OF_KEYIDS+3), 2,
			MEM_PART_DP_DDR,
			NULL, &buffer_pool_id);
	if (status < 0)
		system_init_exception_handler(SYS_KEYID_POOL_CREATE,
			__LINE__,
			SYSTEM_INIT_SLAB_FAILURE);

	id_pool_init(SYS_NUM_OF_KEYIDS, buffer_pool_id,
					&ext_keyid_pool_address);
}

#ifndef AIOP_VERIF
__COLD_CODE int aiop_sl_early_init(void){
	int err = 0;
	/* (SYS_NUM_OF_PRPIDS+3) rounded up modulu 64 - 8 */
	err |= slab_register_context_buffer_requirements(1, 1, 120,
			2, MEM_PART_DP_DDR, 0, 0);
	/* (SYS_NUM_OF_KEYIDS+3) rounded up modulu 64 - 8 */
	err |= slab_register_context_buffer_requirements(1, 1, 312,
			2, MEM_PART_DP_DDR, 0, 0);
	/* The following buffers requests registrations are temporary till new 
	 * abstraction API will be valid for IPR and IPSEC. */
	/* IPR IPsec 2688 rounded up modulu 64 - 8 */
	err |= slab_register_context_buffer_requirements(750, 750, 2744, 64,
			MEM_PART_DP_DDR, 0, 0);
	/* IPsec 512 rounded up modulu 64 - 8 */
	err |= slab_register_context_buffer_requirements(750, 750, 568, 64,
			MEM_PART_DP_DDR, 0, 0);

	if(err){
		pr_err("Failed to register context buffers\n");
		return err;
	}
	return 0;
}
#endif

int aiop_sl_init(void)
{
	int32_t status = 0;
	uint16_t icid;

#ifdef AIOP_VERIF
	/* TMAN EPID Init params*/
	uint32_t val;
	uint32_t *addr;
	struct aiop_tile_regs *aiop_regs =
			(struct aiop_tile_regs *)
			sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
#endif


	/* initialize profile sram */

	/* TEMP FIX: MC-ARENA DO NOT COMMUNICATE */
#ifndef AIOP_VERIF /* No DPNI */
	/* Storage Profile 0 - Default Storage Profile */
	icid  = 0x0100;
	//storage_profile[0].ip_secific_sp_info = 0;
	storage_profile[0].ip_secific_sp_info = ((uint64_t)icid) << 48;
	storage_profile[0].dl = 0;
	storage_profile[0].reserved = 0;
	/* 0x0080 --> 0x8000 (little endian) */
	storage_profile[0].dhr = 0x8000;
	/*storage_profile[0].dhr = 0x0080; */
	storage_profile[0].mode_bits1 = (sp0_mode_bits1_PTAR | sp0_mode_bits1_SGHR |
			sp0_mode_bits1_ASAR);
	storage_profile[0].mode_bits2 = (sp0_mode_bits2_BS | sp0_mode_bits2_FF |
			sp0_mode_bits2_VA | sp0_mode_bits2_DLC);
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	 * 0x0801 --> 0x0108 (little endian) */
	storage_profile[0].pbs1 = 0x0108;
	/* BPID=0 */
	storage_profile[0].bpid1 = 0x0000;
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	* 0x0801 --> 0x0108 (little endian) */
	storage_profile[0].pbs2 = 0x0108;
	/* BPID=0 */
	storage_profile[0].bpid2 = 0x0000;
	storage_profile[0].pbs3 = 0x0000;
	storage_profile[0].bpid3 = 0x0000;
	storage_profile[0].pbs4 = 0x0000;
	storage_profile[0].bpid4 = 0x0000;
	
	/* Storage Profile 1 */
	//storage_profile[1].ip_secific_sp_info = 0;
	storage_profile[1].ip_secific_sp_info = ((uint64_t)icid) << 48;
	storage_profile[1].dl = 0;
	storage_profile[1].reserved = 0;
	/* 0x0080 --> 0x8000 (little endian) */
	storage_profile[1].dhr = 0x8000;
	/*storage_profile[1].dhr = 0x0080; */
	storage_profile[1].mode_bits1 = (sp1_mode_bits1_PTAR | sp1_mode_bits1_SGHR |
			sp1_mode_bits1_ASAR);
	storage_profile[1].mode_bits2 = (sp1_mode_bits2_BS | sp1_mode_bits2_FF |
			sp1_mode_bits2_VA | sp1_mode_bits2_DLC);
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	* 0x0801 --> 0x0108 (little endian) */
	storage_profile[1].pbs1 = 0x0108;
	/* BPID=0 */
	storage_profile[1].bpid1 = 0x0000;
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	* 0x0801 --> 0x0108 (little endian) */
	storage_profile[1].pbs2 = 0x0108;
	/* BPID=0 */
	storage_profile[1].bpid2 = 0x0000;
	storage_profile[1].pbs3 = 0x0000;
	storage_profile[1].bpid3 = 0x0000;
	storage_profile[1].pbs4 = 0x0000;
	storage_profile[1].bpid4 = 0x0000;

#endif
	
#ifdef AIOP_VERIF
	time_cmgw_regs = (struct aiop_cmgw_regs*) &(aiop_regs->cmgw_regs);
	time_get_func_ptr = _get_time_fast;
#endif




/* TODO - remove the AIOP_VERIF section when verification env will include
 * the ARENA code */
#ifdef AIOP_VERIF
	/* TMAN EPID Init */

	val = 1; /* EPID = 1 */
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0xF8);
	*addr = val;

	val = (uint32_t)&tman_timer_callback; /* EP_PC */
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0x100);
	*addr = val;

	val = 0x00600040; /* EP_FDPA */
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0x108);
	*addr = val;
	val = 0x02000000; /*SET NDS bit*/
	addr = (uint32_t *)(AIOP_WRKS_REGISTERS_OFFSET + 0x118);
	*addr = val;
	/* End of TMAN EPID Init */
#endif


/********************************************************/
/* WA TKT226361 STRAT (MFLU) - TODO remove for non Rev1 */
/********************************************************/
#ifdef AIOP_VERIF
	table_workaround_tkt226361(4,4,4);
#else

	table_workaround_tkt226361(g_init_data.app_info.mflu_peb_num_entries,
				   g_init_data.app_info.
					mflu_dp_ddr_num_entries,
				   g_init_data.app_info.
					mflu_sys_ddr_num_entries);
#endif
/********************************************************/
/* WA TKT226361 END (MFLU) - TODO remove for non Rev1 */
/********************************************************/

	sys_prpid_pool_create();

#ifdef AIOP_VERIF
	sys_keyid_pool_create();

#else
	sys_keyid_pool_create();
#ifndef NO_DP_DDR
	status = ipr_init();
#ifndef CDC_ROC
	if (status)
		return status; /* TODO */

#ifdef MC_PORTAL_FIX
	status = aiop_snic_init();	
#endif
	return status;
#endif /*MC_PORTAL_FIX*/
#endif	
#endif
	return 0;
}

void aiop_sl_free(void)
{
/*	int32_t status = 0;*/

	cdma_release_context_memory(ext_prpid_pool_address);
	cdma_release_context_memory(ext_keyid_pool_address);
#ifndef AIOP_VERIF
	/*need to un-reserve bpid for kegen-id and prpid */
	slab_find_and_unreserve_bpid(1, bpid_prpid);
	slab_find_and_unreserve_bpid(1, bpid_keyid);
	/*need to undo to ipr_init */
	keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU,
			ipr_global_parameters1.ipr_key_id_ipv4);
	keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU,
			ipr_global_parameters1.ipr_key_id_ipv6);
#ifndef NO_DP_DDR
#ifndef CDC_ROC
	aiop_snic_free();
#endif	
#endif
#endif

	/* TODO status ? */
}


#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

void system_init_exception_handler(enum system_function_identifier func_id,
		     uint32_t line,
		     int32_t status)
{
	char *func_name;
	char *err_msg;

	/* Translate function ID to function name string */
	switch(func_id) {
	case SYS_PRPID_POOL_CREATE:
		func_name = "sys_prpid_pool_create";
		break;
	case SYS_KEYID_POOL_CREATE:
		func_name = "sys_keyid_pool_create";
		break;
	default:
		/* create own exception */
		func_name = "Unknown Function";
	}

	/* Translate error ID to error name string */
	if (status == SYSTEM_INIT_SLAB_FAILURE) {
		err_msg = "Pool Creation failed due to slab failure.\n";
	} else {
		err_msg = "Unknown or Invalid status.\n";
	}

	exception_handler(__FILE__, func_name, line, err_msg);
}

#pragma pop

