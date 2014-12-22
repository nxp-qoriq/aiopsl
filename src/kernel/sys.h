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

#ifndef __SYS_H
#define __SYS_H

#include "general.h"
#include "aiop_common.h"
#include "common/types.h"
#include "fsl_errors.h"
#include "fsl_list.h"
#include "fsl_dbg.h"
#include "fsl_smp.h"
#include "platform.h"
#include "ls2085_aiop/fsl_platform.h"
#include "inc/fsl_sys.h"
#include "mem_mng.h"

#define PRE_CONSOLE_BUF_SIZE    (4 * 1024)

#define __ERR_MODULE__  MODULE_UNKNOWN

#define SYS_TILE_MASTERS_MASK (0x00000001)
#define SYS_CLUSTER_MASTER_MASK (0x00001111)
#define SYS_BOOT_SYNC_FLAG_DONE (1)

typedef struct t_system {
	/* Memory management variables */
	struct initial_mem_mng	    boot_mem_mng;
	fsl_handle_t                mem_mng;
	int                         heap_partition_id;
	uintptr_t                   heap_addr;
	uint8_t                     mem_part_mng_lock;
	uint8_t                     mem_mng_lock;

	/* Console variables */
	fsl_handle_t                console;
	int                         (*f_console_print)(fsl_handle_t console,
		uint8_t *p_data, uint32_t size);
	int                         (*f_console_get)(fsl_handle_t console,
		uint8_t *p_data, uint32_t size);
	char                        *p_pre_console_buf;
	uint32_t                    pre_console_buf_pos;
	uint8_t                     console_lock;

	/* Multi-Processing variables */
	int                  is_tile_master[INTG_MAX_NUM_OF_CORES];
	int                  is_cluster_master[INTG_MAX_NUM_OF_CORES];
	uint64_t             active_cores_mask;
	uint32_t             num_of_active_cores;
	uint8_t              barrier_lock;
	volatile uint64_t    barrier_mask;

	/* boot synchronization variables */
	volatile uint32_t           boot_sync_flag;
	volatile uint32_t           runtime_flag;

	/* Platform operations */
	t_platform_ops              platform_ops;
} t_system;

extern t_system sys;

void fill_platform_parameters(struct platform_param *platform_param);

/* Internal system routines */
int     sys_init_memory_management(void);
int     sys_free_memory_management(void);
int     sys_init_multi_processing(void);
void    sys_free_multi_processing(void);

void    sys_register_debugger_console(void);

inline void	sys_yield(void)
{
	extern t_system sys;
	
	if(!sys.runtime_flag) {
		__e_hwacceli(YIELD_ACCEL_ID); /* Yield */
	}
}

#endif /* __SYS_H */
