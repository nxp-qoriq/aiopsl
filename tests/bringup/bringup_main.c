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
#include "fsl_smp.h"
#include "fsl_dbg.h"
#include "fsl_system.h"
#include "fsl_spinlock.h"
#include "bringup_tests.h"

int configure_stack_overflow_detection(void);

/* Minimal sys_init() so that
	sys_is_master_core()
	sys_is_core_active()
	sys_get_num_of_cores()
	sys_barrier()
 will work
 */

void * memset(void * dst, int val, size_t n);
void fill_master_core_parameters();
void fill_system_parameters();
int sys_init_multi_processing(void);
int __sys_init(void);

extern t_system sys;

__COLD_CODE int __sys_init(void)
{
	// sys_init()
#define PRE_CONSOLE_BUF_SIZE    (4 * 1024)
#define SYS_BOOT_SYNC_FLAG_DONE (1)

	int err = 0, is_master_core;
	static char pre_console_buf[PRE_CONSOLE_BUF_SIZE];

	fill_master_core_parameters();
	is_master_core = sys_is_master_core();

	if (is_master_core) {
		memset( &pre_console_buf[0], 0, PRE_CONSOLE_BUF_SIZE);
		sys.p_pre_console_buf = &pre_console_buf[0];

		sys.print_to_buffer = FALSE;

		// err = global_sys_init();
		fill_system_parameters();
		err = sys_init_multi_processing();

		if (err != 0) {
			sys.p_pre_console_buf = NULL;
			return err;
		}

		/* signal all other cores that global initiation is done */
		sys.boot_sync_flag = SYS_BOOT_SYNC_FLAG_DONE;
	} else {
		while(!sys.boot_sync_flag) {}
	}

	// sys_init_platform -> pltfrm_init_core_cb()
	booke_generic_irq_init();
	sys_barrier();
	if (err != 0) return err;

	return 0;
#undef PRE_CONSOLE_BUF_SIZE
#undef SYS_BOOT_SYNC_FLAG_DONE
}


/*****************************************************************************/
static uint64_t global_lock __attribute__((aligned(8)));

int main(int argc, char *argv[])
{
	int err = 0;
	int is_master_core;
	UNUSED(argc);
	UNUSED(argv);

	/* Initiate small data area pointers at task initialization */
	asm {
		mtdcr dcr469,r2 // INITR2
		mtdcr dcr470,r13// INITR13
	}

	err |= configure_stack_overflow_detection();

	err |= __sys_init();
	is_master_core = sys_is_master_core();

	if (is_master_core) {
		volatile int tmp = 0;
		while (tmp == 1) {
			int tmp2 = 13;
		}
		err |= console_print_init();
	}
	sys_barrier();

#if (TEST_CONSOLE_PRINT == ON)
	err |= console_print_test();
#endif

#if (TEST_MEM_ACCESS == ON)
	/* memory access test */
	err |= mem_standalone_init();
	err |= mem_test();
#endif

/* Those 2 tests can't be tested together */
#if (TEST_EXCEPTIONS == ON)
	err |= exceptions_test();
#elif (TEST_STACK_OVF == ON)
	err |= stack_ovf_test();
#endif

#if (TEST_SINGLE_CLUSTER == ON)
	err |= single_cluster_test();
#endif

#if (TEST_MULTI_CLUSTER == ON)
	err |= multi_cluster_test();
#endif

#if (TEST_SPINLOCKS == ON)
	err |= spinlock_standalone_init();
	err |= spinlock_test();
#endif

#if (TEST_AIOP_MC_CMD == ON)
	err |= aiop_mc_cmd_test();
#endif

#if (TEST_DPBP == ON)
	if (is_master_core) {
		err |= dpbp_init();
		err |= dpbp_test();
	} else {
		while(1) {}
	}
#endif /* TEST_DPBP */

#if (TEST_DPNI == ON)
	if (is_master_core) {
		err |= dpni_init();
		err |= dpni_test();
	}
	sys_barrier();
#endif /* TEST_DPNI */

#if (TEST_BUFFER_POOLS == ON)
	if (is_master_core) {
		global_lock = 0;
		err |= buffer_pool_init();
	}
	sys_barrier();
	lock_spinlock(&global_lock);
	err |= buffer_pool_test();
	unlock_spinlock(&global_lock);
#endif /* TEST_BUFFER_POOLS */

#if (TEST_GPP_DDR == ON)
	if (is_master_core) {
		global_lock = 0;
		err |= gpp_sys_ddr_init();
	}
	sys_barrier();
	lock_spinlock(&global_lock);
	err |= gpp_sys_ddr_test();
	unlock_spinlock(&global_lock);
#endif

	if (err) {
		pr_info(_TEST_NAME_ ", TEST FAILED\n");
		while(1) {}
	} else {
		pr_info(_TEST_NAME_ ", TEST PASSED\n");
		while(1) {}
	}

	return err;
}
