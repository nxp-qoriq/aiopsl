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

#include "general.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "common/fsl_stdarg.h"
#include "fsl_malloc.h"
#include "kernel/fsl_spinlock.h"
#include "platform.h"
#include "fsl_smp.h"
#include "fsl_io_ccsr.h"
#include "cmgw.h"

#include "sys.h"
#include "fsl_dbg.h"

/* Global System Object */
t_system sys = {0};
extern struct aiop_init_info g_init_data;

extern void     __sys_start(register int argc, register char **argv,
				register char **envp);
extern int icontext_init();

typedef struct t_sys_forced_object {
	fsl_handle_t        h_module;
} t_sys_forced_object_desc;

t_sys_forced_object_desc  sys_handle[FSL_OS_NUM_MODULES];


/*****************************************************************************/
fsl_handle_t sys_get_handle(enum fsl_os_module module, int num_of_ids, ...)
{
	UNUSED(num_of_ids);
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0))
		return NULL;

	return sys_handle[module].h_module;

}

/*****************************************************************************/
int sys_add_handle(fsl_handle_t h_module, enum fsl_os_module module,
				int num_of_ids, ...)
{
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0) || (num_of_ids > 1))
		return -EINVAL;

	sys_handle[module].h_module = h_module;

	return 0;
}

/*****************************************************************************/
int sys_remove_handle(enum fsl_os_module module, int num_of_ids, ...)
{
	UNUSED(num_of_ids);
	if ((module >= FSL_OS_NUM_MODULES) || (module < 0))
		return -EINVAL;


	sys_handle[module].h_module = NULL;

	return -ENAVAIL;
}

/*****************************************************************************/
__COLD_CODE static int sys_init_platform(void)
{
	int i, err = 0;
	int is_master_core = sys_is_master_core();

	for (i = 0 ; i < PLTFORM_NUM_OF_INIT_MODULES ; i++)
	{
		if (sys.platform_ops.modules[i].init)
		{
			if(sys.platform_ops.modules[i].is_single_core)
			{
				if(is_master_core)
				{
					err = sys.platform_ops.modules[i].init(
							sys.platform_ops.h_platform);
					if(err) return err;
				}

				sys_barrier();
			}
			else
			{
				err = sys.platform_ops.modules[i].init(
						sys.platform_ops.h_platform);
				if(err) return err;
			}
		}
	}

	return 0;
}


/*****************************************************************************/
static int sys_free_platform(void)
{
	int i, err = 0;
	int is_master_core = sys_is_master_core();

	for (i = PLTFORM_NUM_OF_INIT_MODULES - 1; i >= 0 ; i--)
	{
		if (sys.platform_ops.modules[i].free)
		{
			if(sys.platform_ops.modules[i].is_single_core)
			{
				if(is_master_core)
				{
					err = sys.platform_ops.modules[i].free(
							sys.platform_ops.h_platform);
					if(err) return err;
				}

				sys_barrier();
			}
			else
			{
				err = sys.platform_ops.modules[i].free(
						sys.platform_ops.h_platform);
				if(err) return err;
			}
		}
	}

	if (is_master_core) {
		err = platform_free(
			sys.platform_ops.h_platform);
		sys.platform_ops.h_platform = NULL;
	}

	sys_barrier();

	return err;
}

__COLD_CODE static uint32_t count_cores(uint64_t cores_mask)
{
    uint32_t count;
    for(count = 0; cores_mask > 0; cores_mask >>= 1) {
	if(cores_mask & 1 == 1)
	    count ++;
    }

    return count;
}

__COLD_CODE static void fill_system_parameters()
{
	uintptr_t reg_base = (uintptr_t)(SOC_PERIPH_OFF_AIOP_TILE \
		+ SOC_PERIPH_OFF_AIOP_CMGW \
		+ 0x02000000);/* PLTFRM_MEM_RGN_AIOP */
	uint32_t abrr_val = ioread32_ccsr(UINT_TO_PTR(reg_base + 0x90));

	sys.active_cores_mask  = abrr_val;

	sys.num_of_active_cores = count_cores(sys.active_cores_mask);
}

__COLD_CODE static int global_sys_init(void)
{
	struct platform_param platform_param;
	int err = 0;
	uintptr_t   aiop_base_addr;
	struct aiop_tile_regs *tile_regs;
	ASSERT_COND(sys_is_master_core());

	sys.runtime_flag = 0;

	fill_system_parameters();

	fill_platform_parameters(&platform_param);

	platform_early_init(&platform_param);

	/* Initialize memory management */
	err = sys_init_memory_management();
	if (err != 0) return err;

	/* Initialize Multi-Processing services as needed */
	err = sys_init_multi_processing();
	if (err != 0) return err;

	/* Platform init */
	err = platform_init(&(platform_param), &(sys.platform_ops));
	if (err != 0) return err;

	err = sys_add_handle(sys.platform_ops.h_platform,
		FSL_OS_MOD_SOC, 1, 0);
	if (err != 0) return err;

	aiop_base_addr = AIOP_PERIPHERALS_OFF + SOC_PERIPH_OFF_AIOP_TILE;
	err = sys_add_handle( (fsl_handle_t)aiop_base_addr,
	                                      FSL_OS_MOD_AIOP_TILE, 1, 0);
	if (err != 0) return err;

	tile_regs = (struct aiop_tile_regs *)aiop_base_addr;
	cmgw_init((void *)&tile_regs->cmgw_regs);

	err = sys_add_handle( (fsl_handle_t)&tile_regs->cmgw_regs,
	                                      FSL_OS_MOD_CMGW, 1, 0);
	if (err != 0) return err;

	return 0;
}

/*****************************************************************************/
__COLD_CODE int sys_init(void)
{
	int err = 0, is_master_core;
	uint32_t core_id = core_get_id();
	char pre_console_buf[PRE_CONSOLE_BUF_SIZE];

	sys.is_tile_master[core_id] = (int)(SYS_TILE_MASTERS_MASK \
						& (1ULL << core_id)) ? 1 : 0;
	sys.is_cluster_master[core_id] = (int)(SYS_CLUSTER_MASTER_MASK \
						& (1ULL << core_id)) ? 1 : 0;

	is_master_core = sys_is_master_core();

	if(is_master_core) {
		/* MUST BE before log_init() because it uses icontext API */
		icontext_init();
		memset( &pre_console_buf[0], 0, PRE_CONSOLE_BUF_SIZE);
		sys.p_pre_console_buf = &pre_console_buf[0];

		if(g_init_data.sl_info.log_buf_size){
			log_init();
			sys.print_to_buffer = TRUE;
		}
		err = global_sys_init();

		if (err != 0) {
			sys.p_pre_console_buf = NULL;
			return err;
		}

		/* signal all other cores that global initiation is done */
		sys.boot_sync_flag = SYS_BOOT_SYNC_FLAG_DONE;
	} else {
		while(!sys.boot_sync_flag) {}
	}

	err = sys_init_platform();
	sys_barrier();
	sys.p_pre_console_buf = NULL;
	sys_barrier();
	if (err != 0) return err;

	return 0;
}

/*****************************************************************************/
void sys_free(void)
{
	sys_free_platform();

	sys_free_multi_processing();
	sys_barrier();

	if (sys_is_master_core()) {

		/* Free memory management module */
		sys_free_memory_management();
	}
}
