/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
#include "ipsec.h"
#include "fsl_sys.h"
#include "time.h"
#include "aiop_common.h"

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

extern void tman_timer_callback(void);
extern int ipr_init(void);
extern int aiop_snic_init(void);

#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,                 \
						0,                          \
						E_MAPPED_MEM_TYPE_GEN_REGS) \
						+ SOC_PERIPH_OFF_AIOP_WRKS)

#endif /* AIOP_VERIF */

/* Global parameters*/
__SHRAM uint64_t ext_prpid_pool_address;
__SHRAM uint64_t ext_keyid_pool_address;


/* Time module globals */
extern __SHRAM struct aiop_cmgw_regs *time_cmgw_regs;
extern __SHRAM _time_get_t *time_get_func_ptr;
/* Storage profiles array */
__PROFILE_SRAM struct storage_profile storage_profile;

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

int aiop_sl_init(void)
{
	int32_t status = 0;

#ifdef AIOP_VERIF
	/* TMAN EPID Init params*/
	uint32_t val;
	uint32_t *addr;
	struct aiop_tile_regs *aiop_regs =
			(struct aiop_tile_regs *)
			sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
#endif


	/* initialize profile sram */

#ifdef AIOP_VERIF
	/* Default Storage Profile */
	storage_profile.ip_secific_sp_info = 0;
	storage_profile.dl = 0;
	storage_profile.reserved = 0;
	/* 0x0080 --> 0x8000 (little endian) */
	storage_profile.dhr = 0x8000;
	/*storage_profile.dhr = 0x0080; */
	storage_profile.mode_bits1 = (mode_bits1_PTAR | mode_bits1_SGHR |
			mode_bits1_ASAR);
	storage_profile.mode_bits2 = (mode_bits2_BS | mode_bits2_FF |
			mode_bits2_VA | mode_bits2_DLC);
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	 * 0x0801 --> 0x0108 (little endian) */
	storage_profile.pbs1 = 0x0108;
	/* BPID=0 */
	storage_profile.bpid1 = 0x0000;
	/* buffer size is 2048 bytes, so PBS should be 32 (0x20).
	* 0x0801 --> 0x0108 (little endian) */
	storage_profile.pbs2 = 0x0108;
	/* BPID=0 */
	storage_profile.bpid2 = 0x0000;
	storage_profile.pbs3 = 0x0000;
	storage_profile.bpid3 = 0x0000;
	storage_profile.pbs4 = 0x0000;
	storage_profile.bpid4 = 0x0000;

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


	sys_prpid_pool_create();

#ifdef AIOP_VERIF
	sys_keyid_pool_create();
#else
	sys_keyid_pool_create();

	status = ipr_init();
	if (status)
		return status; /* TODO */

	status = aiop_snic_init();
	return status;
#endif
	return 0;
}

void aiop_sl_free(void)
{
/*	int32_t status = 0;*/

	cdma_release_context_memory(ext_prpid_pool_address);
	cdma_release_context_memory(ext_keyid_pool_address);

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

