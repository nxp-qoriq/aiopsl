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

#include "fsl_types.h"
#include "common/fsl_stdio.h"
#include "fsl_smp.h"
#include "fsl_dbg.h"
#include "fsl_cmgw.h"
#include "fsl_rcu.h"
#include "fsl_system.h"
#include "fsl_errors.h"
#include "sys.h"

extern void sys_early_init(void);
extern int sys_init(void);
extern void sys_free(void);
extern int global_early_init(void);
extern int global_init(void);
extern void global_free(void);
extern int tile_init(void);
extern int cluster_init(void);
extern int global_post_init(void);
extern int apps_early_init(void);
extern int apps_init(void);
extern void core_ready_for_tasks(void);

#if (STACK_OVERFLOW_DETECTION == 1)
extern char _stack_addr[]; /* Starting address for stack */
extern char _stack_end[];  /* Address after end byte of stack */

int configure_stack_overflow_detection(void);
__COLD_CODE int configure_stack_overflow_detection(void)
{
	if(((uint32_t)_stack_end) >= ((uint32_t)_stack_addr))
	{
		/* Stack end address cannot be larger than stack start address */
		return -ENOMEM;
	}

	/* DBCR2 */
	booke_set_spr_DBCR2(booke_get_spr_DBCR2() | 0x00c00000);

	/* DBCR4 */
	booke_set_spr_DBCR4(booke_get_spr_DBCR4() | 0x00000080); /* DAC1CFG */
	booke_instruction_sync();

	/* DBCR0 */
	booke_set_spr_DBCR0(booke_get_spr_DBCR0() | 0x400f0000);
	booke_instruction_sync();

	/* initiate DAC registers */
	booke_set_spr_DAC1((uint32_t)_stack_end);
	booke_set_spr_DAC2((uint32_t)_stack_addr);

	return 0;
}
#endif

/*****************************************************************************/
/* Allow main() to be redefined */
__declspec(weak) int main(int argc, char *argv[]);
int main(int argc, char *argv[])
{
	int err = 0;
	int is_master_core;
	UNUSED(argc);UNUSED(argv);

	/* Initiate small data area pointers at task initialization */
	asm {
		mtdcr dcr469,r2 // INITR2
		mtdcr dcr470,r13// INITR13
	}

	sys_early_init();

#if (STACK_OVERFLOW_DETECTION == 1)
	err = configure_stack_overflow_detection();
	if (err) {
		sys_set_global_error(err);
		sys_barrier();
		goto sys_init_err;
	}

	sys_barrier();
	if (sys_get_global_error())
		goto sys_init_err;
#endif

	is_master_core = sys_is_master_core();

	/* sys init */
	err = sys_init();
	if (sys_get_global_error())
		goto sys_init_err;

	if(is_master_core) {
		if(cmgw_get_ntasks() > 2 /*4-tasks*/) {
			pr_warn("More then 4 AIOP tasks/core.\n");
		}
	}

	/* tile init */
	sys_enter_section(SECTION_TILE_INIT);

	if (is_master_core) {
		err = tile_init();
		if(err) {
			sys_set_global_error(err);
			sys_barrier();
			goto tile_init_err;
		}
	}

	sys_barrier();
	if (sys_get_global_error())
		goto tile_init_err;

	/* global early init */
	sys_enter_section(SECTION_GLOBAL_EARLY_INIT);

	if (is_master_core) {
		err = global_early_init();
		if(err) {
			sys_set_global_error(err);
			sys_barrier();
			goto tile_init_err;
		}
	}

	sys_barrier();
	if (sys_get_global_error())
		goto tile_init_err;

	/* apps early init */
	sys_enter_section(SECTION_APPS_EARLY_INIT);

	if (is_master_core) {
		err = apps_early_init();
		if(err) {
			sys_set_global_error(err);
			sys_barrier();
			goto tile_init_err;
		}
	}

	sys_barrier();
	if (sys_get_global_error())
		goto tile_init_err;

	/* global init */
	sys_enter_section(SECTION_GLOBAL_INIT);

	if (is_master_core) {
		err = global_init();
		if(err) {
			sys_set_global_error(err);
			sys_barrier();
			goto tile_init_err;
		}
	}

	sys_barrier();
	if (sys_get_global_error())
		goto tile_init_err;

	if (is_master_core)
		pr_info("Running applications\n");

	/* apps init */
	sys_enter_section(SECTION_APPS_INIT);

	if (is_master_core) {
		err = apps_init();
		if (err) {
			sys_set_global_error(err);
			sys_barrier();
			goto apps_init_err;
		}
	}

	sys_barrier();
	if (sys_get_global_error())
		goto apps_init_err;

	/* global post init */
	sys_enter_section(SECTION_GLOBAL_POST_INIT);

	if (is_master_core) {
		err = global_post_init();
		if(err) {
			sys_set_global_error(err);
			sys_barrier();
			goto apps_init_err;
		}
	}

	sys_barrier();
	if (sys_get_global_error())
		goto apps_init_err;

	if (is_master_core) {
		/* Avoid applications hang on rcu_synchronize() or
		 * cdma_ephemeral_reference_sync() call if an unnecessary lock
		 * was taken by applications in the call-back functions called
		 * from apps_early_init or apps_init */
		rcu_read_unlock();
	}

	core_ready_for_tasks();

	if (is_master_core)
		pr_info("complete. freeing resources and going out ...\n");

	sys_barrier();

apps_init_err:
	global_free();

tile_init_err:
	sys_free();

sys_init_err:
	cmgw_update_core_boot_completion();

	/* Keep the PC inside main so that AIOP doesn't execute random code */
	while (1) {}

	return err;
}
