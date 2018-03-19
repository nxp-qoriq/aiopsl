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

/*
 * This file can't be merged into fsl_sys.h because fsl_system.h needs 
 * fsl_platform.h and fsl_platform.h needs fsl_sys.h
 */

#ifndef __FSL_SYSTEM_H
#define __FSL_SYSTEM_H

 #include "general.h"
 #include "fsl_aiop_common.h"
 #include "fsl_types.h"
 #include "fsl_errors.h"
 #include "fsl_list.h"
 #include "fsl_dbg.h"
 #include "fsl_smp.h"
 #include "fsl_platform.h"
 #include "fsl_malloc.h"
 #include "fsl_sys.h"
 #include "fsl_log.h"
 #include "fsl_mem_mng.h"


#pragma pack(push, 1)
typedef struct t_system {
	/* The lock must be aligned to a double word boundary.
	 * It's easy to control the alignment when it's the first member.
	 */
	uint64_t              barrier_lock;

	/* Memory management variables */
	/* It must be aligned to a double word boundary since
	 * the first member it's a lock */
	struct initial_mem_mng	    boot_mem_mng;
	struct t_mem_mng            mem_mng;

	/* Console variables */
	void *                console;
	int                         (*f_console_print)(void * console,
		uint8_t *p_data, uint32_t size);
	int                         (*f_console_get)(void * console,
		uint8_t *p_data, uint32_t size);
	char                        *p_pre_console_buf;
	uint32_t                    pre_console_buf_pos;
	uint8_t                     print_to_buffer;

	/* Multi-Processing variables */
	int                  is_tile_master[INTG_MAX_NUM_OF_CORES];
	int                  is_cluster_master[INTG_MAX_NUM_OF_CORES];
	uint64_t             active_cores_mask;
	uint32_t             num_of_active_cores;
	volatile uint64_t    barrier_mask;

	/* boot synchronization variables */
	volatile uint32_t           boot_sync_flag;
	volatile uint32_t           runtime_flag;

	/* Platform operations */
	t_platform_ops              platform_ops;
} t_system;
#pragma pack(pop)

#pragma warning_errors on
ASSERT_MULTIPLE_OF(offsetof(t_system, boot_mem_mng), 8);
#pragma warning_errors off

extern t_system sys;

inline void system_yield(void)
{
	extern t_system sys;

	if(sys.runtime_flag) {
		__e_hwacceli(YIELD_ACCEL_ID); /* Yield */
	}
}
#endif /* __FSL_SYSTEM_H */
