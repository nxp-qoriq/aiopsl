/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		system.c

@Description	This file contains the AIOP SW system interface.

*//***************************************************************************/
#include "system.h"
#include "fsl_id_pool.h"
#include "fsl_cdma.h"

#include "fsl_ipsec.h"
#include "fsl_keygen.h"
#include "ipsec.h"
#include "fsl_sys.h"
#include "time.h"
#include "fsl_aiop_common.h"
/* #include "fsl_sl_dbg.h" */
#include "fsl_slab.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"

extern void tman_timer_callback(void);
/* The offset of the Work Scheduler registers */
#define AIOP_WRKS_REGISTERS_OFFSET				0x0209d000

#else
#include "fsl_sl_slab.h"
#include "fsl_platform.h"

#include "fsl_dbg.h"
#include "fsl_io.h"
#include "fsl_aiop_common.h"
#include "ipr.h"
#include "cwapr.h"

extern void tman_timer_callback(void);
extern int ipr_init(void);
extern int cwapr_init(void);

#ifdef ENABLE_SNIC
extern int aiop_snic_init(void);
extern int aiop_snic_early_init(void);
extern void aiop_snic_free(void);
#endif	/* ENABLE_SNIC */

#endif /* AIOP_VERIF */

/* Global parameters*/
uint64_t ext_prpid_pool_address;
uint64_t ext_keyid_pool_address;
uint16_t bpid_prpid;
uint16_t bpid_keyid;

#ifndef USE_IPR_SW_TABLE
extern struct  ipr_global_parameters ipr_global_parameters1;
#endif	/* USE_IPR_SW_TABLE */
extern struct  cwapr_global_parameters g_cwapr_params;

/* Time module globals */
extern struct aiop_cmgw_regs *time_cmgw_regs;
/* Storage profiles array */
__PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];

/* Global Parameters for TKT226361 (MFLU) WA - TODO remove for non Rev1 */
#include "fsl_table.h"
#ifndef AIOP_VERIF
extern struct aiop_init_info g_init_data;
#endif

void sys_prpid_pool_create(void)
{
	int32_t status;
	uint16_t buffer_pool_id;
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	status = slab_find_and_reserve_bpid(1, (SYS_NUM_OF_PRPIDS+3), 2,
			mem_pid,
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
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	status = slab_find_and_reserve_bpid(1, (SYS_NUM_OF_KEYIDS+3), 2,
			mem_pid,
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
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	/* (SYS_NUM_OF_PRPIDS+3) rounded up modulu 64 - 8 */
	err |= slab_register_context_buffer_requirements(1, 1, 120,
			2, mem_pid, 0, 0);
	/* (SYS_NUM_OF_KEYIDS+3) rounded up modulu 64 - 8 */
	err |= slab_register_context_buffer_requirements(1, 1, 312,
			2, mem_pid, 0, 0);

#ifdef ENABLE_SNIC
	err |= aiop_snic_early_init();
#endif	/* ENABLE_SNIC */

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

#ifdef AIOP_VERIF
	/* TMAN EPID Init params*/
	uint32_t val;
	uint32_t *addr;
	struct aiop_tile_regs *aiop_regs =
			(struct aiop_tile_regs *)
			sys_get_handle(FSL_MOD_AIOP_TILE, 1);
#endif


	/* initialize profile sram */

#ifdef AIOP_VERIF
	/* Storage Profile 0 - Default Storage Profile */
	storage_profile[0].ip_secific_sp_info = 0;
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

	/* Storage Profile 1 - Reuse buffer mode */
	storage_profile[1].ip_secific_sp_info = 0;
	storage_profile[1].dl = 0;
	storage_profile[1].reserved = 0;

	/* In reuse buffer mode (BS=1) the DHR field is treated
	 * as a signed value of a data headroom correction and defines by
	 * how many bytes an existing offset should be adjusted to make room
	 * for additional output data or any need to move the output �forward� */
	/* In this case, DHR is set to 0,
	 * to preserve the offset of the input frame */
	storage_profile[1].dhr = 0;

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

	status = ipr_init();
	if (status)
		return status; /* TODO */

	status = cwapr_init();
	if (status)
		return status;

#ifdef ENABLE_SNIC
	status = aiop_snic_init();
#endif	/* ENABLE_SNIC */

	return status;
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

#ifndef USE_IPR_SW_TABLE
	/*need to undo to ipr_init */
	keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU,
			ipr_global_parameters1.ipr_key_id_ipv4);
	keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU,
			ipr_global_parameters1.ipr_key_id_ipv6);
#endif	/* USE_IPR_SW_TABLE */

	keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU, g_cwapr_params.cwapr_key_id);
#ifdef ENABLE_SNIC
	aiop_snic_free();
#endif	/* ENABLE_SNIC */
#endif

	/* TODO status ? */
}


#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

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

