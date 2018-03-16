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

#include "general.h"
#include "fsl_types.h"
#include "common/fsl_string.h"
#include "common/fsl_stdarg.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_platform.h"
#include "fsl_smp.h"
#include "fsl_io_ccsr.h"
#include "fsl_cmgw.h"
#include "fsl_console.h"
#include "fsl_system.h"
#include "fsl_dbg.h"
#include "fsl_aiop_common.h"
#include "fsl_errors.h"
#include "compat.h"
#include "sys.h"

#define __ERR_MODULE__  MODULE_UNKNOWN

#define SYS_TILE_MASTERS_MASK (0x00000001)
#define SYS_CLUSTER_MASTER_MASK (0x00001111)
#define SYS_BOOT_SYNC_FLAG_DONE (1)

/*
 * The AIOP boot process is made up of several sections (mentioned in the
 * variable section_name[]). Entering a section is recorded in the most
 * significant two bytes of the sys_status variable.
 * Additionally when an error occurs (in one of the sections) the boot process
 * ends immediatlly and the error is recorded in the least significant two
 * bytes of the sys_status variable.
 * The sys_status variable can be inspected to check the status of the boot
 * process and can be accessed in a several ways:
 * - inspecting the memory location of the variable from the host Linux OS
 *   (its physical address is printed on the AIOP console)
 * - greping the AIOP console for messages of the type:
 *   "Entering section: <section_name>"
 * - inspecting the ACGPR register (it contains the value of the sys_status at
 *   the end of the boot process).
 */
#define STATUS_SECTION_SHIFT	16
#define STATUS_SECTION_MASK	0xFFFF0000
#define STATUS_ERROR_MASK	0x0000FFFF

char *section_name[] = {
		"System Initialization",
		"Tile Initialization",
		"Global Early Initialization",
		"Apps Early Initialization",
		"Global Initialization",
		"Apps Initialization",
		"Global Post Initialization"
};

/* Global System Object */
/* It must be aligned to a double word boundary since the first member
 * it's a lock */
t_system sys __attribute__((aligned(8))) = {0};

extern struct aiop_init_info g_init_data;

extern void     __sys_start(register int argc, register char **argv,
				register char **envp);
extern int icontext_init();

extern int sys_init_memory_management(void);
extern int sys_free_memory_management(void);
extern int sys_init_multi_processing(void);
extern void sys_free_multi_processing(void);
extern void fill_platform_parameters(struct platform_param *platform_param);

typedef struct t_sys_forced_object {
	void *        h_module;
} t_sys_forced_object_desc;

t_sys_forced_object_desc  sys_handle[FSL_NUM_MODULES];

static uint64_t sys_status_lock __attribute__((aligned(8)));
volatile static int sys_status;

void fill_master_core_parameters();
void fill_system_parameters();
void sys_early_init(void);
int sys_init(void);
void sys_free(void);

__COLD_CODE int sys_get_global_error(void)
{
	return (sys_status & STATUS_ERROR_MASK);
}

__COLD_CODE void sys_set_global_error(int err)
{
	int section = (sys_status & STATUS_SECTION_MASK)
		       >> STATUS_SECTION_SHIFT;

	lock_spinlock(&sys_status_lock);
	if (!(sys_status & STATUS_ERROR_MASK) && err) {
		sys_status = (sys_status & STATUS_SECTION_MASK) |
			     (err & STATUS_ERROR_MASK);

		cmgw_report_boot_failure(sys_status);

		pr_err("Boot failed in section %d(%s) with error: 0x%08x\n",
		       section, section_name[section],
		       sys_status & STATUS_ERROR_MASK);
	}
	unlock_spinlock(&sys_status_lock);
}

__COLD_CODE void sys_enter_section(int section)
{
	int current_section = (sys_status & STATUS_SECTION_MASK)
			       >> STATUS_SECTION_SHIFT;

	lock_spinlock(&sys_status_lock);
	if (section > current_section) {
		sys_status = (sys_status & STATUS_ERROR_MASK) |
			     (section << STATUS_SECTION_SHIFT);

		pr_info("Entering section: %s\n", section_name[section]);
	}
	unlock_spinlock(&sys_status_lock);
}

/*****************************************************************************/
void * sys_get_handle(enum fsl_module module, int num_of_ids, ...)
{
	UNUSED(num_of_ids);
	if ((module >= FSL_NUM_MODULES) || (module < 0))
		return NULL;

	return sys_handle[module].h_module;

}

/*****************************************************************************/
int sys_add_handle(void * h_module, enum fsl_module module,
				int num_of_ids, ...)
{
	if ((module >= FSL_NUM_MODULES) || (module < 0) || (num_of_ids > 1))
		return -EINVAL;

	sys_handle[module].h_module = h_module;

	return 0;
}

/*****************************************************************************/
int sys_remove_handle(enum fsl_module module, int num_of_ids, ...)
{
	UNUSED(num_of_ids);
	if ((module >= FSL_NUM_MODULES) || (module < 0))
		return -EINVAL;


	sys_handle[module].h_module = NULL;

	return -ENAVAIL;
}

/*****************************************************************************/
__COLD_CODE static int sys_init_platform(void)
{
	int i, err = 0;
	int is_master_core = sys_is_master_core();
	struct pltform_module_desc *modules = sys.platform_ops.modules;
	void *handle = sys.platform_ops.h_platform;

	for (i = 0 ; i < PLTFORM_NUM_OF_INIT_MODULES ; i++) {
		if (modules[i].init) {
			if (modules[i].is_single_core) {
				if (is_master_core) {
					err = modules[i].init(handle);
					if (err)
						sys_set_global_error(err);
				}

				sys_barrier();
				if (sys_get_global_error())
					goto init_err;
			} else {
				err = modules[i].init(handle);
				if (err) {
					sys_set_global_error(err);
					sys_barrier();
					goto init_err;
				}

				sys_barrier();
				if (sys_get_global_error())
					goto init_err;
			}
		}
	}

	return 0;

init_err:
	for (; i >= 0; i--) {
		if (modules[i].free) {
			if (modules[i].is_single_core) {
				if (is_master_core)
					modules[i].free(handle);

				sys_barrier();
			} else {
				modules[i].free(handle);
			}
		}
	}

	return err;
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

__COLD_CODE void fill_system_parameters()
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

	fill_platform_parameters(&platform_param);

	platform_early_init(&platform_param);

	/* Initialize memory management */
	err = sys_init_memory_management();
	if (err != 0) return err;

	/* Platform init */
	err = platform_init(&(platform_param), &(sys.platform_ops));
	if (err != 0) return err;

	err = sys_add_handle(sys.platform_ops.h_platform,
		FSL_MOD_SOC, 1, 0);
	if (err != 0) return err;

	aiop_base_addr = AIOP_PERIPHERALS_OFF + SOC_PERIPH_OFF_AIOP_TILE;
	err = sys_add_handle( (void *)aiop_base_addr,
	                                      FSL_MOD_AIOP_TILE, 1, 0);
	if (err != 0) return err;

	tile_regs = (struct aiop_tile_regs *)aiop_base_addr;
	err = sys_add_handle( (void *)&tile_regs->cmgw_regs,
	                                      FSL_MOD_CMGW, 1, 0);
	if (err != 0) return err;

	return 0;
}

/*****************************************************************************/
__COLD_CODE void fill_master_core_parameters()
{
	uint32_t core_id = core_get_id();

	sys.is_tile_master[core_id] = (int)(SYS_TILE_MASTERS_MASK \
						& (1ULL << core_id)) ? 1 : 0;
	sys.is_cluster_master[core_id] = (int)(SYS_CLUSTER_MASTER_MASK \
						& (1ULL << core_id)) ? 1 : 0;
}

__COLD_CODE void sys_early_init(void)
{
	int is_master_core;

	fill_master_core_parameters();
	is_master_core = sys_is_master_core();

	if (is_master_core) {
		fill_system_parameters();
		sys_init_multi_processing();
		cmgw_init();

		sys.boot_sync_flag = SYS_BOOT_SYNC_FLAG_DONE;
	} else {
		while (!sys.boot_sync_flag)
			;
	}
}

__COLD_CODE int sys_init(void)
{
	int err = 0, is_master_core;
	char pre_console_buf[PRE_CONSOLE_BUF_SIZE];
	uint64_t stat_addr = 0x004B00000000 + (uint64_t)&sys_status;

	is_master_core = sys_is_master_core();
	if(is_master_core) {
		/* MUST BE before log_init() because it uses icontext API */
		icontext_init();

		/* zero pre_console_buf and link it */
		memset(&pre_console_buf[0], 0, PRE_CONSOLE_BUF_SIZE);
		sys.p_pre_console_buf = &pre_console_buf[0];

		if (g_init_data.sl_info.log_buf_size) {
			err = log_init();
			if (err) {
				sys_set_global_error(err);
				sys_barrier();
				goto init_err;
			}

			sys.print_to_buffer = TRUE;
		}

		pr_info("System status at 0x%08x%08x\n",
			upper_32_bits(stat_addr), lower_32_bits(stat_addr));

		err = global_sys_init();
		if (err) {
			sys_set_global_error(err);
			sys_barrier();
			goto init_err;
		}
	} else {
		sys_barrier();
		if (sys_get_global_error())
			goto init_err;
	}

	err = sys_init_platform();
	if (err)
		sys_set_global_error(err);

	sys_barrier();

init_err:
	sys.p_pre_console_buf = NULL;

	return err;
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
